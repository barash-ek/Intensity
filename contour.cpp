#include "contour.h"

Contour::Contour(QVector<QPoint> *points): isExternalContour(true)
{
    pointsContour = *points;
}
QVector<QPoint>& Contour::getPoints()
{
    return pointsContour;
}

bool Contour::getStateContour()
{
    return isExternalContour;
}

void Contour::setStateContour(bool state)
{
    isExternalContour = state;
}
