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
// "#b7d24d" middle grün
// "#9ebc20" dark grün
// "#96bb00" darkest green for fonts

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);

    // use 'de' as default locale
    QLocale::setDefault(QLocale(QLocale::German, QLocale::Germany));

    // init fake time
    Clock clock;//(QDateTime(QDate(2010,7,23), QTime(23,59,45)));

    // initialize taskModel end corresponding storage modules
    TaskModel taskModel;

    NetworkStorage netStorage;
    QTimer::singleShot(0, &netStorage, SLOT(restoreFromFiles()));
//    FileStorage fileStorage;
//    QTimer::singleShot(0, &fileStorage, SLOT(restoreFromFiles()));

    // init the mainWindow
    MainWindow mainWindow;
    mainWindow.show();

    return a.exec();
}
