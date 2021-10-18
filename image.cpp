#include "image.h"
#include <QImage>
#include <QMouseEvent>
#include <QPainter>
#include <QRgb>
#include <QColor>
#include <QDebug>
#include <QVector>
#include <QQueue>
#include <QList>
#include <QPoint>
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
    sizePicture=size;
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
    if(open) //to hide Qlabel for initializing image
    {
        int a=valueIntensity(event->pos());
        if(a>=0&&a<=255)
        {
            int intensity=valueIntensity(event->pos());
            text->move(event->x()-12, event->y()+18);
            text->setText(QString::number(intensity));
            if(!text->isVisible())
                text->show();
        }
    }
}
int Image::valueIntensity(const QPoint &pointIntensity)
{
    int x=pointIntensity.x();
    int y=pointIntensity.y();
    unsigned char *a;
    a=picture.scanLine(y-1);
    int intens=-1;
    if(a)
        intens=int(a[x-1]);
    else
        qDebug() <<"a is a null pointer";
    return intens;
}
void Image::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
    if(text->isVisible())
        text->hide();
    //qDebug()<<valueIntensity(event->pos());
    //drawPoint(event->pos());
    drawArea(event->pos());
    }
}
/*void Image::drawPoint(const QPoint &pressPoint)                         !!!drawPoint!!!
{
    QPainter painter(&picture);
    painter.setPen(QPen(Qt::red, 3));
    painter.drawPoint(pressPoint);
    update();
}*/
void Image::drawArea(const QPoint &pressPoint)
{
    QVector<QVector <int> > conditionPoint;
    for(int i=0; i < sizePicture.height(); ++i)
    {
         QVector<int> strVector;
         for(int j=0; j < sizePicture.width(); ++j)
             strVector.push_back(0);
         conditionPoint.push_back(strVector);
    }
    conditionPoint[pressPoint.y()-1][pressPoint.x()-1]=1;
    int mainIntensity=valueIntensity(pressPoint);
    int diff=4;
    QQueue<QPoint> pointsQueue;
    QVector<QPoint> pointsVector;
    pointsQueue.enqueue(pressPoint);
    int x, y;
    for(int i=0;i<pointsQueue.count();++i)
    {
        x=pointsQueue[i].x();
        y=pointsQueue[i].y();
        pointsVector << QPoint(x,y-1) << QPoint(x+1, y) << QPoint(x, y+1) << QPoint(x-1, y);
        for(int it=0; it < pointsVector.count();++it)
        {
            if(conditionPoint[pointsVector[it].y()-1][pointsVector[it].x()-1]==0)
            {
                if(abs(valueIntensity(pointsVector[it])-mainIntensity)<=diff)
                {
                    conditionPoint[pointsVector[it].y()-1][pointsVector[it].x()-1]=1;
                    pointsQueue.enqueue(pointsVector[it]);
                }
                else
                    conditionPoint[pointsVector[it].y()-1][pointsVector[it].x()-1]=-1;
            }
        }
        //int a=pointsQueue.count();
        //for(int i=0;i<a; ++i)
          // qDebug() <<pointsQueue.dequeue();
        pointsVector.clear();
    }
    /*for(int i=0; i < sizePicture.height(); ++i)
    {
        for(int j=0; j < sizePicture.width(); ++j)
            if(conditionPoint[i][j])
            qDebug() << conditionPoint[i][j];
    }*/
    QImage foundArea(picture.width(), picture.height(), QImage::Format_ARGB32);
    QRgb transparent=qRgba(0,0,0,0);
    QRgb redColor=qRgba(255, 0, 0, 255);
    for(int i=0;i<foundArea.height();++i)
    {
        for(int j=0;j<foundArea.width();++j)
        {
            if(conditionPoint[i][j]==1)
                foundArea.setPixel(j,i, redColor);
            else
                foundArea.setPixel(j,i,transparent);
        }
    }
    pictureArea=foundArea;
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
    painter.drawImage(dirtyRect, pictureArea, dirtyRect);
    update();
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
void Image::clearImage()
{
    picture.fill(qRgb(255, 255, 255));
    open=false;
    if(text->isVisible())
        text->hide();
    update();
}
