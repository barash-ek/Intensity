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
    QImage loadedImage;
    if (!loadedImage.load(fileName))
        return false;
    QSize size=loadedImage.size();
    emit signal_im(size);
    loadedImage=loadedImage.QImage::convertToFormat(QImage::Format_Grayscale8);
    picture = loadedImage;
    return true;
}
void Image::mouseMoveEvent(QMouseEvent *event)
{
    int mouseX=event->x();
    int mouseY=event->y();
    int a=valueIntensity(event->pos());
    if(a>=0)
    {
        text->move(mouseX-12, mouseY+18);
        text->setText(QString::number(a));
        if(!text->isVisible())
            text->show();
    }
}
int Image::valueIntensity(const QPoint &pointIntensity)
{
    int x=pointIntensity.x();
    int y=pointIntensity.y();
    unsigned char *a;
    if(x>=picture.width()||y>=picture.height())
        return -1;
    a=picture.scanLine(y);
    int intens=-1;
    if(a)
        intens = int(a[x]);
    else
        qDebug() <<"a is a null pointer";
    return intens;
}
void Image::mousePressEvent(QMouseEvent *event)
{
    if(event->x()<picture.width()&&event->y()<picture.height())
    {
        if (event->button() == Qt::LeftButton)
        {
             if(text->isVisible())
              text->hide();
             drawArea(event->pos());
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
    int pictureHeight = picture.height();
    int pictureWidth = picture.width();
    QVector<QVector <int> > conditionPoint;
    for(int i=0; i < pictureHeight; ++i)
    {
         QVector<int> strVector;
         for(int j=0; j < pictureWidth; ++j)
             strVector.push_back(0);
         conditionPoint.push_back(strVector);
    }
    conditionPoint[pressPoint.y()][pressPoint.x()]=1;
    int mainIntensity=valueIntensity(pressPoint);
    int diff=3;
    QQueue<QPoint> pointsQueue;
    QVector<QPoint> pointsVector;
    pointsQueue.enqueue(QPoint(pressPoint.x(), pressPoint.y()));
    int x, y;
    int startY=0, startX=0, endY=pictureHeight, endX=pictureWidth;
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
            if(conditionPoint[pointsVector[it].y()][pointsVector[it].x()]==0)
            {
                if(abs(valueIntensity(pointsVector[it])-mainIntensity)<=diff)
                {
                    conditionPoint[pointsVector[it].y()][pointsVector[it].x()]=1;
                    pointsQueue.enqueue(pointsVector[it]);
                }
                else
                    conditionPoint[pointsVector[it].y()][pointsVector[it].x()]=-1;
            }
        }
        pointsVector.clear();                         //Вывод: ошибка выдается тогда, когда доходим до угловых точек!
    }
    QImage foundArea(pictureWidth, pictureHeight, QImage::Format_ARGB32);
    //int transp = rightBar::getterTransparency();
    QRgb transparent=qRgba(0, 0, 0, 0);
    QRgb redColor=qRgba(255, 0, 0, 255);
    for(int i=0;i<pictureHeight;++i)
    {
        for(int j=0;j<pictureWidth;++j)
        {
            if(conditionPoint[i][j]==1)
                foundArea.setPixel(j,i, redColor);
            else
                foundArea.setPixel(j,i,transparent);
        }
    }
    pictureArea=foundArea;
    update(); //to evoke PainEvent
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
