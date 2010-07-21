#include <core/clock.h>
#include <core/filestorage.h>
#include <core/networkstorage.h>
#include <core/taskmodel.h>
#include <widgets/mainwindow.h>

#include <QApplication>
#include <QTimer>
#include <QLocale>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);

    // use 'de' as default language
    QLocale::setDefault(QLocale(QLocale::German, QLocale::Germany));

    // init fake time
    Clock clock;//(QDateTime(QDate(2010,7,17), QTime(23,59,45)));

    NetworkStorage netStorage;

    // initialize taskModel end corresponding fileStorage
    TaskModel taskModel;
    FileStorage fileStorage;
    QTimer::singleShot(0, &fileStorage, SLOT(restoreFromFiles()));

    // init the mainWindow
    MainWindow mainWindow;
    mainWindow.show();

    return a.exec();
}
