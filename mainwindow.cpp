#include "mainwindow.h"
#include "image.h"
#include <QWidget>
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
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), image(new Image(this))
{
    setCentralWidget(image);

    createActions();
    createMenus();

    setWindowTitle(tr("Image"));
    resize(500, 500);
    connect(image, SIGNAL (signal_im(QSize)), this, SLOT (slot_im(QSize)) );
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
    exitAct = new QAction(tr("Close"), this);
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
    if(size()!=s)
    this->resize(s);
}
