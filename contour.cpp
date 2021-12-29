#include "contour.h"
#include <QFile>
#include <QTextStream>
#include <QtMath>
#include <QVector2D>

Contour::Contour()
{}
Contour::Contour(ImageArea &area): conditionPoint(0x0)
{
    QVector<QPoint> pointsAuxiliary, pointsNeigbour;
    int imageHeight = area.getImageObject().getImage().height();
    int imageWidth = area.getImageObject().getImage().width();
    int x = -1, y = -1;
    if(area.getconditionPoint() != nullptr)
        conditionPoint = area.getconditionPoint();
    for(int i = 0; i < imageHeight; ++i)
    {
        for(int j = 0; j < imageWidth; ++j)
        {
            if((*conditionPoint)[i][j] == ContourPoint)
            {
                x = j;
                y = i;
                break;
            }
        }
        if(y >= 0)
            break;
    }
    if(x != - 1 && y != - 1)
    {
        int quantityNeigbours = 0;
        QVector<int> pointsState(4);
        pointsContour << QPoint(x, y);
        (*conditionPoint)[y][x] = ArrangeContour;
        do
        {
            area.addPointsFront(pointsAuxiliary, x, y, imageWidth, imageHeight);
            area.addPointsDiagonal(pointsAuxiliary, x, y, imageWidth, imageHeight);
            findAppropriateNeigbours(pointsAuxiliary, pointsState, pointsNeigbour, x, y);
            quantityNeigbours = pointsNeigbour.size();
            if(quantityNeigbours == 1)
            {
                addNeigbour(pointsNeigbour[0], x, y);
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
                addNeigbour(pointExternal, x, y);
            }
            pointsNeigbour.clear();
            pointsAuxiliary.clear();
        }while(quantityNeigbours);
        pointsContour << pointsContour[0];
        QFile fileOut("dataset.json");
        fileOut.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream writeStream(&fileOut);
        for(int i = 0; i < pointsContour.size(); i++)
            writeStream << pointsContour[i].x() << " " << -pointsContour[i].y() << "\n";
        fileOut.close();
    }
}
void Contour::findAppropriateNeigbours(QVector<QPoint> &points, QVector<int> &states, QVector<QPoint> &neigbours, int x, int y)
{
    for(int it = 0; it < points.size(); ++it)
    {
        if((*conditionPoint)[points[it].y()][points[it].x()] == ContourPoint)
        {
            states.fill(0);
            if((points[it].y() == y) && (points[it].x() == (x + 1))) //вправо
            {
                if(y != (conditionPoint->size() - 1))
                {
                    states[0] = (*conditionPoint)[y + 1][x];
                    states[1] = (*conditionPoint)[y + 1][x + 1];
                }
                else
                    states[0] = states[1] = OutImage;
            }
            else if((points[it].y() == y) && (points[it].x() == (x - 1))) //влево
            {
                if(y != 0)
                {
                    states[0] = (*conditionPoint)[y - 1][x];
                    states[1] = (*conditionPoint)[y - 1][x - 1];
                }
                else
                    states[0] = states[1] = OutImage;
            }
            else if((points[it].y() == (y - 1)) && (points[it].x() == x)) //вверх
            {
                if(x != (conditionPoint[0].size() - 1))
                {
                    states[0] = (*conditionPoint)[y][x + 1];
                    states[1] = (*conditionPoint)[y - 1][x + 1];
                }
                else
                    states[0] = states[1] = OutImage;
            }
            else if((points[it].y() == (y + 1)) && (points[it].x() == x)) //вниз
            {
                if(x != 0)
                {
                    states[0] = (*conditionPoint)[y][x - 1];
                    states[1] = (*conditionPoint)[y + 1][x - 1];
                }
                else
                    states[0] = states[1] = OutImage;
            }
            else if((points[it].y() == (y - 1)) && (points[it].x() == (x + 1))) // вверх по диагонали вправо
            {
                states[2] = (*conditionPoint)[y][x + 1];
                states[3] = (*conditionPoint)[y - 1][x];
            }
            else if((points[it].y() == (y + 1)) && (points[it].x() == (x + 1))) // вниз по диагонали вправо
            {
                states[2] = (*conditionPoint)[y + 1][x];
                states[3] = (*conditionPoint)[y][x + 1];
            }
            else if((points[it].y() == (y + 1)) && (points[it].x() == (x - 1))) //вниз по диагонали влево
            {
                states[2] = (*conditionPoint)[y][x - 1];
                states[3] = (*conditionPoint)[y + 1][x];
            }
            else if((points[it].y() == (y - 1)) && (points[it].x() == (x - 1)))// вверх по диагонали влево
            {
                states[2] = (*conditionPoint)[y - 1][x];
                states[3] = (*conditionPoint)[y][x - 1];
            }
            if((states[0] == InnerArea && states[1] == ContourPoint) || (states[0] == ContourPoint && states[1] == InnerArea))
                 neigbours << points[it];
            else if((states[0] == InnerVoid && states[1] == ContourPoint) || (states[0] == ContourPoint && states[1] == InnerVoid))
                neigbours << points[it];
            else if(states[0] == InnerArea && states[1] == InnerArea)
                neigbours << points[it];
            else if(states[0] == InnerVoid && states[1] == InnerVoid)
                neigbours << points[it];
            else if((states[0] == ContourPoint && states[1] == ContourPoint) || (states[0] == ArrangeContour && states[1] == ArrangeContour))
                neigbours << points[it];
            else if((states[0] == ContourPoint && states[1] == OuterArea) || (states[0] == OuterArea && states[1] == ContourPoint))
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
            else if(states[0] == OutImage && states[1] == OutImage)
                neigbours << points[it];
        }
    }
}
void Contour::addNeigbour(QPoint &point, int &x, int &y)
{
    y = point.y();
    x = point.x();
    pointsContour << QPoint(x, y);
    (*conditionPoint)[y][x] = ArrangeContour;
}
QPoint& Contour::findExternalPoint(int xMain, int yMain, QPoint &firstPoint, QPoint &secondPoint)
{
    if((firstPoint.x() == xMain) && ((yMain - firstPoint.y()) == 1))
    {
        if((*conditionPoint)[firstPoint.y() - 1][firstPoint.x()] == OuterArea) //если внешняя, то берём другую
            return secondPoint;
    }
    else if((firstPoint.x() == xMain) && ((yMain - firstPoint.y()) == -1))
    {
        if((*conditionPoint)[firstPoint.y() + 1][firstPoint.x()] == OuterArea) //если внешняя, то берём другую
            return secondPoint;
    }
    else if((firstPoint.y() == yMain) && ((xMain - firstPoint.x()) == 1))
    {
        if((*conditionPoint)[firstPoint.y()][firstPoint.x() - 1] == OuterArea) //если внешняя, то берём другую
            return secondPoint;
    }
    else if((firstPoint.y() == yMain) && ((xMain - firstPoint.x()) == -1))
    {
        if((*conditionPoint)[firstPoint.y()][firstPoint.x() + 1] == OuterArea) //если внешняя, то берём другую
            return secondPoint;
    }
    if((secondPoint.x() == xMain) && ((yMain - secondPoint.y()) == 1))
    {
        if((*conditionPoint)[secondPoint.y() - 1][secondPoint.x()] == OuterArea) //если внешняя, то берём другую
            return firstPoint;
    }
    else if((secondPoint.x() == xMain) && ((yMain - secondPoint.y()) == -1))
    {
        if((*conditionPoint)[secondPoint.y() + 1][secondPoint.x()] == OuterArea) //если внешняя, то берём другую
            return firstPoint;
    }
    else if((secondPoint.y() == yMain) && ((xMain - secondPoint.x()) == 1))
    {
        if((*conditionPoint)[secondPoint.y()][secondPoint.x() - 1] == OuterArea) //если внешняя, то берём другую
            return firstPoint;
    }
    else if((secondPoint.y() == yMain) && ((xMain - secondPoint.x()) == -1))
    {
        if((*conditionPoint)[secondPoint.y()][secondPoint.x() + 1] == OuterArea) //если внешняя, то берём другую
            return firstPoint;
    }
    return firstPoint;
}
void Contour::buildApproximation(int fallibility)
{
    if(!pointsContour.isEmpty())
    {
        int i = 0, j = 0;
        int differenceX = 0, differenceY = 0;
        chosenPoints << pointsContour[0];
        while(i != (pointsContour.size() - 1))
        {
            differenceX = 0;
            differenceY = 0;
            while((qPow((qreal)differenceX, 2) + qPow((qreal)differenceY, 2) <= qPow((qreal)fallibility, 2)) && (i != (pointsContour.size() - 1)))
            {
                ++i;
                differenceX = chosenPoints[j].x() - pointsContour[i].x();
                differenceY = chosenPoints[j].y() - pointsContour[i].y();
            }
            if(i != (pointsContour.size() - 1))
                chosenPoints << pointsContour[i - 1];
            ++j;
        }
        chosenPoints << chosenPoints[0];
        QFile fileOut("contourApproximation.json");
        fileOut.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream writeStream(&fileOut);
        for(int i = 0; i < chosenPoints.size(); i++)
            writeStream << chosenPoints[i].x() << " " << -chosenPoints[i].y() << "\n";
        //writeStream << chosenPoints[0].x() << " " << -chosenPoints[0].y() << "\n";
        fileOut.close();
    }
}
QVector<QPoint>& Contour::getPointsApproximation()
{
    return chosenPoints;
}
