#include "contour.h"

Contour::Contour(QVector<QPoint> *points)
{
    pointsContour = *points;
}
QVector<QPoint>& Contour::getPoints()
{
    return pointsContour;
}
