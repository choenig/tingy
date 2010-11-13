/****************************************************************************
** Author: Christian Hoenig <tingy@mail.hoenig.cc>
** Copyright (c) 2010 Christian Hoenig
**
** This file is part of Tingy.
**
** Tingy is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** Tingy is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy (license.txt) of the
** GNU General Public License along with Tingy.
** If not, see <http://www.gnu.org/licenses/>.
****************************************************************************/
#include "caldavstorage.h"

#include <core/settings.h>
#include <core/taskmodel.h>
#include <util/log.h>
#include <util/util.h>

#include <QBuffer>
#include <QDebug>
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QRegExp>
#include <QSslConfiguration>
#include <QTimer>

#define NL "\n"

class CalDavNetworkManager : public QNetworkAccessManager
{
    Q_OBJECT

public:
    CalDavNetworkManager(QObject *parent)
        : QNetworkAccessManager(parent)
    {
        url_.setScheme(Settings::CalDavStorage::UseSSL() ? "https" : "http");
        url_.setHost(Settings::CalDavStorage::Hostname());
        url_.setPort(Settings::CalDavStorage::Port());
        url_.setPath(Settings::CalDavStorage::Taskdir());

        connect(this, SIGNAL(finished(QNetworkReply*)), this, SLOT(onReplyFinished(QNetworkReply*)));
    }

    void listAvailableHrefsAndETags(QHash<TaskId, QString> & availableEtags)
    {
        QNetworkRequest request;
        request.setUrl(url_);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/xml; charset=\"utf-8\"");
        request.setRawHeader("Depth", "1");
        request.setRawHeader("Authorization", authorizationString());

        QBuffer outBuffer;
        outBuffer.setData("<?xml version=\"1.0\" encoding=\"utf-8\" ?>" NL
                           "<C:calendar-query xmlns:C=\"urn:ietf:params:xml:ns:caldav\">" NL
                           "  <D:prop xmlns:D=\"DAV:\">" NL
                           "    <D:getetag/>" NL
                         //"      <C:calendar-data />" NL
                           "  </D:prop>" NL
                           "  <C:filter>" NL
                           "    <C:comp-filter name=\"VCALENDAR\">" NL
                           "      <C:comp-filter name=\"VTODO\">" NL
                         //"        <C:prop-filter name=\"COMPLETED\">" NL
                         //"          <C:is-not-defined/>" NL
                         //"        </C:prop-filter>" NL
                         //"        <C:prop-filter name=\"STATUS\">" NL
                         //"          <C:text-match" negate-condition=\"yes\">CANCELLED</C:text-match>" NL
                         //"        </C:prop-filter>" NL
                         //"      </C:comp-filter>" NL
                           "    </C:comp-filter>" NL
                           "  </C:filter>" NL
                           "</C:calendar-query>");

        sendCustomRequest(request, "REPORT", &outBuffer);
        if (loop_.exec() != 0) {
            return;
        }

//        qDebug() << replyString_;

        const QStringList parts = split("<D:response>", "</D:response>", replyString_);
        foreach (const QString & part, parts) {

            const QStringList hrefs = split("<D:href>",    "</D:href>",    part);
            const QStringList etags = split("<D:getetag>", "</D:getetag>", part);
            if (hrefs.size() != 1 || etags.size() != 1) {
                log(Log::Error) << "could not parse href and etag:" << part;
                continue;
            }

            TaskId taskId = taskIdFromHRef(hrefs.first());
            if (taskId.isNull()) {
                log(Log::Error) << "invalid TaskId, could not parse href and etag:" << part;
                continue;
            }

            availableEtags[taskId] = unescapeFromXml(etags.first());
        }
    }

    QList<Task> report(QHash<TaskId, QString> & availableEtags)
    {
        QNetworkRequest request;
        request.setUrl(url_);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/xml; charset=\"utf-8\"");
        request.setRawHeader("Depth", "1");
        request.setRawHeader("Authorization", authorizationString());

        QBuffer outBuffer;
        outBuffer.setData("<?xml version=\"1.0\" encoding=\"utf-8\" ?>" NL
                     "<C:calendar-query xmlns:D=\"DAV:\" xmlns:C=\"urn:ietf:params:xml:ns:caldav\">" NL
                     "  <D:prop>" NL
                     "      <D:getetag/>" NL
                     "      <C:calendar-data />" NL
                     "  </D:prop>" NL
                     "  <C:filter>" NL
                     "      <C:comp-filter name=\"VCALENDAR\">" NL
                     "          <C:comp-filter name=\"VTODO\">" NL
                     "              <C:prop-filter name=\"COMPLETED\">" NL
                     "                  <C:is-not-defined/>" NL
                     "              </C:prop-filter>" NL
                     "          </C:comp-filter>" NL
                     "      </C:comp-filter>" NL
                     "  </C:filter>" NL
                     "</C:calendar-query>");

        sendCustomRequest(request, "REPORT", &outBuffer);
        if (loop_.exec() != 0) {
            return QList<Task>();
        }

