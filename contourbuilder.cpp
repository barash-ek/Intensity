#include "contourbuilder.h"
#include "contoursset.h"
#include "imagearea.cpp"

ContourBuilder::ContourBuilder(ImageArea *area): conditionPoint(0x0)
{
    if(area->getconditionPoint() != nullptr)
        conditionPoint = area->getconditionPoint();
    const int imageHeight = area->getImageObject().getImage().height();
    const int imageWidth = area->getImageObject().getImage().width();
    int x = -1, y = -1;
    bool indicator = false;
    do
    {
        x = -1;
        y = -1;
        indicator = false;
        for(int i = 0; i < imageHeight; ++i)
        {
            const QVector<int>& row = conditionPoint->at(i);
            const int *rowPointer = &row[0];
            for(int j = 0; j < imageWidth; ++j)
            {
                if(rowPointer[j] == ImageArea::ContourPoint)
                {
                    x = j;
                    y = i;
                    indicator = true;
                    break;
                }
            }
            if(indicator)
                break;
        }
        if(x != - 1 && y != - 1)
        {
            QVector<QPoint> pointsContour, pointsAuxiliary, pointsNeigbour;
            int quantityNeigbours = 0;
            QVector<int> pointsState(4);
            pointsContour << QPoint(x, y);
            (*conditionPoint)[y][x] = ImageArea::ArrangeContour;
            do
            {
                area->addPointsFront(pointsAuxiliary, x, y, imageWidth, imageHeight);
                area->addPointsDiagonal(pointsAuxiliary, x, y, imageWidth, imageHeight);
                findAppropriateNeigbours(pointsAuxiliary, pointsState, pointsNeigbour, x, y);
                quantityNeigbours = pointsNeigbour.size();
                if(quantityNeigbours == 1)
                {
                    addNeigbour(pointsContour, pointsNeigbour[0], x, y);
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
                    addNeigbour(pointsContour, pointExternal, x, y);
                }
                pointsNeigbour.clear();
                pointsAuxiliary.clear();
            }while(quantityNeigbours);
            pointsContour << pointsContour[0];
            Contour contour(&pointsContour);
            setContours << contour;
        }
        else
            indicator = false;
    }while(indicator);
}
void ContourBuilder::findAppropriateNeigbours(QVector<QPoint> &points, QVector<int> &states, QVector<QPoint> &neigbours, int x, int y)
{
    for(int it = 0; it < points.size(); ++it)
    {
        if((*conditionPoint)[points[it].y()][points[it].x()] == ImageArea::ContourPoint)
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
                    states[0] = states[1] = ImageArea::OutImage;
            }
            else if((points[it].y() == y) && (points[it].x() == (x - 1))) //влево
            {
                if(y != 0)
                {
                    states[0] = (*conditionPoint)[y - 1][x];
                    states[1] = (*conditionPoint)[y - 1][x - 1];
                }
                else
                    states[0] = states[1] = ImageArea::OutImage;
            }
            else if((points[it].y() == (y - 1)) && (points[it].x() == x)) //вверх
            {
                if(x != (conditionPoint[0].size() - 1))
                {
                    states[0] = (*conditionPoint)[y][x + 1];
                    states[1] = (*conditionPoint)[y - 1][x + 1];
                }
                else
                    states[0] = states[1] = ImageArea::OutImage;
            }
            else if((points[it].y() == (y + 1)) && (points[it].x() == x)) //вниз
            {
                if(x != 0)
                {
                    states[0] = (*conditionPoint)[y][x - 1];
                    states[1] = (*conditionPoint)[y + 1][x - 1];
                }
                else
                    states[0] = states[1] = ImageArea::OutImage;
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
            if((states[0] == ImageArea::InnerArea && states[1] == ImageArea::ContourPoint) || (states[0] == ImageArea::ContourPoint && states[1] == ImageArea::InnerArea))
                 neigbours << points[it];
            else if((states[0] == ImageArea::InnerVoid && states[1] == ImageArea::ContourPoint) || (states[0] == ImageArea::ContourPoint && states[1] == ImageArea::InnerVoid))
                neigbours << points[it];
            else if(states[0] == ImageArea::InnerArea && states[1] == ImageArea::InnerArea)
                neigbours << points[it];
            else if(states[0] == ImageArea::InnerVoid && states[1] == ImageArea::InnerVoid)
                neigbours << points[it];
            else if((states[0] == ImageArea::ContourPoint && states[1] == ImageArea::ContourPoint) || (states[0] == ImageArea::ArrangeContour && states[1] == ImageArea::ArrangeContour))
                neigbours << points[it];
            else if((states[0] == ImageArea::ContourPoint && states[1] == ImageArea::OuterArea) || (states[0] == ImageArea::OuterArea && states[1] == ImageArea::ContourPoint))
                neigbours << points[it];
            else if((states[0] == ImageArea::InnerArea && states[1] == ImageArea::InnerVoid) || (states[0] == ImageArea::InnerVoid && states[1] == ImageArea::InnerArea))
                neigbours << points[it];
            else if((states[0] == ImageArea::ArrangeContour && states[1] == ImageArea::InnerArea) || (states[0] == ImageArea::InnerArea && states[1] == ImageArea::ArrangeContour))
                neigbours << points[it];
            else if((states[0] == ImageArea::ArrangeContour && states[1] == ImageArea::InnerVoid) || (states[0] == ImageArea::InnerVoid && states[1] == ImageArea::ArrangeContour))
                neigbours << points[it];
            else if(states[2] == ImageArea::InnerArea && states[3] == ImageArea::OuterArea)
                 neigbours << points[it];
            else if(states[2] == ImageArea::InnerVoid && states[3] == ImageArea::OuterArea)
                 neigbours << points[it];
            else if(states[0] == ImageArea::OutImage && states[1] == ImageArea::OutImage)
                neigbours << points[it];
        }
    }
}
void ContourBuilder::addNeigbour(QVector<QPoint>& pointsContour, QPoint &point, int &x, int &y)
{
    y = point.y();
    x = point.x();
    pointsContour << QPoint(x, y);
    (*conditionPoint)[y][x] = ImageArea::ArrangeContour;
}
QPoint& ContourBuilder::findExternalPoint(int xMain, int yMain, QPoint &firstPoint, QPoint &secondPoint)
{
    if((firstPoint.x() == xMain) && ((yMain - firstPoint.y()) == 1))
    {
        if((*conditionPoint)[firstPoint.y() - 1][firstPoint.x()] == ImageArea::OuterArea) //если внешняя, то берём другую
            return secondPoint;
    }
    else if((firstPoint.x() == xMain) && ((yMain - firstPoint.y()) == -1))
    {
        if((*conditionPoint)[firstPoint.y() + 1][firstPoint.x()] == ImageArea::OuterArea) //если внешняя, то берём другую
            return secondPoint;
    }
    else if((firstPoint.y() == yMain) && ((xMain - firstPoint.x()) == 1))
    {
        if((*conditionPoint)[firstPoint.y()][firstPoint.x() - 1] == ImageArea::OuterArea) //если внешняя, то берём другую
            return secondPoint;
    }
    else if((firstPoint.y() == yMain) && ((xMain - firstPoint.x()) == -1))
    {
        if((*conditionPoint)[firstPoint.y()][firstPoint.x() + 1] == ImageArea::OuterArea) //если внешняя, то берём другую
            return secondPoint;
    }
    if((secondPoint.x() == xMain) && ((yMain - secondPoint.y()) == 1))
    {
        if((*conditionPoint)[secondPoint.y() - 1][secondPoint.x()] == ImageArea::OuterArea) //если внешняя, то берём другую
            return firstPoint;
    }
    else if((secondPoint.x() == xMain) && ((yMain - secondPoint.y()) == -1))
    {
        if((*conditionPoint)[secondPoint.y() + 1][secondPoint.x()] == ImageArea::OuterArea) //если внешняя, то берём другую
            return firstPoint;
    }
    else if((secondPoint.y() == yMain) && ((xMain - secondPoint.x()) == 1))
    {
        if((*conditionPoint)[secondPoint.y()][secondPoint.x() - 1] == ImageArea::OuterArea) //если внешняя, то берём другую
            return firstPoint;
    }
    else if((secondPoint.y() == yMain) && ((xMain - secondPoint.x()) == -1))
    {
        if((*conditionPoint)[secondPoint.y()][secondPoint.x() + 1] == ImageArea::OuterArea) //если внешняя, то берём другую
            return firstPoint;
    }
    return firstPoint;
}
ContoursSet& ContourBuilder::getSetContours()
{
    return setContours;
}
