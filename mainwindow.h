#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "imagewidget.h"
#include "rightbar.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    void createActions();
    void createMenus();
    bool maybeExit();
    ImageWidget *widget;
    RightBar *bar;
    QMenu *fileMenu;
    QAction *openAct;
    QAction *exitAct;

public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void open();
    void slotWidget(QSize s);
};
#endif //MAINWINDOW_H
