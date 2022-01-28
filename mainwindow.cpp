#include "mainwindow.h"
#include "imagewidget.h"
#include "rightbar.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), widget(new ImageWidget(this)), bar(new RightBar(this)), fileMenu(0x0), openAct(0x0), exitAct(0x0)
{
    QPalette pall;
    pall.setColor(this->backgroundRole(), Qt::white);
    setPalette(pall);

    createActions();
    createMenus();

    QHBoxLayout *imageBar = new QHBoxLayout;
    imageBar->addWidget(widget, Qt::AlignCenter);
    imageBar->setSpacing(2);
    imageBar->addWidget(bar);
    QWidget *CentralWidget = new QWidget;
        CentralWidget->setLayout(imageBar);
    setCentralWidget(CentralWidget);

    connect(widget, SIGNAL(signalWidget(QSize)), this, SLOT(slotWidget(QSize)));
    resize(800, 800);
    setWindowTitle(tr("Image"));

    bar->setInitialValueSlider(widget->getTransparency());
    bar->setInitialValueAccuracy(widget->getAccuracy());
    bar->setInitialValueFallibility((widget->getFallibility()));

    connect(bar, SIGNAL(signalSliderTransparency(int)), widget, SLOT(userTransparency(int)));
    connect(bar, SIGNAL(signalFallibility(int)), widget, SLOT(userFallibility(int)));
    connect(bar, SIGNAL(signalColor()), widget, SLOT(userColor()));
    connect(bar, SIGNAL(accuracyChanged(int)), widget, SLOT(userAccuracy(int)));
    connect(widget, SIGNAL(mouseMoved(int)), bar, SLOT(setValueIntensity(int)));

    connect(widget, SIGNAL(signalSetEnabled(bool)), bar, SLOT(setEnabledSpinBox(bool)));
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
        widget->openImage(fileName);
}
void MainWindow::createActions()
{
    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    connect(openAct, &QAction::triggered, this, &MainWindow::open);

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
    return false;
}
void MainWindow::slotWidget(QSize s)
{
    int diffHeight = height() - widget->height();
    int diffWidth = width() - widget->width() - bar->width();
    widget->resize(s);
    bar->resize(bar->width(),s.height());
    this->resize(s.width() + bar->width() + diffWidth, s.height() + diffHeight);
}
