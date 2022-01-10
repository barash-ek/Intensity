#ifndef CONTOUR_H
#define CONTOUR_H

class Contour
{
    QVector<QPoint> pointsContour;

public:
    Contour(QVector<QPoint> *points = nullptr);
    QVector<QPoint>& getPoints();
};

#endif // CONTOUR_H
