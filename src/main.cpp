#include <core/filestorage.h>
#include <core/taskmodel.h>
#include <widgets/mainwindow.h>

#include <QApplication>
#include <QTimer>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    TaskModel taskModel;

    FileStorage fileStorage;
    QTimer::singleShot(0, &fileStorage, SLOT(restoreFromFiles()));

    MainWindow mainWindow;
    mainWindow.show();

    return a.exec();
}
