#include "image.h"
#include <QImage>
#include <QMouseEvent>
#include <QPainter>
#include <QDebug>
#include <QHBoxLayout>
Image::Image(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_StaticContents); //indicate that the widget contents are rooted to the top-left corner and don't change when the widget is resized.
    this->setMouseTracking(true);
    createText();
}
bool Image::openImage(const QString &fileName)
{
    QImage loadedImage;
    if (!loadedImage.load(fileName))
        return false;
    QSize size=loadedImage.size();
    emit signal_im(size);
    QSize newSize = loadedImage.size().expandedTo(this->size());
    QImage convertedImage;
    convertedImage=loadedImage.QImage::convertToFormat(QImage::Format_Grayscale8);
    resizeImage(&convertedImage, newSize);
    picture = convertedImage;
    open = true;
    update();
    return true;
}
void Image::mouseMoveEvent(QMouseEvent *event)
{
    if(open)
    {
        int x=event->x();
        int y=event->y();
        unsigned char *a;
        a=picture.scanLine(y);
        if(a)
            intensity=int(a[x]);
        else
            qDebug() <<"a is a null pointer";
        text->move(x-12, y+18);
        int intens=get();
        text->setText(QString::number(intens));
        if(!text->isVisible())
            text->show();
    }
}
void Image::resizeEvent(QResizeEvent *event)
{
     resizeImage(&picture, event->size());
     QWidget::resizeEvent(event);
}
void Image::resizeImage(QImage *image, const QSize &newSize)
{
    QImage newImage(newSize, QImage::Format_Grayscale8);
    newImage.fill(qRgb(255, 255, 255));
    QPainter painter(&newImage);
    int x=(width()-picture.width())/2;
    int y=(height()-picture.height())/2;
    painter.drawImage(QPoint(x, y), *image);
    *image = newImage;
}
void Image::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QRect dirtyRect = event->rect();
    painter.drawImage(dirtyRect, picture, dirtyRect);
}
void Image::leaveEvent(QEvent *event)
{
    text->hide();
}
void Image::createText()
{
    text = new QLabel(this);
    text->setFixedSize(20, 15);
    text->setAlignment(Qt::AlignCenter);
    text->setStyleSheet("background-color: white");
    //text->setStyleSheet("color: red");
    text->hide();
}
