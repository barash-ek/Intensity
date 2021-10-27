#include "mainwindow.h"
#include "image.h"
#include "rightbar.h"
#include <QWidget>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QCloseEvent>
#include <QFileDialog>
#include <QColorDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QImageWriter>
#include <QApplication>
#include <QMenuBar>
#include <QDebug>
#include <QFrame>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), image(new Image(this)), bar(new rightBar(this))
{
    QPalette pall;
    pall.setColor(this->backgroundRole(), Qt::white);
    setPalette(pall);
    createActions();
    createMenus();

    QHBoxLayout *imageBar = new QHBoxLayout;
    imageBar->addWidget(image, Qt::AlignCenter);
    imageBar->setSpacing(2);
    //imageBar->addStretch(1);
    imageBar->addWidget(bar);
    QWidget *widget = new QWidget;
        widget->setLayout(imageBar);
    setCentralWidget(widget);
    //setLayout(imageBar);

    connect(image, SIGNAL (signal_im(QSize)), this, SLOT (slot_im(QSize)) );
    resize(600, 600);
    setWindowTitle(tr("Image"));
    //setMouseTracking(true);
        //centralWidget()->setMouseTracking(true);
}
void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeExit())
        event->accept();
    else
        event->ignore();
}
void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                               tr("Open File"), "D:/Inobitec/Dicom/Pictures");
    if (!fileName.isEmpty())
        image->openImage(fileName);
}
void MainWindow::createActions()
{
    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    connect(openAct, &QAction::triggered, this, &MainWindow::open); //triggered - is a signal

    exitAct = new QAction(tr("Exit"), this);
    exitAct->setShortcuts(QKeySequence::Quit); //changes!!
    connect(exitAct, &QAction::triggered, this, &MainWindow::close);
}
void MainWindow::createMenus()
{

    fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(openAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    menuBar()->addMenu(fileMenu);
}
bool MainWindow::maybeExit()
{
       QMessageBox::StandardButton ret;
       ret = QMessageBox::warning(this, tr("Warning"),
                          tr("Do you want to quit?"),
                          QMessageBox::Yes | QMessageBox::No);
        if (ret == QMessageBox::Yes)
            return true;
        else if (ret == QMessageBox::No)
            return false;
    return false; //in order to stay
}
void MainWindow::slot_im(QSize s)
{
    int diffHeight = height() - image->height();
    int diffWidth = width() - image->width() - bar->width();
    image->resize(s);
    bar->resize(bar->width(),s.height());
    this->resize(s.width()+bar->width()+diffWidth, s.height()+diffHeight);
}
