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

    QSize newSize = loadedImage.size().expandedTo(size());
    //qDebug()<<"new size"<<newSize;
    QImage convertedImage;
    convertedImage=loadedImage.QImage::convertToFormat(QImage::Format_Grayscale8);
    convertedImage=convertedImage.scaled(newSize.width(), newSize.height());
    resizeImage(&convertedImage, newSize);
    picture = convertedImage;
    //qDebug()<<picture.width()<< " "<<picture.height();
    update();
    return true;
}
void Image::mouseMoveEvent(QMouseEvent *event) //указатель на объект события, который содержит информацию о нём
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
        text->setStyleSheet("background-color: #FFFFFF");
        //text->setStyleSheet("color: red");
        int intens=get();
        text->setText(QString::number(intens));
}
void Image::resizeEvent(QResizeEvent *event)
{
    //qDebug()<<picture.width();
    //qDebug() <<event->size().width();
        /*int newWidth = event->size().width();
        int newHeight = event->size().height();
        resizeImage(&picture, QSize(newWidth, newHeight));
        update();*/
    if (width() > picture.width() || height() > picture.height()) {
            int newWidth = qMax(width() + 128, picture.width()); //to avoid always resizing the image when the user resizes the main window
            int newHeight = qMax(height() + 128, picture.height());
            resizeImage(&picture, QSize(newWidth, newHeight));
            update();
        }
    QWidget::resizeEvent(event);
}
void Image::resizeImage(QImage *image, const QSize &newSize)
{
    if (image->size() == newSize)
        return;

    QImage newImage(newSize, QImage::Format_Grayscale8);
    newImage.fill(qRgb(255, 255, 255));
    newImage=newImage.scaled(newSize.width(), newSize.height());
    QPainter painter(&newImage);
    painter.drawImage(QPoint(0, 0), *image);
    *image = newImage;
}
void Image::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QRect dirtyRect = event->rect();
    painter.drawImage(dirtyRect, picture, dirtyRect);
}
void Image::createText()
{
    text = new QLabel(this);
    text->setFixedSize(20, 15);
    text->setAlignment(Qt::AlignCenter);
}
