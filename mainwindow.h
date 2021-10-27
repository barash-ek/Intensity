#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "image.h"
#include "rightbar.h"
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
    void slot_im(QSize s);

private:
    void createActions();
    void createMenus();
    bool maybeExit();
    Image *image;
    rightBar *bar;
    QMenu *fileMenu;
    QAction *openAct;
    QAction *exitAct;
};
#endif //MAINWINDOW_H
