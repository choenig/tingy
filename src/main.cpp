#include <core/clock.h>
#include <core/filestorage.h>
#include <core/networkstorage.h>
#include <core/settings.h>
#include <core/taskmodel.h>
#include <widgets/mainwindow.h>

#include <QApplication>
#include <QTimer>
#include <QLocale>

// tingy colors
// "#f8faec" very light
// "#b7d24d" middle gr�n
// "#9ebc20" dark gr�n
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

    // initialize storage modules
    FileStorage * fileStorage = new FileStorage(&a);
    NetworkStorage * netStorage = 0;
    if (Settings::NetworkStorage::Enabled()) {
        netStorage = new NetworkStorage(&a);
        QTimer::singleShot(0, netStorage, SLOT(restoreFromFiles()));
    } else {
        QTimer::singleShot(0, fileStorage, SLOT(restoreFromFiles()));
    }

    // init the mainWindow
    MainWindow mainWindow;
    mainWindow.show();

    return a.exec();
}