//        qDebug() << replyString_;

        QList<Task> retval;
        QStringList parts = split("<D:response>", "</D:response>", replyString_);
        foreach (const QString & part, parts) {
            QStringList curEtags = split("<D:getetag>", "</D:getetag>", part);
            QStringList curTasks = split("BEGIN:VCALENDAR", "END:VCALENDAR", part);

            if (curEtags.size() != 1 || curTasks.size() != 1) {
                log(Log::Error) << "could not parse task:" << part;
                continue;
            }

            Task task = Task::fromICal(unescapeFromXml(curTasks.first()));
            if (!task.isValid()) {
                log(Log::Error) << "could not parse task:" << part;
                continue;
            }

            // remember task
            retval << task;

            // remember loaded eTags
            availableEtags[task.getId()] = unescapeFromXml(curEtags.first());
        }

        return retval;
    }

    /// @return the etag of the new item
    QString put(const Task & task)
    {
        const QUrl taskUrl = urlFromTaskId(task.getId());

        QNetworkRequest request;
        request.setUrl( taskUrl );
        request.setRawHeader("If-None-Match", "*");
        request.setHeader(QNetworkRequest::ContentTypeHeader, "text/calendar");
        request.setRawHeader("Authorization", authorizationString());

        QBuffer outBuffer;
        outBuffer.setData(task.toICal().toUtf8());
        sendCustomRequest(request, "PUT", &outBuffer);

        if (loop_.exec() != 0) return QString();
        return replyETag_;
    }

    bool remove(const TaskId & taskId)
    {
        const QUrl taskUrl = urlFromTaskId(taskId);

        QNetworkRequest request;
        request.setUrl( taskUrl );
        request.setHeader(QNetworkRequest::ContentTypeHeader, "text/calendar");
        request.setRawHeader("Authorization", authorizationString());

        QBuffer outBuffer;
        outBuffer.setData(QByteArray());
        sendCustomRequest(request, "DELETE", &outBuffer);

        return loop_.exec() == 0;
    }

    Task load(const TaskId & taskId)
    {
        QNetworkRequest request;
        request.setUrl(url_);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/xml; charset=\"utf-8\"");
        request.setRawHeader("Depth", "1");
        request.setRawHeader("Authorization", authorizationString());

        QBuffer outBuffer;
        outBuffer.setData(QString(
                     "<?xml version=\"1.0\" encoding=\"utf-8\" ?>" NL
                     "<C:calendar-query xmlns:D=\"DAV:\" xmlns:C=\"urn:ietf:params:xml:ns:caldav\">" NL
                     "  <D:prop>" NL
                     "      <D:getetag/>" NL
                     "      <C:calendar-data />" NL
                     "  </D:prop>" NL
                     "  <C:filter>" NL
                     "      <C:comp-filter name=\"VCALENDAR\">" NL
                     "          <C:comp-filter name=\"VTODO\">" NL
                     "              <C:prop-filter name=\"UID\">" NL
                     "                  <C:text-match collation=\"i;octet\">%1</C:text-match>" NL
                     "              </C:prop-filter>" NL
                     "          </C:comp-filter>" NL
                     "      </C:comp-filter>" NL
                     "  </C:filter>" NL
                     "</C:calendar-query>").arg(taskId.toString()).toUtf8());

        sendCustomRequest(request, "REPORT", &outBuffer);
        if (loop_.exec() != 0) {
            return Task();
        }

