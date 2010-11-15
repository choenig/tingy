/****************************************************************************
** Author: Christian Hoenig <tingy@mail.hoenig.cc>
** Copyright (c) 2010 Christian Hoenig
**
** This file is part of tingy.
**
** tingy is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** tingy is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy (license.txt) of the
** GNU General Public License along with tingy.
** If not, see <http://www.gnu.org/licenses/>.
****************************************************************************/
#include <core/clock.h>
#include <core/settings.h>
#include <core/taskmodel.h>
#include <storage/caldavstorage.h>
#include <storage/filestorage.h>
#include <storage/networkstorage.h>
#include <widgets/mainwindow.h>

#include <QApplication>
#include <QTimer>
#include <QLocale>

// tingy colors
// "#f8faec" very light
// "#b7d24d" middle grün
// "#9ebc20" dark grün
// "#96bb00" darkest green for fonts

int main(int argc, char * argv[])
{
    QApplication a(argc, argv);

    // set application details
    QApplication::setApplicationName("tingy");
    QApplication::setApplicationVersion("1.0.0");

    // do not close the applicaton when the quicktingy is closed
    a.setQuitOnLastWindowClosed(false);

    // use 'de' as default locale
    QLocale::setDefault(QLocale(QLocale::German, QLocale::Germany));

    // init fake time
    Clock clock;//(QDateTime(QDate(2010,7,23), QTime(23,59,45)));

    // initialize taskModel end corresponding storage modules
    TaskModel taskModel;
    QTimer::singleShot(0, TaskModel::instance(), SLOT(init()));

    // initialize storage modules
    if (Settings::FileStorage::Enabled())    taskModel.instance()->addStorageEngine(new FileStorage);
    if (Settings::NetworkStorage::Enabled()) taskModel.instance()->addStorageEngine(new NetworkStorage);
    if (Settings::CalDavStorage::Enabled())  taskModel.instance()->addStorageEngine(new CalDavStorage);

    // init the mainWindow
    MainWindow mainWindow;
    mainWindow.show();

    return a.exec();
}
