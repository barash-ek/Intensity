#ifndef CONTOURBUILDER_H
#define CONTOURBUILDER_H
#include "contoursset.h"
class ImageArea;

class ContourBuilder
{
    ImageArea *imageArea;
    QVector<QVector<int>> *conditionPoint;
    ContoursSet setContours;
    //void findAppropriateNeigbours(QVector<QPoint> &points, QVector<int> &states, QVector<QPoint> &neigbours, int x, int y);
    void saveInformationAboutNeigbour(const QPoint &point, QVector<int> &states,int x, int y);
    bool isAppropriateNeigbour(const QPoint &point, QVector<int> &states);
    void addNeigbour(QVector<QPoint>& pointsContour, QPoint &point, int &x, int &y);
    QPoint& findExternalPoint(int xMain, int yMain, QPoint &firstPoint, QPoint &secondPoint);
    bool leftNeigbourNotFromArea(const QPoint &neigbour, const QPoint &point, int imageWidth, int imageHeight);
    bool defineIsContourExternal(const int x, const int y, const int imageWidth, const int imageHeight);

public:
    ContourBuilder(ImageArea *area = nullptr);
    ContoursSet& getSetContours();
};

#endif // CONTOURBUILDER_H
