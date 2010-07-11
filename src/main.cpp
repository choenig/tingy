#include <core/taskmodel.h>
#include <widgets/mainwindow.h>

#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    TaskModel taskModel;

    MainWindow mainWindow;
    mainWindow.show();

    return a.exec();
}
