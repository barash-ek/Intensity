#include "imagearea.h"
#include "contour.h"
#include <QFile>
#include <QTextStream>
#include <QtMath>

ImageArea::ImageArea(): accuracy(3)
{}
ImageArea::ImageArea(const Image &picture, const QPoint &point, int a): image(picture), accuracy(a)
{
    int imageHeight = image.getImage().height();
    int imageWidth = image.getImage().width();
    conditionPoint.resize(imageHeight);
    for(int i = 0; i < imageHeight; ++i)
        conditionPoint[i].resize(imageWidth);
    conditionPoint[point.y()][point.x()] = InnerArea;
    int mainIntensity = image.getIntensity(point);
    QQueue<QPoint> pointsQueue;
    QVector<QPoint> pointsVector;
    pointsQueue.enqueue(QPoint(point.x(), point.y()));
    int x, y, inner = 0, innerVoid = 0, outer = 0;
    for(int i = 0; i < pointsQueue.size(); ++i)
    {
        x = pointsQueue[i].x();
        y = pointsQueue[i].y();
        addPointsFront(pointsVector, x, y, imageWidth, imageHeight);
        // Выделение области
        for(int it = 0; it < pointsVector.size();++it)
        {
            if(conditionPoint[pointsVector[it].y()][pointsVector[it].x()] == NoState)
            {
                if(abs(image.getIntensity(pointsVector[it])-mainIntensity) <= accuracy)
                {
                    conditionPoint[pointsVector[it].y()][pointsVector[it].x()] = InnerArea;
                    pointsQueue.enqueue(pointsVector[it]);
                }
                else
                    conditionPoint[pointsVector[it].y()][pointsVector[it].x()] = OuterArea;
            }
        }
        pointsVector.clear();
    }
    // Выделение пустых точек внутри области
    int y_start = 0;
    for(int i = 0; i < imageHeight; ++i)
    {
        for(int j = 0; j < imageWidth; ++j)
        {
            if(conditionPoint[i][j] == InnerArea)
            {
                y_start = i;
                break;
            }
        }
        if(y_start >= 0)
            break;
    }
    int k = 0;
    for(int i = y_start; i< imageHeight; ++i)
    {
        for(int j = 0; j < imageWidth; ++j)
        {
            k = 0;
            if(conditionPoint[i][j] == OuterArea)
            {
                addPointsFront(pointsVector, j, i, imageWidth, imageHeight);
                addPointsDiagonal(pointsVector, j, i, imageWidth, imageHeight);
                for(int it = 0; it < pointsVector.size(); ++it)
                {
                    if(conditionPoint[pointsVector[it].y()][pointsVector[it].x()] == NoState)
                        k++;
                }
                if(k == 0)
                    conditionPoint[i][j] = InnerVoid;
                pointsVector.clear();
            }
        }
    }
        // Выделение граничных точек
        for(int i = 0; i < imageHeight; ++i)
            {
                for(int j = 0; j < imageWidth; ++j)
                {
                    if(conditionPoint[i][j] == InnerArea || conditionPoint[i][j] == InnerVoid)
                    {
                        addPointsFront(pointsVector, j, i, imageWidth, imageHeight);
                        for(int it = 0; it < pointsVector.size(); ++it)
                        {
                            if(conditionPoint[pointsVector[it].y()][pointsVector[it].x()] == OuterArea)
                                outer++;
                        }
                        if(outer)
                            conditionPoint[i][j] = ContourPoint;
                        outer = 0;
                        pointsVector.clear();
                    }
                }
            }
        // Удаление точек, ведущих к самопересечению контура
        for(int i = 0; i < imageHeight; ++i)
            {
                for(int j = 0; j < imageWidth; ++j)
                {
                    if(conditionPoint[i][j] == ContourPoint)
                    {
                        addPointsFront(pointsVector, j, i, imageWidth, imageHeight);
                        addPointsDiagonal(pointsVector, j, i, imageWidth, imageHeight);
                        for(int it = 0; it < pointsVector.size(); ++it)
                        {
                            if(conditionPoint[pointsVector[it].y()][pointsVector[it].x()] == InnerArea)
                                inner++;
                            else if(conditionPoint[pointsVector[it].y()][pointsVector[it].x()] == InnerVoid)
                                innerVoid++;
                        }
                        if(!inner && !innerVoid)
                            conditionPoint[i][j] = OuterArea;
                        inner = 0;
                        innerVoid = 0;
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
            if(conditionPoint[i][j] == InnerArea)
                foundArea.setPixelColor(j, i, color);
            /*else if(conditionPoint[i][j] == ArrangeContour)
                foundArea.setPixelColor(j, i, Qt::green);
            else if(conditionPoint[i][j] == ContourPoint)
                foundArea.setPixelColor(j, i, Qt::green);*/
            else if(conditionPoint[i][j] == InnerVoid)
                foundArea.setPixelColor(j, i, Qt::yellow);
            else
                foundArea.setPixelColor(j, i,transparentColor);
        }
    }
    return foundArea;
}
void ImageArea::addPointsFront(QVector<QPoint> &a, int x, int y, int width, int height)
{
    a.clear();
    if(y == 0 && (x >= 1 && x <= (width - 2)))
    {
        a << QPoint(x + 1, y) << QPoint(x, y + 1) << QPoint(x - 1, y);
    }
    else if((y == (height - 1)) && (x >= 1 && x <= (width - 2)))
    {
        a << QPoint(x, y - 1) << QPoint(x + 1, y)  << QPoint(x - 1, y);
    }
    else if(x == 0 && (y >= 1 && y <= (height - 2)))
    {
        a << QPoint(x, y - 1) << QPoint(x + 1, y) << QPoint(x, y + 1);
    }
    else if((x == (width - 1)) && (y >= 1 && y <= (height - 2)))
    {
        a << QPoint(x, y - 1) << QPoint(x, y + 1) << QPoint(x - 1, y);
    }
    else if((y == (height - 1)) && (x == 0))
    {
        a << QPoint(x, y - 1) << QPoint(x + 1, y) ;
    }
    else if((y == 0) && (x == (width - 1)))
    {
        a << QPoint(x - 1, y) << QPoint(x, y + 1);
    }
    else if((y == (height - 1)) && (x == (width - 1)))
    {
        a << QPoint(x, y - 1) << QPoint(x - 1, y);
    }
    else if(y == 0 && x== 0)
    {
        a << QPoint(x + 1, y) << QPoint(x, y + 1);
    }
    else
        a << QPoint(x, y - 1) << QPoint(x + 1, y) << QPoint(x, y + 1) << QPoint(x - 1, y);
}
void ImageArea::addPointsDiagonal(QVector<QPoint> &a, int x, int y, int width, int height)
{
    if(y == 0 && (x >= 1 && x <= (width - 2)))
    {
        a << QPoint(x + 1, y + 1) << QPoint(x - 1, y + 1);
    }
    else if((y == (height - 1)) && (x >= 1 && x <= (width - 2)))
    {
        a << QPoint(x - 1, y - 1) << QPoint(x + 1, y - 1);
    }
    else if(x == 0 && (y >= 1 && y <= (height - 2)))
    {
        a << QPoint(x + 1, y - 1) << QPoint(x + 1, y + 1);
    }
    else if((x == (width - 1)) && (y >= 1 && y <= (height - 2)))
    {
        a << QPoint(x - 1, y - 1) << QPoint(x - 1, y + 1);
    }
    else if((y == (height - 1)) && (x == 0))
    {
        a << QPoint(x + 1, y - 1);
    }
    else if((y == 0) && (x == (width - 1)))
    {
        a << QPoint(x - 1, y + 1);
    }
    else if((y == (height - 1)) && (x == (width - 1)))
    {
        a << QPoint(x - 1, y - 1);
    }
    else if(y == 0 && x == 0)
    {
        a << QPoint(x + 1, y + 1);
    }
    else
        a << QPoint(x - 1, y - 1) << QPoint(x + 1, y - 1) << QPoint(x + 1, y + 1) << QPoint(x - 1, y + 1);
}
Image& ImageArea::getImageObject()
{
    return image;
}
QVector<QVector <int> >* ImageArea::getconditionPoint()
{
    return &conditionPoint;
}
