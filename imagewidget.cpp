#include "imagewidget.h"
#include "contourbuilder.h"
#include "contour.h"
#include <QtMath>

ImageWidget::ImageWidget(QWidget *parent): QWidget(parent),
    xMouse(-1),
    yMouse(-1),
    transparency(255),
    color(255, 0, 0, transparency),
    accuracy(10),
    fallibility(2),
    zoom(1.0),
    scale(1.0),
    dx(0),
    dy(0),
    startDraw(0.0, 0.0),
    pointInImage(0.0, 0.0)
{
    setMouseTracking(true);
    setAttribute(Qt::WA_StaticContents);
    connect(this, SIGNAL(imageLoaded()), this, SLOT(scaleImage()));
    connect(this, SIGNAL(scaleChanged(qreal)), this, SLOT(changeScale(qreal)));
    connect(this, SIGNAL(translateChanged(qreal,qreal)), this, SLOT(changeTranslate(qreal,qreal)));
}
void ImageWidget::openImage(const QString &fileName)
{
    Image openImage(fileName);
    image = openImage;
    QSize size = image.getImage().size();
    //emit signalWidget(size);
    clearScreen();
    zoom = 1.0;
    emit imageLoaded();
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
    //painter.setTransform(transform);

    qreal width = image.getImage().width();
    qreal height = image.getImage().height();
    qreal x = pointInImage.x();
    qreal y = pointInImage.y();

    QRectF  sourceRect(x - (x / zoom), y - (y / zoom), width / zoom, height / zoom);             // переделать
    sourceRect &= image.getImage().rect();

    painter.translate(startDraw);
    painter.scale(zoom * scale, zoom * scale);
    painter.translate(-startDraw);
    painter.drawImage(startDraw, image.getImage(), sourceRect);

    /*QTransform scaleMatrix(scale, 0, 0, scale, 0, 0);
    QImage changedImage;
    changedImage = image.getImage().transformed(transform);
    painter.drawImage(QPoint(0,0), changedImage);*/
    //painter.drawImage(QPoint(0,0), image.getImage());
    /*painter.drawImage(QPoint(0,0), areaImage);

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
    }*/
}
void ImageWidget::resizeEvent(QResizeEvent *event)
{
    QSize sizeWidget = event->size();
    qreal imageWidth = image.getImage().width();
    qreal imageHeight = image.getImage().height();
    qreal xScale = (qreal)sizeWidget.width() / imageWidth;
    qreal yScale = (qreal)sizeWidget.height() / imageHeight;

    scale =  qMin<qreal>(xScale, yScale);

    qreal xOffset = 0.0, yOffset = 0.0;
    if((imageWidth * scale) < sizeWidget.width())
        xOffset = (sizeWidget.width() - (imageWidth * scale)) / 2.0;
    if((imageHeight * scale) < sizeWidget.height())
        yOffset = (sizeWidget.height() - (imageHeight * scale)) / 2.0;
    startDraw = QPointF(xOffset, yOffset);
    update();
}
void ImageWidget::scaleImage()
{
    QSize sizeWidget = this->size();
    qreal imageWidth = image.getImage().width();
    qreal imageHeight = image.getImage().height();
    qreal xScale = (qreal)sizeWidget.width() / imageWidth;
    qreal yScale = (qreal)sizeWidget.height() / imageHeight;

    scale =  qMin<qreal>(xScale, yScale);

    qreal xOffset = 0.0, yOffset = 0.0;
    if((imageWidth * scale) < sizeWidget.width())
        xOffset = (sizeWidget.width() - (imageWidth * scale)) / 2.0;
    if((imageHeight * scale) < sizeWidget.height())
        yOffset = (sizeWidget.height() - (imageHeight * scale)) / 2.0;
    startDraw = QPointF(xOffset, yOffset);
    update();
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
//    qreal zoomMin = 0.07;
//    qreal zoomMax = 8.0;
    qreal zoomLocal = 0;
//    if(event->angleDelta().y() > 0)
//        zoomLocal = 1.3;
//    else
//        zoomLocal = 0.7;
    /*if(zoomLocal >= zoomMin && zoomLocal <= zoomMax)
    {
        pointInImage = QPointF((event->position() - startDraw) / scale);
        QPointF startState = event->position();
        QPointF endState = transform.map(startState);
        dx += endState.x() - startState.x();
        dy += endState.y() - startState.y();
        transform.translate(dx, dy);
        emit scaleChanged(zoomLocal);
    }*/
    qreal degrees = event->angleDelta().y() / 8.0;
    qreal steps = degrees / 60.0;
    zoomLocal = zoom * qPow(1.125, steps);
    QPointF mousePosition = event->position();
    if(degrees > 0)
    {
        if(mouseOverImage(mousePosition))
            pointInImage = QPointF((mousePosition - startDraw) / scale);
        else
            pointInImage = QPointF(image.getImage().width() / 2, image.getImage().height() / 2);
    }
    else if(degrees < 0)
        pointInImage = QPointF((mousePosition - startDraw) / scale);
    emit scaleChanged(zoomLocal);
}
void ImageWidget::changeScale(qreal z)
{
    zoom = z;
    update();
}
void ImageWidget::changeTranslate(qreal onX, qreal onY)
{
    dx = onX;
    dy = onY;
    update();
}
