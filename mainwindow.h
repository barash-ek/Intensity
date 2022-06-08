#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "imagewidget.h"
#include "rightbar.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    ImageWidget *widget;
    RightBar *bar;
    QMenu *fileMenu;
    QAction *openAct;
    QAction *exitAct;

    void createActions();
    void createMenus();
    bool maybeExit();

public:
    explicit MainWindow(QWidget *parent = Q_NULLPTR);

protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private slots:
    void open();
};
#endif //MAINWINDOW_H
