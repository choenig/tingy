#include <core/clock.h>
#include <core/filestorage.h>
#include <core/taskmodel.h>
#include <widgets/mainwindow.h>

#include <QApplication>
#include <QTimer>
#include <QLocale>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // init fake time
    // Clock::init(QDateTime(QDate(2010,7,17), QTime(23,59,45)));

    // use 'de' as default language
    QLocale::setDefault(QLocale(QLocale::German, QLocale::Germany));

    TaskModel taskModel;
    FileStorage fileStorage;
    QTimer::singleShot(0, &fileStorage, SLOT(restoreFromFiles()));

    MainWindow mainWindow;
    mainWindow.show();

    return a.exec();
}
