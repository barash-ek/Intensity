#ifndef WIDGET_H
#define WIDGET_H
#include "image.h"
#include "imagearea.h"
#include "contour.h"

class ImageWidget: public QWidget
{
    Q_OBJECT

public:
    ImageWidget(QWidget *parent = nullptr);
    void openImage(const QString &fileName);
    int getTransparency();
    int getAccuracy();
    int getFallibility();
signals:
    void mouseMoved(int a);
    void signalWidget(QSize s);
    void signalSetEnabled(bool condition);

public slots:
    void userTransparency(int a);
    void userFallibility(int a);
    void userColor();
    void userAccuracy(int a);

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    Image image;
    ImageArea area;
    Contour contour;
    QImage areaImage;
    int xMouse, yMouse;
    int transparency;
    QColor color;
    int accuracy;
    int fallibility;
};

#endif // WIDGET_H
