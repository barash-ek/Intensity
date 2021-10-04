#include "image.h"
#include <QImage>
#include <QMouseEvent>
#include <QPainter>
#include <QDebug>
Image::Image(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_StaticContents); //indicate that the widget contents are rooted to the top-left corner and don't change when the widget is resized.
}
bool Image::openImage(const QString &fileName)
{
    QImage loadedImage;
    if (!loadedImage.load(fileName))
        return false;

    QSize newSize = loadedImage.size().expandedTo(size());
    QImage convertedImage;
    convertedImage=loadedImage.QImage::convertToFormat(QImage::Format_Grayscale8);
    resizeImage(&convertedImage, newSize);
    //saveImage(fileName, "jpg");
    picture = convertedImage;
    update();
    return true;
}
void Image::mousePressEvent(QMouseEvent *event) //указатель на объект события, который содержит информацию о нём
{
    if (event->button() == Qt::LeftButton) {
        x=event->x();
        y=event->y();
        unsigned char *a;
        a=picture.scanLine(y);
        if(a)
            intensity=int(a[x]);
        else
            qDebug() <<"a is a null pointer";
        emit clicked(event);
    }
}
void Image::resizeEvent(QResizeEvent *event)
{
    if (width() > picture.width() || height() > picture.height()) {
        int newWidth = qMax(width() + 128, picture.width());
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