//        qDebug() << replyString_;

        QStringList parts = split("<D:response>", "</D:response>", replyString_);
        if (parts.size() != 1) return Task();

        const QString & part = parts.first();
        QStringList curEtags = split("<D:getetag>", "</D:getetag>", part);
        QStringList curTasks = split("BEGIN:VCALENDAR", "END:VCALENDAR", part);

        if (curEtags.size() != 1 || curTasks.size() != 1) {
            log(Log::Error) << "could not parse task:" << part;
            return Task();
        }

        Task task = Task::fromICal(unescapeFromXml(curTasks.first()));
        if (!task.isValid()) {
            log(Log::Error) << "could not parse task:" << part;
            return Task();
        }

        return task;
    }

private slots:
    void onReplyFinished(QNetworkReply * reply)
    {
        if (reply->error() != QNetworkReply::NoError) {
            log << reply->errorString();
        }

        replyString_ = reply->readAll();
        replyETag_   = reply->rawHeader("ETag");
        loop_.exit(reply->error() == QNetworkReply::NoError ? 0 : 1);
    }

private:
    QUrl urlFromTaskId(const TaskId & taskId) const
    {
        QUrl taskUrl(url_);
        taskUrl.setPath(taskUrl.path() + "/" + taskId.toString() + ".ics");
        return taskUrl;
    }

    QByteArray authorizationString() const
    {
        QByteArray retval;
        retval.append("Basic ");
        retval.append(QString(Settings::CalDavStorage::Username()+":"+Settings::CalDavStorage::Password()).toUtf8().toBase64());
        return retval;
    }

    TaskId taskIdFromHRef(const QString & href) const
    {
        QRegExp idRegExp(".*/(\\{.+\\})\\.ics");
        if (!idRegExp.exactMatch(href)) {
            return TaskId();
        }
        return TaskId::fromString(idRegExp.cap(1));
    }

private:
    QEventLoop loop_;
    QUrl url_;
    QString replyString_;
    QString replyETag_;
};

#include "caldavstorage.moc"

//
//
// CalDavStorage

CalDavStorage::CalDavStorage(QObject *parent)
    : QObject(parent), networkMgr_(new CalDavNetworkManager(this))
{
    QTimer * reloadTimer = new QTimer(this);
    reloadTimer->setInterval(60 * 1000);
    connect(reloadTimer, SIGNAL(timeout()), this, SLOT(checkForChanges()));
    reloadTimer->start();
}

QList<Task> CalDavStorage::loadTasks()
{
    return networkMgr_->report(knownEtags_);
}

bool CalDavStorage::saveTasks(const QList<Task> & tasks)
{
    bool retval = true;
    foreach (const Task & task, tasks) {
        retval = retval && addTask(task);
    }
    return retval;

}

bool CalDavStorage::addTask(const Task & task)
{
    const QString eTag = networkMgr_->put(task);
    if (eTag.isNull()) return false;
    knownEtags_[task.getId()] = eTag;
    return true;
}

bool CalDavStorage::updateTask(const Task & task, bool doneChanged)
{
    Q_UNUSED(doneChanged);

    const QString eTag = networkMgr_->put(task);
    if (eTag.isNull()) return false;

    knownEtags_[task.getId()] = eTag;
    return true;
}

bool CalDavStorage::removeTask(const TaskId & taskId)
{
    // if we don't know the etag, we don't care about deleting the task
    if (!knownEtags_.contains(taskId)) return true;

    bool ok = networkMgr_->remove(taskId);
    if (!ok) return false;

    knownEtags_.remove(taskId);
    return true;
}

void CalDavStorage::checkForChanges()
{
    QHash<TaskId, QString> newETags;
    networkMgr_->listAvailableHrefsAndETags(newETags);

    foreach (const TaskId & taskId, newETags.keys()) {
        if (knownEtags_.contains(taskId)) {
            if (knownEtags_[taskId] != newETags[taskId]) {
                Task task = networkMgr_->load(taskId);
                TaskModel::instance()->updateTask(task);
            }
        } else {
            Task task = networkMgr_->load(taskId);
            TaskModel::instance()->addTask(task);
        }
    }

    // remove tags that have disapeared
    foreach (const TaskId & taskId, knownEtags_.keys()) {
        if (!newETags.contains(taskId)) {
            // remove the taskId from the knownETags so that a later removeTask() will not try to delete the task
            knownEtags_.remove(taskId);
            TaskModel::instance()->removeTask(taskId);
        }
    }
}
