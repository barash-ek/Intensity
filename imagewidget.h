#ifndef WIDGET_H
#define WIDGET_H
#include "image.h"
#include "imagearea.h"
#include "contoursset.h"
#include <QTransform>

class ImageWidget: public QWidget
{
    Q_OBJECT
private:
    Image image;
    ImageArea area;
    ContoursSet contour;
    QImage areaImage, contourImage;
    int xMouse, yMouse;
    int transparency;
    QColor color;
    int accuracy;
    int fallibility;
    qreal zoom;
    qreal scale;
    qreal dx, dy;
    QPointF startDraw;
    QTransform transform;
    void clearScreen();

public:
    ImageWidget(QWidget *parent = nullptr);
    void openImage(const QString &fileName);
    int getTransparency();
    int getAccuracy();
    int getFallibility();

signals:
    void mouseMoved(int a);
    void signalWidget(QSize s);
    void imageLoaded();
    void signalSetEnabled(bool condition);

public slots:
    void scaleImage();
    void userTransparency(int a);
    void userFallibility(int a);
    void userColor();
    void userAccuracy(int a);

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    //void resizeEvent(QResizeEvent *event) override;
};

#endif // WIDGET_H
