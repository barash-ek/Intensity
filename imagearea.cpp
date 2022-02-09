#include "imagearea.h"
#include "contour.h"

ImageArea::ImageArea(): accuracy(3)
{}
ImageArea::ImageArea(const Image &picture, const QPoint &point, int a): image(picture), accuracy(a)
{
    const int imageHeight = image.getImage().height();
    const int imageWidth = image.getImage().width();

    conditionPoint.resize(imageHeight);
    for(int i = 0; i < imageHeight; ++i)
        conditionPoint[i].resize(imageWidth);

    selectionArea(point);
    selectionInnerVoidPoints();
    selectionBoundaryPoints();
    deleteUnnecessaryPoint();
}
void ImageArea::selectionArea(const QPoint& pointPress)
{
    const int imageHeight = image.getImage().height();
    const int imageWidth = image.getImage().width();
    QQueue<QPoint> pointsQueue;
    QVector<QPoint> pointsVector;

    conditionPoint[pointPress.y()][pointPress.x()] = InnerArea;
    const int mainIntensity = image.getIntensity(pointPress);
    pointsQueue.enqueue(pointPress);
    int x = -1, y = -1;

    while(!pointsQueue.isEmpty())
    {
        const QPoint &pointInQueue = pointsQueue.dequeue();
        x = pointInQueue.x();
        y = pointInQueue.y();
        addPointsFront(pointsVector, x, y, imageWidth, imageHeight);

        for(int i = 0, t = pointsVector.size(); i < t; ++i)
        {
            const QPoint &point = pointsVector[i];
            const int xPoint = point.x();
            const int yPoint = point.y();
            if(conditionPoint[yPoint][xPoint] == NoState)
            {
                if(abs(image.getIntensity(point) - mainIntensity) <= accuracy)
                {
                    conditionPoint[yPoint][xPoint] = InnerArea;
                    pointsQueue.enqueue(point);
                }
                else
                    conditionPoint[yPoint][xPoint] = OuterArea;
            }
        }
        pointsVector.clear();
    }
}
void ImageArea::selectionInnerVoidPoints()
{
    const int imageHeight = image.getImage().height();
    const int imageWidth = image.getImage().width();
    QVector<QPoint> pointsVector;
    int y_start = 0;

    for(int i = 0; i < imageHeight; ++i)
    {
        const QVector<int>& row = conditionPoint.at(i);
        const int *rowPointer = &row[0];
        for(int j = 0; j < imageWidth; ++j)
        {
            if(rowPointer[j] == InnerArea)
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
        const QVector<int>& row = conditionPoint.at(i);
        const int *rowPointer = &row[0];
        for(int j = 0; j < imageWidth; ++j)
        {
            k = 0;
            if(rowPointer[j] == OuterArea)
            {
                addPointsFront(pointsVector, j, i, imageWidth, imageHeight);
                addPointsDiagonal(pointsVector, j, i, imageWidth, imageHeight);
                for(int i = 0, t = pointsVector.size(); i < t; ++i)
                {
                    const QPoint &point = pointsVector[i];
                    if(conditionPoint[point.y()][point.x()] == NoState)
                        k++;
                }
                if(k == 0)
                    conditionPoint[i][j] = InnerVoid;
                pointsVector.clear();
            }
        }
    }
}
void ImageArea::selectionBoundaryPoints()
{
    const int imageHeight = image.getImage().height();
    const int imageWidth = image.getImage().width();
    QVector<QPoint> pointsVector;
    int outer = 0;

    for(int i = 0; i < imageHeight; ++i)
    {
        const QVector<int>& row = conditionPoint.at(i);
        const int *rowPointer = &row[0];
        for(int j = 0; j < imageWidth; ++j)
        {
            if(rowPointer[j] == InnerArea || rowPointer[j] == InnerVoid)
            {
                addPointsFront(pointsVector, j, i, imageWidth, imageHeight);
                for(int i = 0, t  = pointsVector.size(); i < t; ++i)
                {
                    const QPoint &point = pointsVector[i];
                    if(conditionPoint[point.y()][point.x()] == OuterArea)
                        outer++;
                }
                if(outer)
                    conditionPoint[i][j] = ContourPoint;
                outer = 0;
                pointsVector.clear();
            }
        }
     }
}
void ImageArea::deleteUnnecessaryPoint()   // Удаление точек, ведущих к самопересечению контура
{
    const int imageHeight = image.getImage().height();
    const int imageWidth = image.getImage().width();
    QVector<QPoint> pointsVector;
    int inner = 0, innerVoid = 0;

    for(int i = 0; i < imageHeight; ++i)
    {
        const QVector<int>& row = conditionPoint.at(i);
        const int *rowPointer = &row[0];
        for(int j = 0; j < imageWidth; ++j)
        {
            if(rowPointer[j] == ContourPoint)
            {
                addPointsFront(pointsVector, j, i, imageWidth, imageHeight);
                addPointsDiagonal(pointsVector, j, i, imageWidth, imageHeight);
                for(int i = 0, t = pointsVector.size(); i < t; ++i)
                {
                    const int x = pointsVector[i].x();
                    const int y  = pointsVector[i].y();
                    if(conditionPoint[y][x] == InnerArea)
                        inner++;
                    else if(conditionPoint[y][x] == InnerVoid)
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
    const int imageHeight = image.getImage().height();
    const int imageWidth = image.getImage().width();
    QImage foundArea(imageWidth, imageHeight, QImage::Format_ARGB32);
    QColor transparentColor(0, 0, 0, 0);
    for(int i = 0; i < imageHeight; ++i)
    {
        for(int j = 0; j < imageWidth; ++j)
        {
            /*if(conditionPoint[i][j] == InnerArea)
                foundArea.setPixelColor(j, i, color);
            else if(conditionPoint[i][j] == ArrangeContour)
                foundArea.setPixelColor(j, i, Qt::blue);
            else if(conditionPoint[i][j] == ContourPoint)
                foundArea.setPixelColor(j, i, Qt::green);
            else if(conditionPoint[i][j] == InnerVoid)
                foundArea.setPixelColor(j, i, Qt::yellow);
            else*/
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
