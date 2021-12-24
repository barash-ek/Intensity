#ifndef IMAGEAREA_H
#define IMAGEAREA_H
#include "image.h"

class ImageArea
{
private:
    Image image;
    int accuracy;
    int fallibility;
    QVector<QVector <int> > conditionPoint;
    enum PixelState
    {
        OuterArea = -1,
        NoState = 0,
        InnerArea = 1,
        ContourPoint = 2,
        ArrangeContour = 3,
        InnerVoid = 4,
    };

public:
    ImageArea();
    ImageArea(const Image &picture, const QPoint &point, int a);
    void setFallibility(int &a);
    Image& getImageObject();
    QVector<QVector <int> >* getconditionPoint();
    QImage drawArea(const QColor &color);
    void addPointsFront(QVector<QPoint> &a, int x, int y);
    void addPointsDiagonal(QVector<QPoint> &a, int x, int y);
};

#endif // IMAGEAREA_H
