#include "contoursset.h"

ContoursSet::ContoursSet()
{}
QVector<Contour>& ContoursSet::getSetContours()
{
    return setContours;
}
void ContoursSet::buildApproximation(int fallibility)
{
    const int sizeSetContours = setContours.size();
    nodesApproximation.resize(sizeSetContours);
    for(int i = 0; i < sizeSetContours; ++i)
    {
        QVector<QPoint> pointsContour = setContours[i].getPoints();
        QVector<QPoint> chosenPoints;
        chosenPoints << pointsContour[0];
        bool condition = true;
        for(int j = 0, t = pointsContour.size(); j < t; ++j)  // j отвечает за стартовую точку
        {
            condition = true;
            int sizeChosenPoints = chosenPoints.size();
            QVector2D startPoint(chosenPoints[sizeChosenPoints - 1]);
            for(int i = j + 1,  p = pointsContour.size(); i < p; ++i)
            {
                QVector2D endPoint(pointsContour[i]);
                QQueue<QVector2D> pointsBetween;
                for(int k = i - 1; k > j; --k)
                    if(pointsContour[k] != chosenPoints[sizeChosenPoints - 1])
                        pointsBetween.enqueue(QVector2D(pointsContour[k]));
                if(!pointsBetween.isEmpty())
                {
                    QVector2D direction((endPoint - startPoint).normalized());
                    while(!pointsBetween.isEmpty())
                    {
                        const QVector2D &pointInQueue = pointsBetween.dequeue();
                        condition = pointInQueue.distanceToLine(startPoint, direction) <= fallibility;
                        if(!condition)
                        {
                            chosenPoints << pointsContour[i - 1];
                            j = i - 2;
                            break;
                        }
                    }
                }
                if(!condition)
                    break;
            }
        }
        nodesApproximation[i] = chosenPoints;
    }
}
void ContoursSet::operator<<(const Contour &contour)
{
    setContours << contour;
}
QVector<QVector<QPoint>>* ContoursSet::getNodesApproximation()
{
    return &nodesApproximation;
}
