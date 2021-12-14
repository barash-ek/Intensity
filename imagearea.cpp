#include "imagearea.h"
#include <QFile>
#include <QTextStream>

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
    conditionPoint[point.y()][point.x()] = InnerArea;
    int mainIntensity = image.getIntensity(point);
    QQueue<QPoint> pointsQueue;
    QVector<QPoint> pointsVector;
    pointsQueue.enqueue(QPoint(point.x(), point.y()));
    int x, y, inner = 0, innerVoid = 0, outer = 0;
    for(int i = 0; i < pointsQueue.count(); ++i)
    {
        x = pointsQueue[i].x();
        y = pointsQueue[i].y();
        addPointsFront(pointsVector, x, y);
        // Выделение области
        for(int it = 0; it < pointsVector.count();++it)
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
                addPointsFront(pointsVector, j, i);
                addPointsDiagonal(pointsVector, j, i);
                for(int it = 0; it < pointsVector.count(); ++it)
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
                        addPointsFront(pointsVector, j, i);
                        for(int it = 0; it < pointsVector.count(); ++it)
                        {
                            if(conditionPoint[pointsVector[it].y()][pointsVector[it].x()] == OuterArea)
                                outer++;
                        }
                        if(outer)
                            conditionPoint[i][j] = Contour;
                        outer = 0;
                        pointsVector.clear();
                    }
                }
            }
        // Удадение точек, ведущих к самопересечению контура
        for(int i = 0; i < imageHeight; ++i)
            {
                for(int j = 0; j < imageWidth; ++j)
                {
                    if(conditionPoint[i][j] == Contour)
                    {
                        addPointsFront(pointsVector, j, i);
                        addPointsDiagonal(pointsVector, j, i);
                        for(int it = 0; it < pointsVector.count(); ++it)
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
    arrangePoints();
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
            else if(conditionPoint[i][j] == ArrangeContour)
                foundArea.setPixelColor(j, i, Qt::green);
            else if(conditionPoint[i][j] == Contour)
                foundArea.setPixelColor(j, i, Qt::blue);
            else if(conditionPoint[i][j] == InnerVoid)
                foundArea.setPixelColor(j, i, Qt::yellow);
            else
                foundArea.setPixelColor(j, i,transparentColor);
        }
    }
    return foundArea;
}
void ImageArea::addPointsFront(QVector<QPoint> &a, int x, int y)
{
    static int endY = image.getImage().height(), endX = image.getImage().width();
    if(y == 0 && (x >= 1 && x <= (endX - 2)))
    {
        a << QPoint(x + 1, y) << QPoint(x, y + 1) << QPoint(x - 1, y);
    }
    else if((y == (endY - 1)) && (x >= 1 && x <= (endX - 2)))
    {
        a << QPoint(x, y - 1) << QPoint(x + 1, y)  << QPoint(x - 1, y);
    }
    else if(x == 0 && (y >= 1 && y <= (endY - 2)))
    {
        a << QPoint(x, y - 1) << QPoint(x + 1, y) << QPoint(x, y + 1);
    }
    else if((x == (endX - 1)) && (y >= 1 && y <= (endY - 2)))
    {
        a << QPoint(x, y - 1) << QPoint(x, y + 1) << QPoint(x - 1, y);
    }
    else if((y == (endY - 1)) && (x == 0))
    {
        a << QPoint(x, y - 1) << QPoint(x + 1, y) ;
    }
    else if((y == 0) && (x == (endX - 1)))
    {
        a << QPoint(x - 1, y) << QPoint(x, y + 1);
    }
    else if((y == (endY - 1)) && (x == (endX - 1)))
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
void ImageArea::addPointsDiagonal(QVector<QPoint> &a, int x, int y)
{
    static int endY = image.getImage().height(), endX = image.getImage().width();
    if(y == 0 && (x >= 1 && x <= (endX - 2)))
    {
        a << QPoint(x + 1, y + 1) << QPoint(x - 1, y + 1);
    }
    else if((y == (endY - 1)) && (x >= 1 && x <= (endX - 2)))
    {
        a << QPoint(x - 1, y - 1) << QPoint(x + 1, y - 1);
    }
    else if(x == 0 && (y >= 1 && y <= (endY - 2)))
    {
        a << QPoint(x + 1, y - 1) << QPoint(x + 1, y + 1);
    }
    else if((x == (endX - 1)) && (y >= 1 && y <= (endY - 2)))
    {
        a << QPoint(x - 1, y - 1) << QPoint(x - 1, y + 1);
    }
    else if((y == (endY - 1)) && (x == 0))
    {
        a << QPoint(x + 1, y - 1);
    }
    else if((y == 0) && (x == (endX - 1)))
    {
        a << QPoint(x - 1, y + 1);
    }
    else if((y == (endY - 1)) && (x == (endX - 1)))
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
void ImageArea::arrangePoints()
{
    QVector<QPoint> pointsContour;
    QVector<QPoint> pointsAuxiliary, pointsNeigbour;
    int imageHeight = image.getImage().height();
    int imageWidth = image.getImage().width();
    int x = -1, y = -1;
    for(int i = 0; i < imageHeight; ++i)
    {
        for(int j = 0; j < imageWidth; ++j)
        {
            if(conditionPoint[i][j] == Contour)
            {
                x = j;
                y = i;
                break;
            }
        }
        if(y >= 0)
            break;
    }
    int quantityNeigbours = 0;
    QVector<int> pointsState(4);
    pointsContour << QPoint(x, y);
    conditionPoint[y][x] = ArrangeContour;
    do
    {
        addPointsFront(pointsAuxiliary, x, y);
        addPointsDiagonal(pointsAuxiliary, x, y);
        findAppropriateNeigbours(pointsAuxiliary, pointsState, pointsNeigbour, x, y);
        quantityNeigbours = pointsNeigbour.count();
        if(quantityNeigbours == 1)
        {
            addNeigbour(pointsNeigbour[0], pointsContour, x, y);
        }
        else if(quantityNeigbours == 2)
        {
            QPoint pointExternal, pointInternal;
            if(pointsNeigbour[0] == findExternalPoint(x, y, pointsNeigbour[0], pointsNeigbour[1]))
            {
                pointExternal = pointsNeigbour[0];
                pointInternal = pointsNeigbour[1];
            }
            else
            {
                pointExternal = pointsNeigbour[1];
                pointInternal = pointsNeigbour[0];
            }
            addNeigbour(pointExternal, pointsContour, x, y);
        }
        pointsNeigbour.clear();
        pointsAuxiliary.clear();
    }while(quantityNeigbours);
    pointsContour << pointsContour[0];
    QFile fileOut("D:\\Inobitec\\Dicom\\Intensity\\dataset.json");
    fileOut.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream writeStream(&fileOut);
    for(int i = 0; i < pointsContour.count(); i++)
        writeStream << pointsContour[i].x() << " " << -pointsContour[i].y() << "\n";
    fileOut.close();
}
void ImageArea::findAppropriateNeigbours(QVector<QPoint> &points, QVector<int> &states, QVector<QPoint> &neigbours, int x, int y)
{
    for(int it = 0; it < points.count(); ++it)
    {
        if(conditionPoint[points[it].y()][points[it].x()] == Contour)
        {
            states.fill(0);
            if((points[it].y() == y) && (points[it].x() == (x + 1))) //вправо
            {
                states[0] = conditionPoint[y + 1][x];
                states[1] = conditionPoint[y + 1][x + 1];
            }
            else if((points[it].y() == y) && (points[it].x() == (x - 1))) //влево
            {
                states[0] = conditionPoint[y - 1][x];
                states[1] = conditionPoint[y - 1][x - 1];
            }
            else if((points[it].y() == (y - 1)) && (points[it].x() == x)) //вверх
            {
                states[0] = conditionPoint[y][x + 1];
                states[1] = conditionPoint[y - 1][x + 1];
            }
            else if((points[it].y() == (y + 1)) && (points[it].x() == x)) //вниз
            {
                states[0] = conditionPoint[y][x - 1];
                states[1] = conditionPoint[y + 1][x - 1];
            }
            else if((points[it].y() == (y - 1)) && (points[it].x() == (x + 1))) // вверх по диагонали вправо
            {
                states[2] = conditionPoint[y][x + 1];
                states[3] = conditionPoint[y - 1][x];
            }
            else if((points[it].y() == (y + 1)) && (points[it].x() == (x + 1))) // вниз по диагонали вправо
            {
                states[2] = conditionPoint[y + 1][x];
                states[3] = conditionPoint[y][x + 1];
            }
            else if((points[it].y() == (y + 1)) && (points[it].x() == (x - 1))) //вниз по диагонали влево
            {
                states[2] = conditionPoint[y][x - 1];
                states[3] = conditionPoint[y + 1][x];
            }
            else if((points[it].y() == (y - 1)) && (points[it].x() == (x - 1)))// вверх по диагонали влево
            {
                states[2] = conditionPoint[y - 1][x];
                states[3] = conditionPoint[y][x - 1];
            }
            if((states[0] == InnerArea && states[1] == Contour) || (states[0] == Contour && states[1] == InnerArea))
                 neigbours << points[it];
            else if((states[0] == InnerVoid && states[1] == Contour) || (states[0] == Contour && states[1] == InnerVoid))
                neigbours << points[it];
            else if(states[0] == InnerArea && states[1] == InnerArea)
                neigbours << points[it];
            else if(states[0] == InnerVoid && states[1] == InnerVoid)
                neigbours << points[it];
            else if((states[0] == Contour && states[1] == Contour) || (states[0] == ArrangeContour && states[1] == ArrangeContour))
                neigbours << points[it];
            else if((states[0] == Contour && states[1] == OuterArea) || (states[0] == OuterArea && states[1] == Contour))
                neigbours << points[it];
            else if((states[0] == InnerArea && states[1] == InnerVoid) || (states[0] == InnerVoid && states[1] == InnerArea))
                neigbours << points[it];
            else if((states[0] == ArrangeContour && states[1] == InnerArea) || (states[0] == InnerArea && states[1] == ArrangeContour))
                neigbours << points[it];
            else if((states[0] == ArrangeContour && states[1] == InnerVoid) || (states[0] == InnerVoid && states[1] == ArrangeContour))
                neigbours << points[it];
            else if(states[2] == InnerArea && states[3] == OuterArea)
                 neigbours << points[it];
            else if(states[2] == InnerVoid && states[3] == OuterArea)
                 neigbours << points[it];
        }
    }
}
void ImageArea::addNeigbour(QPoint &point, QVector<QPoint> &contour,int &x, int &y)
{
    y = point.y();
    x = point.x();
    contour << QPoint(x, y);
    conditionPoint[y][x] = ArrangeContour;
}
void ImageArea::addNeigbour(QPoint &point, int &x, int &y)
{
    y = point.y();
    x = point.x();
}
QPoint& ImageArea::findExternalPoint(int xMain, int yMain, QPoint &firstPoint, QPoint &secondPoint)
{
    if((firstPoint.x() == xMain) && ((yMain - firstPoint.y()) == 1))
    {
        if(conditionPoint[firstPoint.y() - 1][firstPoint.x()] == OuterArea) //если внешняя, то берём другую
            return secondPoint;
    }
    else if((firstPoint.x() == xMain) && ((yMain - firstPoint.y()) == -1))
    {
        if(conditionPoint[firstPoint.y() + 1][firstPoint.x()] == OuterArea) //если внешняя, то берём другую
            return secondPoint;
    }
    else if((firstPoint.y() == yMain) && ((xMain - firstPoint.x()) == 1))
    {
        if(conditionPoint[firstPoint.y()][firstPoint.x() - 1] == OuterArea) //если внешняя, то берём другую
            return secondPoint;
    }
    else if((firstPoint.y() == yMain) && ((xMain - firstPoint.x()) == -1))
    {
        if(conditionPoint[firstPoint.y()][firstPoint.x() + 1] == OuterArea) //если внешняя, то берём другую
            return secondPoint;
    }
    if((secondPoint.x() == xMain) && ((yMain - secondPoint.y()) == 1))
    {
        if(conditionPoint[secondPoint.y() - 1][secondPoint.x()] == OuterArea) //если внешняя, то берём другую
            return firstPoint;
    }
    else if((secondPoint.x() == xMain) && ((yMain - secondPoint.y()) == -1))
    {
        if(conditionPoint[secondPoint.y() + 1][secondPoint.x()] == OuterArea) //если внешняя, то берём другую
            return firstPoint;
    }
    else if((secondPoint.y() == yMain) && ((xMain - secondPoint.x()) == 1))
    {
        if(conditionPoint[secondPoint.y()][secondPoint.x() - 1] == OuterArea) //если внешняя, то берём другую
            return firstPoint;
    }
    else if((secondPoint.y() == yMain) && ((xMain - secondPoint.x()) == -1))
    {
        if(conditionPoint[secondPoint.y()][secondPoint.x() + 1] == OuterArea) //если внешняя, то берём другую
            return firstPoint;
    }
    return firstPoint;
}


