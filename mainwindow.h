#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "image.h"
//#include "clickablelabel.h"
#include <QWidget>
#include <QMenu>
#include <QLabel>
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
    void labelClicked(QMouseEvent*);

private:
    void createActions();
    void createMenus();
    void createText();
    bool maybeExit();

    Image *image;
    QMenu *fileMenu;
    QAction *openAct;
    QAction *exitAct;

    QLabel *text;
};
#endif //MAINWINDOW_H
