#ifndef CONTOURBUILDER_H
#define CONTOURBUILDER_H
#include "contoursset.h"
class ImageArea;

class ContourBuilder
{
    QVector<QVector<int>> *conditionPoint;
    ContoursSet setContours;
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
    void addNeigbour(QVector<QPoint>& pointsContour, QPoint &point, int &x, int &y);
    QPoint& findExternalPoint(int xMain, int yMain, QPoint &firstPoint, QPoint &secondPoint);

public:
    ContourBuilder(ImageArea *area = nullptr);
    ContoursSet& getSetContours();
};

#endif // CONTOURBUILDER_H
