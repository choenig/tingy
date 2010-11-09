#include "caldavstorage.h"

#include <core/settings.h>
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

    QHash<QString, QString> listAvailableHrefsAndETags()
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
                           "      <C:calendar-data />" NL
                           "  </D:prop>" NL
                           "  <C:filter>" NL
                           "    <C:comp-filter name=\"VCALENDAR\">" NL
                           "      <C:comp-filter name=\"VTODO\">" NL
//                           "        <C:prop-filter name=\"COMPLETED\">" NL
//                           "          <C:is-not-defined/>" NL
//                           "        </C:prop-filter>" NL
//                           "        <C:prop-filter name=\"STATUS\">" NL
//                           "          <C:text-match" negate-condition=\"yes\">CANCELLED</C:text-match>" NL
//                           "        </C:prop-filter>" NL
//                           "      </C:comp-filter>" NL
                           "    </C:comp-filter>" NL
                           "  </C:filter>" NL
                           "</C:calendar-query>");

        sendCustomRequest(request, "REPORT", &outBuffer);
        if (loop_.exec() != 0) {
            return QHash<QString,QString>();
        }

        qDebug() << replyString_;

        QHash<QString, QString> retval;
        const QStringList parts = split("<D:response>", "</D:response>", replyString_);
        foreach (const QString & part, parts) {

            const QStringList hrefs = split("<D:href>",    "</D:href>",    part);
            const QStringList etags = split("<D:getetag>", "</D:getetag>", part);
            if (hrefs.size() != 1 || etags.size() != 1) {
                log(Log::Error) << "could not parse href and etag:" << part;
                continue;
            }

            retval[hrefs.first()] = etags.first();
        }
        return retval;
    }

    QList<Task> report()
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

        QList<Task> tasks;
        QStringList parts = split("BEGIN:VCALENDAR", "END:VCALENDAR", replyString_);
        foreach (const QString & part, parts) {
            tasks << Task::fromICal(part);
        }
        return tasks;
    }

    bool put(const Task & task)
    {
        QNetworkRequest request;
        request.setUrl( urlFromTaskId(task.getId()) );
        request.setRawHeader("If-None-Match", "*");
        request.setHeader(QNetworkRequest::ContentTypeHeader, "text/calendar");
        request.setRawHeader("Authorization", authorizationString());

        QBuffer outBuffer;
        outBuffer.setData(task.toICal().toUtf8());
        sendCustomRequest(request, "PUT", &outBuffer);
        return loop_.exec() == 0;
    }

    bool remove(const TaskId & taskId)
    {
        QNetworkRequest request;
        request.setUrl( urlFromTaskId(taskId) );
        request.setHeader(QNetworkRequest::ContentTypeHeader, "text/calendar");
        request.setRawHeader("Authorization", authorizationString());

        QBuffer outBuffer;
        outBuffer.setData(QByteArray());
        sendCustomRequest(request, "DELETE", &outBuffer);
        return loop_.exec() == 0;
    }

private slots:
    void onReplyFinished(QNetworkReply * reply)
    {
        if (reply->error() != QNetworkReply::NoError) {
            log << reply->errorString();
        }

        replyString_ = reply->readAll();
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

private:
    QEventLoop loop_;
    QUrl url_;
    QString replyString_;
};

#include "caldavstorage.moc"

namespace {
    CalDavNetworkManager * networkMgr;
}

//
// CalDavStorage

CalDavStorage::CalDavStorage(QObject *parent) :
    QObject(parent)
{
    networkMgr = new CalDavNetworkManager(this);
}


QList<Task> CalDavStorage::loadTasks()
{
    return networkMgr->report();
}

bool CalDavStorage::saveTasks(const QList<Task> & tasks)
{
    //    QHash<QString, QString> availableHRefs = networkMgr->listAvailableHrefsAndETags();
    //    foreach (const QString & href, availableHRefs.keys()) {
    //        qDebug() << href << availableHRefs[href];
    //    }

    //    return false;

    bool retval = true;
    foreach (const Task & task, tasks) {
        retval = retval && addTask(task);
    }
    return retval;

}

bool CalDavStorage::addTask(const Task & task)
{
    return networkMgr->put(task);
}

bool CalDavStorage::updateTask(const Task & task, bool doneChanged)
{
    Q_UNUSED(doneChanged);

    return networkMgr->put(task);
}

bool CalDavStorage::removeTask(const TaskId & taskId)
{
    return networkMgr->remove(taskId);
}
