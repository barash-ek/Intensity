#include "image.h"
#include "rightbar.h"
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
#include <QStyleOption>

Image::Image(QWidget *parent)
    : QWidget(parent)
{
    setMouseTracking(true);
    setAttribute(Qt::WA_StaticContents); //indicate that the widget contents are rooted to the top-left corner and don't change when the widget is resized.
    //setStyleSheet("border: 2px solid black");               //border
    createText();
    setGeometry(0, 0, 500, 500);
}
bool Image::openImage(const QString &fileName)
{
    //qDebug() << width() << height();
    QImage loadedImage;
    if (!loadedImage.load(fileName))
        return false;
    QSize size=loadedImage.size();
    sizePicture=size;
    emit signal_im(size);
    QImage convertedImage;
    convertedImage=loadedImage.QImage::convertToFormat(QImage::Format_Grayscale8);
    convertedPicture = convertedImage;
    resizeImage(&convertedImage, size);
    picture = convertedImage;
    open = true;
    update();
    return true;
}
void Image::mouseMoveEvent(QMouseEvent *event)
{
    if(open) //to hide Qlabel for initializing image
    {
        if(ourRect->contains(event->x(), event->y()))
        {
        int a=valueIntensity(event->pos());
        if(a>=0)
        {
            //int intensity=valueIntensity(event->pos());
            text->move(event->x()-12, event->y()+18);
            text->setText(QString::number(a));
            if(!text->isVisible())
                text->show();
        }
        }
    }
}
int Image::valueIntensity(const QPoint &pointIntensity)
{
    int x=pointIntensity.x();
    int y=pointIntensity.y();
    unsigned char *a;
    a=picture.scanLine(y);
    int intens=-1;
    if(a)
        intens=int(a[x]);
    else
        qDebug() <<"a is a null pointer";
    return intens;
}
void Image::mousePressEvent(QMouseEvent *event)
{
    if(open)
    {
        if(ourRect->contains(event->x(), event->y()))
        {
            if (event->button() == Qt::LeftButton) {
             if(text->isVisible())
                 text->hide();
            drawArea(event->pos());}
        }
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
    conditionPoint[pressPoint.y()-yPicture][pressPoint.x()-xPicture]=1;
    int mainIntensity=valueIntensity(pressPoint);
    int diff=3;
    QQueue<QPoint> pointsQueue;
    QVector<QPoint> pointsVector;
    pointsQueue.enqueue(QPoint(pressPoint.x(), pressPoint.y()));
    int x, y;
    int startY=yPicture, startX=xPicture, endY=yPicture+sizePicture.height(), endX=xPicture+sizePicture.width();
    for(int i=0;i<pointsQueue.count();++i)
    {
        x=pointsQueue[i].x();
        y=pointsQueue[i].y();
        if(y==startY&&(x>=startX+1&&x<=endX-2))
        {
            pointsVector << QPoint(x+1, y) << QPoint(x, y+1) << QPoint(x-1, y);
        }
        else if((y==endY-1)&&(x>=startX+1&&x<=endX-2))
        {
            pointsVector << QPoint(x, y-1) << QPoint(x+1, y)  << QPoint(x-1, y);
        }
        else if(x==startX&&(y>=startY+1&&y<=endY-2))
        {
            pointsVector << QPoint(x, y-1) << QPoint(x+1, y) << QPoint(x, y+1);
        }
        else if((x==endX-1)&&(y>=startY+1&&y<=endY-2))
        {
            pointsVector << QPoint(x, y-1) << QPoint(x, y+1) << QPoint(x-1, y);
        }
        else if((y==endY-1)&&(x==startX))
        {
            pointsVector << QPoint(x, y-1) << QPoint(x+1,y) ;
        }
        else if((y==startY)&&(x==endX-1))
        {
            pointsVector << QPoint(x-1,y) << QPoint(x, y+1);
        }
        else if((y==endY-1)&&(x==endX-1))
        {
            pointsVector << QPoint(x,y-1) << QPoint(x-1, y);
        }
        else if(y==startY&&x==startX)
        {
            pointsVector << QPoint(x+1, y) << QPoint(x, y+1);
        }
        else
            pointsVector << QPoint(x,y-1) << QPoint(x+1, y) << QPoint(x, y+1) << QPoint(x-1, y);
        for(int it=0; it < pointsVector.count();++it)
        {
            if(conditionPoint[pointsVector[it].y()-startY][pointsVector[it].x()-startX]==0)
            {
                if(abs(valueIntensity(pointsVector[it])-mainIntensity)<=diff)
                {
                    conditionPoint[pointsVector[it].y()-startY][pointsVector[it].x()-startX]=1;
                    pointsQueue.enqueue(pointsVector[it]);
                }
                else
                    conditionPoint[pointsVector[it].y()-startY][pointsVector[it].x()-startX]=-1;
            }
        }
        pointsVector.clear();                         //Вывод: ошибка выдается тогда, когда доходим до угловых точек!
    }
    /*for(int i=0; i < sizePicture.height(); ++i)
    {
        qDebug() << conditionPoint[i];
    }*/
    QImage foundArea(picture.width(), picture.height(), QImage::Format_ARGB32);
    //int transp = rightBar::getterTransparency();
    QRgb transparent=qRgba(0, 0, 0, 0);
    QRgb redColor=qRgba(255, 0, 0, 255);
    for(int i=0;i<foundArea.height();++i)
    {
        for(int j=0;j<foundArea.width();++j)
        {
            foundArea.setPixel(j,i,transparent);
        }
    }
    for(int i=0;i<sizePicture.height();++i)
    {
        for(int j=0;j<sizePicture.width();++j)
        {
            if(conditionPoint[i][j]==1)
                foundArea.setPixel(j+startX,i+startY, redColor);
        }
    }
    pictureArea=foundArea;
    update(); //to evoke PainEvent
}
void Image::resizeEvent(QResizeEvent *event)
{
    if(width() < sizePicture.width() || height() < sizePicture.height())
    {
        resizeImage(&convertedPicture, sizePicture);
    }
    else
     {
        resizeImage(&picture, event->size());
     }
    QWidget::resizeEvent(event);
    //qDebug() << xPicture << yPicture;
}
void Image::resizeImage(QImage *image, const QSize &newSize)
{
    QImage newImage(newSize, QImage::Format_Grayscale8);
    newImage.fill(qRgb(255, 255, 255));
    QPainter painter(&newImage);
    //qDebug() << newSize.width() << newSize.height();
    int x, y;
    x=(newSize.width()-image->width())/2;
    y=(newSize.height()-image->height())/2;

    xPicture = abs(newSize.width()-sizePicture.width())/2;
    yPicture = abs(newSize.height()-sizePicture.height())/2;
    painter.drawImage(QPoint(x, y), *image);
    ourRect = new QRect(xPicture, yPicture, sizePicture.width(), sizePicture.height()); // неправильно рисует при увеличенииБ правая сторона блокируется
    *image = newImage;
}
void Image::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawImage(QPoint(0,0), picture);
    painter.drawImage(QPoint(0,0), pictureArea);
    /*QStyleOption option;
    option.initFrom(this);
    style()->drawPrimitive(QStyle::PE_Widget, &option, &painter, this);*/
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
    //pictureArea.fill(qRgb(255, 255, 255));
    open=false;
    if(text->isVisible())
        text->hide();
    update();
}
void Image::getterTransparency(rightBar &bar)
{
    transparency = bar.valueTransparency;
}
void Image::getterAccuracy(rightBar &bar)
{
    accuracy = bar.accuracy;
}
void Image::getterColor(rightBar &bar)
{
    color = bar.colorChosen;
}
