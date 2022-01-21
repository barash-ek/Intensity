#include "imagewidget.h"
#include "contourbuilder.h"
#include "contour.h"

ImageWidget::ImageWidget(QWidget *parent): QWidget(parent), xMouse(-1), yMouse(-1), transparency(255), color(255, 0, 0, transparency), accuracy(10), fallibility(2), scale(1.0)
{
    setMouseTracking(true);
    setAttribute(Qt::WA_StaticContents);
    connect(this, SIGNAL(scaleChanged(qreal)), this, SLOT(changeScale(qreal)));
}
void ImageWidget::openImage(const QString &fileName)
{
    Image openImage(fileName);
    image = openImage;
    QSize size = image.getImage().size();
    emit signalWidget(size);
    clearScreen();
    scale = 1.0;
}
void ImageWidget::mouseMoveEvent(QMouseEvent *event)
{
    int x = event->pos().x();
    int y = event->pos().y();
    QPoint scalePoint = transform.inverted().map(QPoint(x, y));

    int a = image.getIntensity(scalePoint);
    if(a >= 0)
        emit mouseMoved(a);
}
void ImageWidget::mousePressEvent(QMouseEvent *event)
{
    int x = event->x();
    int y = event->y();
    QPoint scalePoint = transform.inverted().map(QPoint(x, y));
    x = scalePoint.x();
    y = scalePoint.y();
    int imageHeight = image.getImage().height();
    int imageWidth = image.getImage().width();
    if(x < imageWidth && y < imageHeight)
    {
        if (event->button() == Qt::LeftButton)
        {
            xMouse = x;
            yMouse = y;
            ImageArea clickedArea(image, QPoint(xMouse, yMouse), accuracy);
            area = clickedArea;
            ContourBuilder contourNew(&area);
            contour = contourNew.getSetContours();
            contour.buildApproximation(fallibility);
            areaImage = area.drawArea(color);
            update();
        }
    }
}
void ImageWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setTransform(transform);

    /*QTransform scaleMatrix(scale, 0, 0, scale, 0, 0);
    QImage changedImage;
    changedImage = image.getImage().transformed(transform);
    painter.drawImage(QPoint(0,0), changedImage);*/
    painter.drawImage(QPoint(0,0), image.getImage());
    painter.drawImage(QPoint(0,0), areaImage);

    QVector<QVector<QPoint>> *pointsApproximation;
    pointsApproximation = contour.getNodesApproximation();
    for(int k = 0; k < (*pointsApproximation).size(); ++k)
    {
        QVector<QPoint> points = (*pointsApproximation)[k];
        painter.setPen(QPen(Qt::blue, 2));
        for(int i = 0 ; i < (points.size() - 1); ++i)
            //painter.drawLine(scaleMatrix.map(points[i]), scaleMatrix.map(points[i + 1]));
            painter.drawLine(points[i], points[i + 1]);
        painter.setPen(QPen(Qt::magenta, 3));
        for(int i = 0; i < points.size(); ++i)
            painter.drawPoint(points[i]);
            //painter.drawPoint(scaleMatrix.map(points[i]));
    }
}
void ImageWidget::userTransparency(int a)
{
   transparency = a;
   color.setAlpha(transparency);
   areaImage = area.drawArea(color);
   update();
}
void ImageWidget::userFallibility(int a)
{
    fallibility = a;
    if(xMouse >= 0 && yMouse >= 0)
    {
        ImageArea clickedArea(image, QPoint(xMouse, yMouse), accuracy);
        area = clickedArea;
        ContourBuilder contourNew(&area);
        contour = contourNew.getSetContours();
        contour.buildApproximation(fallibility);
        areaImage = area.drawArea(color);
        update();
    }
}
void ImageWidget::userColor()
{
    QColor colorChosen = QColorDialog::getColor(color);
    if(colorChosen.isValid())
    {
        colorChosen.setAlpha(transparency);
        color = colorChosen;
        areaImage = area.drawArea(color);
    }
    update();
}
void ImageWidget::userAccuracy(int a)
{
    emit signalSetEnabled(false);
    accuracy = a;
    if(xMouse >= 0 && yMouse >= 0)
    {
        ImageArea chosenArea(image, QPoint(xMouse, yMouse), accuracy);
        area = chosenArea;
        ContourBuilder contourNew(&area);
        contour = contourNew.getSetContours();
        contour.buildApproximation(fallibility);
        areaImage = area.drawArea(color);
        update();
    }
    emit signalSetEnabled(true);
}
int ImageWidget::getTransparency()
{
    return transparency;
}
int ImageWidget::getAccuracy()
{
    return accuracy;
}
int ImageWidget::getFallibility()
{
    return fallibility;
}
void ImageWidget::clearScreen()
{
    xMouse = -1;
    yMouse = -1;
    QImage newArea;
    areaImage = newArea;
    ContoursSet newContour;
    contour = newContour;
}
void ImageWidget::wheelEvent(QWheelEvent *event)
{
    qreal scaleMin = 0.07;
    qreal scaleMax = 8.0;
    qreal scaleLocal = 0;
    /*qreal scaleLocal = scale;
    if(event->angleDelta().y() > 0)
        scaleLocal += 0.07;
    else
        scaleLocal -= 0.07;*/
    if(event->angleDelta().y() > 0)
        scaleLocal = 1.1;
    else
        scaleLocal = 0.9;
    if(scaleLocal >= scaleMin && scaleLocal <= scaleMax)
    {
        scale = scaleLocal;
        emit scaleChanged(scale);
    }
    transform.scale(scale, scale);
}
void ImageWidget::changeScale(qreal s)
{
    scale = s;
    update();
}
