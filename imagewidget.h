#ifndef WIDGET_H
#define WIDGET_H
#include "image.h"
#include "imagearea.h"
#include "contoursset.h"

class ImageWidget: public QWidget
{
    Q_OBJECT

private:
    Image image;
    ImageArea area;
    ContoursSet contours;
    QImage areaImage;
    int xMouse, yMouse;
    int transparency;
    QColor color;
    int accuracy;
    int fallibility;
    bool stateArea;
    bool stateInnerContours;
    qreal zoom;
    QMatrix transformMatrix;

    bool isContourExist;
    QPointF dragStartPosition;
    QPoint *draggableNode;

    const int thicknessPenNodes;
    const int thicknessPenLines;

    void clearScreen();
    bool isPointNode(const QPoint &cursorPosition);

public:
    ImageWidget(QWidget *parent = Q_NULLPTR);
    bool openImage(const QString &fileName);
    int getTransparency();
    int getAccuracy();
    int getFallibility();
    bool getStateArea();
    bool getStateInnerContours();

signals:
    void mouseMoved(QString a);
    void countNodesChanged(QString a);
    void imageLoaded();

public slots:
    void scaleImage();
    void userTransparency(int a);
    void userFallibility(int a);
    void userColor();
    void userAccuracy(int a);
    void displayArea(int state);
    void displayInnerContours(int state);

protected:
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
};

#endif // WIDGET_H
