#include "contourbuilder.h"
#include "contoursset.h"
#include "imagearea.cpp"

ContourBuilder::ContourBuilder(ImageArea *area): imageArea(Q_NULLPTR), conditionPoint(Q_NULLPTR)
{
    if(area->getconditionPoint() != Q_NULLPTR)
    {
        imageArea = area;
        conditionPoint = imageArea->getconditionPoint();

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
                QVector<int> states(2);
                pointsContour << QPoint(x, y);
                (*conditionPoint)[y][x] = ImageArea::ArrangeContour;
                // проверка, внутренний контур или внешний
                bool isContourExternal = defineIsContourExternal(x, y, imageWidth, imageHeight);
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
                        bool resultFirstNeigbour = leftNeigbourNotFromArea(pointsNeigbour[0], QPoint(x, y), imageWidth, imageHeight);
                        bool resultSecondNeigbour = leftNeigbourNotFromArea(pointsNeigbour[1], QPoint(x, y), imageWidth, imageHeight);

                        if(resultFirstNeigbour && !resultSecondNeigbour)
                            addNeigbour(pointsContour, pointsNeigbour[0], x, y);
                        else if(!resultFirstNeigbour && resultSecondNeigbour)
                            addNeigbour(pointsContour, pointsNeigbour[1], x, y);
                        else
                        {
                            qDebug() << resultFirstNeigbour << resultSecondNeigbour;
                            qDebug() << "You're in such case...";
                            quantityNeigbours = 0;
                        }
                    }
                    pointsNeigbour.clear();
                    pointsAuxiliary.clear();
                 }while(quantityNeigbours);
                 pointsContour << pointsContour[0];
                 Contour contour(&pointsContour);
                 contour.setStateContour(isContourExternal);
                 setContours << contour;
            }
            else
                indicator = false;
        }while(indicator);
    }
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
    else if(states[0] == ImageArea::InnerArea && states[1] == ImageArea::ArrangeContour)
         condition = true;
    else if(states[0] == ImageArea::InnerVoid && states[1] == ImageArea::ArrangeContour)
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

bool ContourBuilder::leftNeigbourNotFromArea(const QPoint &neigbour, const QPoint &point, int imageWidth, int imageHeight)
{
    int xNeigbour = neigbour.x();
    int yNeigbour = neigbour.y();

    int xPoint = point.x();
    int yPoint = point.y();

    if (xNeigbour == xPoint)
    {
        if(yNeigbour == yPoint + 1)
        {
            if(xNeigbour != (imageWidth - 1))
            {
                if(conditionPoint->at(yNeigbour).at(xNeigbour + 1) == ImageArea::OuterArea)
                    return true;
            }
            else
                return true;
        }
        else if(yNeigbour == yPoint - 1)
        {
            if(xNeigbour != 0)
            {
                if(conditionPoint->at(yNeigbour).at(xNeigbour - 1) == ImageArea::OuterArea)
                    return true;
            }
            else
                return true;
        }
    }
    else if(yNeigbour == yPoint)
    {
        if(xNeigbour == xPoint + 1)
        {
            if(yNeigbour != 0)
            {
                if(conditionPoint->at(yNeigbour - 1).at(xNeigbour) == ImageArea::OuterArea)
                    return true;
            }
            else
                return true;
        }
        else if(xNeigbour == xPoint - 1)
        {
            if(yNeigbour != (imageHeight - 1))
            {
                if(conditionPoint->at(yNeigbour + 1).at(xNeigbour) == ImageArea::OuterArea)
                    return true;
            }
            else
                return true;
        }
    }
    return false;
}

bool ContourBuilder::defineIsContourExternal(const int x, const int y, const int imageWidth, const int imageHeight)
{
    QVector<QPoint> pointsAuxiliary;
    imageArea->addPointsFront(pointsAuxiliary, x, y, imageWidth, imageHeight);
    imageArea->addPointsDiagonal(pointsAuxiliary, x, y, imageWidth, imageHeight);
    for(int i = 0, t = pointsAuxiliary.size(); i < t; ++i)
    {
        const QPoint &point = pointsAuxiliary[i];
        if(conditionPoint->at(point.y()).at(point.x()) == ImageArea::OuterArea)
        {
            if(point.y() == (y + 1) && (point.x() - x) >= 0)
                return false;
        }
    }
    return true;
}
ContoursSet& ContourBuilder::getSetContours()
{
    return setContours;
}
