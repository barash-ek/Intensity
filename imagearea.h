#ifndef IMAGEAREA_H
#define IMAGEAREA_H
#include "image.h"
#include <QPoint>

class ImageArea
{
private:
    Image image;
    int accuracy;
    QVector<QVector <int> > conditionPoint;
public:
    ImageArea();
    ImageArea(const Image &picture, const QPoint &point, int a);
    QImage drawArea(const QColor &color);
};

#endif // IMAGEAREA_H
