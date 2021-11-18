#include "imagearea.h"
#include <QQueue>
#include <QDebug>

ImageArea::ImageArea(): accuracy(3)
{}
ImageArea::ImageArea(const Image &picture, const QPoint &point, int a): image(picture), accuracy(a)
{
    int imageHeight = image.getImage().height();
    int imageWidth = image.getImage().width();
    for(int i = 0; i < imageHeight; ++i)
    {
         QVector<int> strVector;
         for(int j = 0; j < imageWidth; ++j)
             strVector.push_back(0);
         conditionPoint.push_back(strVector);
    }
    conditionPoint[point.y()][point.x()]=1;
    int mainIntensity = image.getIntensity(point);
    QQueue<QPoint> pointsQueue;
    QVector<QPoint> pointsVector;
    pointsQueue.enqueue(QPoint(point.x(), point.y()));
    int x, y, one = 0;
    for(int i = 0; i < pointsQueue.count(); ++i)
    {
        x=pointsQueue[i].x();
        y=pointsQueue[i].y();
        addPoints(pointsVector, x, y);
        for(int it = 0; it < pointsVector.count();++it)
        {
            assert((0 <= pointsVector[it].x()  && pointsVector[it].x() <= imageWidth - 1) && (0 <= pointsVector[it].y() && pointsVector[it].y() <= imageHeight - 1));
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
    for(int i = 0; i < imageHeight; ++i)
        {
            for(int j = 0; j < imageWidth; ++j)
            {
                if(conditionPoint[i][j] == 2)
                {
                    addPoints(pointsVector, j, i);
                    if(i == 0 && (j >= 1 && j <= imageWidth - 2))
                    {
                        pointsVector << QPoint(j + 1, i + 1) << QPoint(j - 1, i + 1);
                    }
                    else if((i == imageHeight - 1) && (j >= 1 && j <= imageWidth - 2))
                    {
                        pointsVector << QPoint(j - 1, i - 1) << QPoint(j + 1, i - 1);
                    }
                    else if(j == 0 && (i >= 1 && i <= imageHeight - 2))
                    {
                        pointsVector << QPoint(j + 1, i - 1) << QPoint(j + 1, i + 1);
                    }
                    else if((j == imageWidth - 1) && (i >= 1 && i <= imageHeight - 2))
                    {
                        pointsVector << QPoint(j - 1, i - 1) << QPoint(j - 1, i + 1);
                    }
                    else if((i == imageHeight - 1) && (j == 0))
                    {
                        pointsVector << QPoint(j + 1, i - 1);
                    }
                    else if((i == 0) && (j == imageWidth - 1))
                    {
                        pointsVector << QPoint(j - 1, i + 1);
                    }
                    else if((i == imageHeight - 1) && (j == imageWidth - 1))
                    {
                        pointsVector << QPoint(j - 1, i - 1);
                    }
                    else if(i == 0 && j== 0)
                    {
                        pointsVector << QPoint(j + 1, i + 1);
                    }
                    else
                        pointsVector << QPoint(j - 1, i - 1) << QPoint(j + 1, i - 1) << QPoint(j + 1, i + 1) << QPoint(j - 1, i + 1);
                    for(int it = 0; it < pointsVector.count(); ++it)
                    {
                        assert((0 <= pointsVector[it].x()  && pointsVector[it].x() <= imageWidth - 1) && (0 <= pointsVector[it].y() && pointsVector[it].y() <= imageHeight - 1));
                        if(conditionPoint[pointsVector[it].y()][pointsVector[it].x()] == 1)
                            one++;
                    }
                    if(!one)
                        conditionPoint[i][j] = -1;
                    one = 0;
                    pointsVector.clear();
                }
            }
        }
}
QImage ImageArea::drawArea(const QColor &color)
{
    int imageHeight = image.getImage().height();
    int imageWidth = image.getImage().width();
    QImage foundArea(imageWidth, imageHeight, QImage::Format_ARGB32);
    QColor transparentColor(0, 0, 0, 0);
    for(int i = 0; i < imageHeight; ++i)
    {
        for(int j = 0; j < imageWidth; ++j)
        {
            if(conditionPoint[i][j] == 1)
                foundArea.setPixelColor(j, i, color);
            else if(conditionPoint[i][j] == 2)
                foundArea.setPixelColor(j, i, Qt::green);
            else
                foundArea.setPixelColor(j, i,transparentColor);
        }
    }
    return foundArea;
}
void ImageArea::addPoints(QVector<QPoint> &a, int x, int y)
{
    static int endY = image.getImage().height(), endX = image.getImage().width();
    if(y == 0 && (x >= 1 && x <= endX-2))
    {
        a << QPoint(x+1, y) << QPoint(x, y+1) << QPoint(x-1, y);
    }
    else if((y == endY-1) && (x >= 1 && x <= endX-2))
    {
        a << QPoint(x, y-1) << QPoint(x+1, y)  << QPoint(x-1, y);
    }
    else if(x == 0 && (y >= 1 && y <= endY-2))
    {
        a << QPoint(x, y-1) << QPoint(x+1, y) << QPoint(x, y+1);
    }
    else if((x == endX-1) && (y >= 1 && y <= endY-2))
    {
        a << QPoint(x, y-1) << QPoint(x, y+1) << QPoint(x-1, y);
    }
    else if((y == endY-1) && (x == 0))
    {
        a << QPoint(x, y-1) << QPoint(x+1, y) ;
    }
    else if((y == 0) && (x == endX-1))
    {
        a << QPoint(x-1, y) << QPoint(x, y+1);
    }
    else if((y == endY-1) && (x == endX-1))
    {
        a << QPoint(x, y-1) << QPoint(x-1, y);
    }
    else if(y == 0 && x== 0)
    {
        a << QPoint(x+1, y) << QPoint(x, y+1);
    }
    else
        a << QPoint(x, y-1) << QPoint(x+1, y) << QPoint(x, y+1) << QPoint(x-1, y);
}
