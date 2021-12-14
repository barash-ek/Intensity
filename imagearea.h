#ifndef IMAGEAREA_H
#define IMAGEAREA_H
#include "image.h"

class ImageArea
{
private:
    Image image;
    int accuracy;
    QVector<QVector <int> > conditionPoint;
    enum PixelState
    {
        OuterArea = -1,
        NoState = 0,
        InnerArea = 1,
        InnerVoid = 4,
        Contour = 2,
        ArrangeContour = 3,
    };
    void addPointsFront(QVector<QPoint> &a, int x, int y);
    void addPointsDiagonal(QVector<QPoint> &a, int x, int y);
    void arrangePoints();
    void findAppropriateNeigbours(QVector<QPoint> &points, QVector<int> &states, QVector<QPoint> &neigbours, int x, int y);
    void addNeigbour(QPoint &point, QVector<QPoint> &contour, int &x, int &y);
    void addNeigbour(QPoint &point, int &x, int &y);
    QPoint& findExternalPoint(int xMain, int yMain, QPoint &firstPoint, QPoint &secondPoint);

public:
    ImageArea();
    ImageArea(const Image &picture, const QPoint &point, int a);
    QImage drawArea(const QColor &color);
};

#endif // IMAGEAREA_H
