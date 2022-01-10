#ifndef CONTOURSSET_H
#define CONTOURSSET_H
#include "contour.h"

class ContoursSet
{
    QVector<Contour> setContours;
    QVector<QVector<QPoint>> nodesApproximation;

public:
    ContoursSet();
    QVector<Contour>& getSetContours();
    QVector<QVector<QPoint>>* getNodesApproximation();
    void buildApproximation(int fallibility);
    void operator<<(const Contour &contour);
};

#endif // CONTOURSSET_H
