#include "contoursset.h"

ContoursSet::ContoursSet()
{}
QVector<Contour>& ContoursSet::getSetContours()
{
    return setContours;
}
QVector<QPoint> ContoursSet::buildApproximation(int fallibility)
{
    QVector<QPoint> pointsContour;
    QVector<QPoint> chosenPoints;
    if(!setContours.isEmpty())
    {
        pointsContour = setContours[0].getPoints();
        chosenPoints << pointsContour[0];
        bool condition = true;
        for(int j = 0; j < pointsContour.size(); ++j)  // j отвечает за стартовую точку
        {
            condition = true;
            QVector2D startPoint(chosenPoints[chosenPoints.size() - 1]);
            for(int i = j + 1; i < pointsContour.size(); ++i)
            {
                QVector2D endPoint(pointsContour[i]);
                QQueue<QVector2D> pointsBetween;
                for(int k = i - 1; k > j; --k)
                    if(pointsContour[k] != chosenPoints[chosenPoints.size() - 1])
                        pointsBetween.enqueue(QVector2D(pointsContour[k]));
                if(!pointsBetween.isEmpty())
                {
                    QVector2D direction((endPoint - startPoint).normalized());
                    while(!pointsBetween.isEmpty())
                    {
                        condition = pointsBetween.dequeue().distanceToLine(startPoint, direction) <= fallibility;
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
        chosenPoints << pointsContour[0];
    }
    return chosenPoints;
}
void ContoursSet::operator<<(Contour &contour)
{
    setContours << contour;
}
