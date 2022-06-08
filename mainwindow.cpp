#include "mainwindow.h"
#include "imagewidget.h"
#include "rightbar.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      widget(new ImageWidget(this)),
      bar(new RightBar(this)),
      fileMenu(0x0),
      openAct(0x0),
      exitAct(0x0)
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

    resize(800, 600);
    setWindowTitle(tr("Intensity"));

    bar->setInitialValueSlider(widget->getTransparency());
    bar->setInitialValueAccuracy(widget->getAccuracy());
    bar->setInitialValueFallibility((widget->getFallibility()));
    bar->setInitialStateCheckBoxDisplayArea(widget->getStateArea());
    bar->setInitialStateCheckBoxInnerContours(widget->getStateInnerContours());

    connect(bar, &RightBar::signalSliderTransparency, widget, &ImageWidget::userTransparency);
    connect(bar, &RightBar::signalFallibility, widget, &ImageWidget::userFallibility);
    connect(bar, &RightBar::signalColor, widget, &ImageWidget::userColor);
    connect(bar, &RightBar::accuracyChanged, widget, &ImageWidget::userAccuracy);
    connect(bar, &RightBar::checkBoxDisplayAreaStateChanged, widget, &ImageWidget::displayArea);
    connect(bar, &RightBar::checkBoxInnerContoursStateChanged, widget, &ImageWidget::displayInnerContours);

    connect(widget, &ImageWidget::mouseMoved, bar, &RightBar::setValueIntensity);
    connect(widget, &ImageWidget::countNodesChanged, bar, &RightBar::setCountNodes);
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
    if(!fileName.isEmpty())
    {
        if(!widget->openImage(fileName))
        {
            QMessageBox::StandardButton warningImage;
            warningImage = QMessageBox::warning(this, tr("Warning"),
                               tr("Image hasn't been opened"));
        }
    }
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
