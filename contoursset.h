#ifndef CONTOURSSET_H
#define CONTOURSSET_H
#include "contour.h"


class ContoursSet
{
private:
    QVector<Contour> setContours;
public:
    ContoursSet();
    QVector<Contour>& getSetContours();
    QVector<QPoint> buildApproximation(int fallibility);
    void operator<<(Contour &contour);
};

#endif // CONTOURSSET_H
