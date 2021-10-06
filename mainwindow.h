#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "image.h"
//#include "clickablelabel.h"
#include <QWidget>
#include <QMenu>
#include <QMainWindow>
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void open();

private:
    void createActions();
    void createMenus();
    bool maybeExit();
    Image *image;
    QMenu *fileMenu;
    QAction *openAct;
    QAction *exitAct;
};
#endif //MAINWINDOW_H
