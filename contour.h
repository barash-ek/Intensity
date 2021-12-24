#ifndef CONTOUR_H
#define CONTOUR_H
#include "imagearea.h"

class Contour
{
private:
    QVector<QVector<int>> *conditionPoint;
    QVector<QPoint> pointsContour;  // arranged contour
    QVector<QPoint> chosenPoints; // for approximation
    enum PixelState
    {
        OuterArea = -1,
        NoState = 0,
        InnerArea = 1,
        ContourPoint = 2,
        ArrangeContour = 3,
        InnerVoid = 4,
        OutImage = 5,
    };
    void findAppropriateNeigbours(QVector<QPoint> &points, QVector<int> &states, QVector<QPoint> &neigbours, int x, int y);
    void addNeigbour(QPoint &point, int &x, int &y);
    QPoint& findExternalPoint(int xMain, int yMain, QPoint &firstPoint, QPoint &secondPoint);

public:
    Contour();
    Contour(ImageArea &area);
    void buildApproximation(int fallibility);
    QVector<QPoint>& getPointsApproximation();
};

#endif // CONTOUR_H
