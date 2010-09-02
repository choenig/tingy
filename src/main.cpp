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
    if (Settings::FileStorage::Enabled())   taskModel.instance()->addStorageEngine(new FileStorage);
    if (Settings::NetworkStorage::Enabled()) taskModel.instance()->addStorageEngine(new NetworkStorage);
    if (Settings::CalDavStorage::Enabled())  taskModel.instance()->addStorageEngine(new CalDavStorage);

    // init the mainWindow
    MainWindow mainWindow;
    mainWindow.show();

    return a.exec();
}
