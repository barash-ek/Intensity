#include "imagearea.h"
#include <QQueue>
#include <QDebug>

ImageArea::ImageArea(): accuracy(3)
{}
ImageArea::ImageArea(const Image &picture, const QPoint &point, int a): image(picture), accuracy(a)
{
    int imageHeight = image.getImage().height();
    int imageWidth = image.getImage().width();
    for(int i=0; i < imageHeight; ++i)
    {
         QVector<int> strVector;
         for(int j=0; j < imageWidth; ++j)
             strVector.push_back(0);
         conditionPoint.push_back(strVector);
    }
    conditionPoint[point.y()][point.x()]=1;
    int mainIntensity = image.getIntensity(point);
    QQueue<QPoint> pointsQueue;
    QVector<QPoint> pointsVector;
    pointsQueue.enqueue(QPoint(point.x(), point.y()));
    int x, y;
    int startY=0, startX=0, endY=imageHeight, endX=imageWidth;
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
            if(conditionPoint[pointsVector[it].y()][pointsVector[it].x()] == 0)
            {
                if(abs(image.getIntensity(pointsVector[it])-mainIntensity) <= accuracy)
                {
                    conditionPoint[pointsVector[it].y()][pointsVector[it].x()] = 1;
                    pointsQueue.enqueue(pointsVector[it]);
                }
                else
                {
                    conditionPoint[pointsVector[it].y()][pointsVector[it].x()] = -1;
                    if(conditionPoint[y][x] != 2)
                        conditionPoint[y][x] = 2;
                }
            }
            else if(conditionPoint[pointsVector[it].y()][pointsVector[it].x()] == -1 && conditionPoint[y][x] != 2)
            {
                conditionPoint[y][x] = 2;
            }
        }
        pointsVector.clear();
    }
}
QImage ImageArea::drawArea(const QColor &color)
{
    int imageHeight = image.getImage().height();
    int imageWidth = image.getImage().width();
    QImage foundArea(imageWidth, imageHeight, QImage::Format_ARGB32);
    QColor transparentColor(0, 0, 0, 0);
    for(int i=0;i<imageHeight;++i)
    {
        for(int j=0;j<imageWidth;++j)
        {
            if(conditionPoint[i][j]==1)
                foundArea.setPixelColor(j,i, color);
            else if(conditionPoint[i][j]==2)
                foundArea.setPixelColor(j,i, Qt::green);
            else
                foundArea.setPixelColor(j,i,transparentColor);
        }
    }
    return foundArea;
}
