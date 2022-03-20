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
    //do
    //{
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
            QVector<int> states(2);
            pointsContour << QPoint(x, y);
            (*conditionPoint)[y][x] = ImageArea::ArrangeContour;
            int quantityNeigbours = 0;
            do
            {
                area->addPointsFront(pointsAuxiliary, x, y, imageWidth, imageHeight);
                for(int i = 0, t = pointsAuxiliary.size(); i < t; ++i)
                {
                    const QPoint &point = pointsAuxiliary[i];
                    if(conditionPoint->at(point.y()).at(point.x()) == ImageArea::ContourPoint)
                    {
                        saveInformationAboutNeigbour(point, states, x, y);
                        if(isAppropriateNeigbour(point, states))
                            pointsNeigbour << point;
                    }
                }
                quantityNeigbours = pointsNeigbour.size();
                if(quantityNeigbours == 1)
                {
                    addNeigbour(pointsContour, pointsNeigbour[0], x, y);
                }
                else if(quantityNeigbours == 2)
                {
                    /*//quantityNeigbours = 0;
                    QVector<QPoint> additionalPointsAuxiliary;
                    const QPoint &explorePoint = pointsNeigbour[0];
                    const int xPoint = explorePoint.x();
                    const int yPoint = explorePoint.y();
                    bool condition = true;
                    area->addPointsFront(additionalPointsAuxiliary, xPoint, yPoint, imageWidth, imageHeight);
                    for(int i = 0, t = pointsAuxiliary.size(); i < t; ++i)
                    {
                        const QPoint &point = pointsAuxiliary[i];
                        if(conditionPoint->at(point.y()).at(point.x()) == ImageArea::ContourPoint)
                        {
                            QVector<int> additionalStates(2);
                            saveInformationAboutNeigbour(point, additionalStates, x, y);
                            if(!isAppropriateNeigbour(point, additionalStates))
                            {
                                condition = false;
                                break;
                            }
                        }
                     }
                    if(condition)
                        addNeigbour(pointsContour, pointsNeigbour[0], x, y);
                    else
                        addNeigbour(pointsContour, pointsNeigbour[1], x, y);*/
                    quantityNeigbours = 0;
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
    //}while(indicator);
}
void ContourBuilder::saveInformationAboutNeigbour(const QPoint &point, QVector<int> &states, int x, int y)
{
    int xPoint = point.x();
    int yPoint = point.y();
    states.fill(0);
    if((yPoint == y) && (xPoint == (x + 1))) //сосед справа
    {
        if(y != (conditionPoint->size() - 1))
        {
            states[0] = conditionPoint->at(y + 1).at(x);
            states[1] = conditionPoint->at(y + 1).at(x + 1);
        }
        else
            states[0] = states[1] = ImageArea::OutImage;
    }
    else if((yPoint == y) && (xPoint == (x - 1))) //сосед слева
    {
        if(y != 0)
        {
            states[0] = conditionPoint->at(y - 1).at(x);
            states[1] = conditionPoint->at(y - 1).at(x - 1);
        }
        else
            states[0] = states[1] = ImageArea::OutImage;
    }
    else if((yPoint == (y - 1)) && (xPoint == x)) //сосед сверху
    {
        if(x != (conditionPoint[0].size() - 1))
        {
            states[0] = conditionPoint->at(y).at(x + 1);
            states[1] = conditionPoint->at(y - 1).at(x + 1);
        }
        else
            states[0] = states[1] = ImageArea::OutImage;
    }
    else if((yPoint == (y + 1)) && (xPoint == x)) //сосед снизу
    {
        if(x != 0)
        {
            states[0] = conditionPoint->at(y).at(x - 1);
            states[1] = conditionPoint->at(y + 1).at(x - 1);
        }
        else
            states[0] = states[1] = ImageArea::OutImage;
    }
}
bool ContourBuilder::isAppropriateNeigbour(const QPoint &point, QVector<int> &states)
{
    bool condition = false;
    if((states[0] == ImageArea::InnerArea && states[1] == ImageArea::ContourPoint) || (states[0] == ImageArea::ContourPoint && states[1] == ImageArea::InnerArea))
         condition = true;
    else if((states[0] == ImageArea::InnerVoid && states[1] == ImageArea::ContourPoint) || (states[0] == ImageArea::ContourPoint && states[1] == ImageArea::InnerVoid))
         condition = true;
    else if((states[0] == ImageArea::InnerArea && states[1] == ImageArea::InnerVoid) || (states[0] == ImageArea::InnerVoid && states[1] == ImageArea::InnerArea))
         condition = true;
    else if(states[0] == ImageArea::InnerArea && states[1] == ImageArea::InnerArea)
         condition = true;
    else if(states[0] == ImageArea::InnerVoid && states[1] == ImageArea::InnerVoid)
         condition = true;
    else if(states[0] == ImageArea::ArrangeContour && states[1] == ImageArea::InnerArea)
         condition = true;
    else if(states[0] == ImageArea::ArrangeContour && states[1] == ImageArea::InnerVoid)
         condition = true;
    else if(states[0] == ImageArea::OutImage && states[1] == ImageArea::OutImage)
         condition = true;
    return condition;
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
    const int xFisrt = firstPoint.x();
    const int yFirst = firstPoint.y();
    if((xFisrt == xMain) && ((yMain - yFirst) == 1))
    {
        if(conditionPoint->at(yFirst - 1).at(xFisrt) == ImageArea::OuterArea) //если внешняя, то берём другую
            return secondPoint;
    }
    else if((xFisrt == xMain) && ((yMain - yFirst) == -1))
    {
        if(conditionPoint->at(yFirst + 1).at(xFisrt) == ImageArea::OuterArea) //если внешняя, то берём другую
            return secondPoint;
    }
    else if((yFirst == yMain) && ((xMain - xFisrt) == 1))
    {
        if(conditionPoint->at(yFirst).at(xFisrt - 1) == ImageArea::OuterArea) //если внешняя, то берём другую
            return secondPoint;
    }
    else if((yFirst == yMain) && ((xMain - xFisrt) == -1))
    {
        if(conditionPoint->at(yFirst).at(xFisrt + 1) == ImageArea::OuterArea) //если внешняя, то берём другую
            return secondPoint;
    }
    const int xSecond = secondPoint.x();
    const int ySecond = secondPoint.y();
    if((xSecond == xMain) && ((yMain - ySecond) == 1))
    {
        if(conditionPoint->at(ySecond - 1).at(xSecond) == ImageArea::OuterArea) //если внешняя, то берём другую
            return firstPoint;
    }
    else if((xSecond == xMain) && ((yMain - ySecond) == -1))
    {
        if(conditionPoint->at(ySecond + 1).at(xSecond) == ImageArea::OuterArea) //если внешняя, то берём другую
            return firstPoint;
    }
    else if((ySecond == yMain) && ((xMain - xSecond) == 1))
    {
        if(conditionPoint->at(ySecond).at(xSecond - 1) == ImageArea::OuterArea) //если внешняя, то берём другую
            return firstPoint;
    }
    else if((ySecond == yMain) && ((xMain - xSecond) == -1))
    {
        if(conditionPoint->at(ySecond).at(xSecond + 1) == ImageArea::OuterArea) //если внешняя, то берём другую
            return firstPoint;
    }
    return firstPoint;
}
ContoursSet& ContourBuilder::getSetContours()
{
    return setContours;
}
