#ifndef CONTOUR_H
#define CONTOUR_H

class Contour
{
    QVector<QPoint> pointsContour;
    bool isExternalContour;

public:
    Contour(QVector<QPoint> *points = Q_NULLPTR);
    QVector<QPoint>& getPoints();
    bool getStateContour();
    void setStateContour(bool state);
};

#endif // CONTOUR_H
