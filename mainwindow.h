#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "imagewidget.h"
#include "rightbar.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void open();
    void slotWidget(QSize s);

private:
    void createActions();
    void createMenus();
    bool maybeExit();
    ImageWidget *widget;
    RightBar *bar;
    QMenu *fileMenu;
    QAction *openAct;
    QAction *exitAct;
};
#endif //MAINWINDOW_H
