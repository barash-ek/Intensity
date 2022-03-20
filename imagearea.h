#ifndef IMAGEAREA_H
#define IMAGEAREA_H
#include "image.h"

class ImageArea
{
    Image image;
    int accuracy;
    QVector<QVector <int> > conditionPoint;
    void selectionArea(const QPoint& pointPress);
    void selectionInnerVoidPoints();
    void selectionBoundaryPoints();
    void deleteUnnecessaryPoint();
    void adjustVicinityComplicatedPoint();

public:
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
    ImageArea();
    ImageArea(const Image &picture, const QPoint &point, int a);
    Image& getImageObject();
    QVector<QVector <int> >* getconditionPoint();
    QImage drawArea(const QColor &color);
    static void addPointsFront(QVector<QPoint> &a, int x, int y, int width, int height);
    static void addPointsDiagonal(QVector<QPoint> &a, int x, int y, int width, int height);
};

#endif // IMAGEAREA_H
