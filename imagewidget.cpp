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
    transform(zoom, 0, 0, zoom, dx, dy)
{
    setMouseTracking(true);
    setAttribute(Qt::WA_StaticContents);
    connect(this, SIGNAL(imageLoaded()), this, SLOT(scaleImage()));
}
void ImageWidget::openImage(const QString &fileName)
{
    Image openImage(fileName);
    image = openImage;
    QSize size = image.getImage().size();
    emit signalWidget(size);
    clearScreen();
    zoom = 1.0;
    //emit imageLoaded();
}
void ImageWidget::mouseMoveEvent(QMouseEvent *event)
{
    int xScaled = event->pos().x() - startDraw.x();
    int yScaled = event->pos().y() - startDraw.y();
    QPoint drawStart(startDraw.x(), startDraw.y());
    QPoint point = transform.inverted().map(QPoint(xScaled, yScaled));
    qDebug() << point;

    int a = image.getIntensity(point);
    if(a >= 0)
        emit mouseMoved(a);
}
void ImageWidget::mousePressEvent(QMouseEvent *event)
{
    int xScaled = event->pos().x() - startDraw.x();
    int yScaled = event->pos().y() - startDraw.y();
    QPointF point = transform.inverted().map(QPoint(xScaled, yScaled));
    qDebug() << point;

    int x = point.x();
    int y = point.y();

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

    startDraw = QPointF((this->width() - image.getImage().width()) / 2, (this->height() - image.getImage().height()) / 2);
    qDebug() << startDraw << "startDraw here";
    painter.drawImage(startDraw, image.getImage());
    painter.drawImage(startDraw, areaImage);

    QVector<QVector<QPoint>> *pointsApproximation;
    pointsApproximation = contour.getNodesApproximation();
    for(int k = 0; k < (*pointsApproximation).size(); ++k)
    {
        QVector<QPoint> points = (*pointsApproximation)[k];
        painter.setPen(QPen(Qt::blue, 2));
        for(int i = 0 ; i < (points.size() - 1); ++i)
            //painter.drawLine(scaleMatrix.map(points[i]), scaleMatrix.map(points[i + 1]));
            painter.drawLine(points[i] + startDraw, points[i + 1] + startDraw);
        painter.setPen(QPen(Qt::magenta, 3));
        for(int i = 0; i < points.size(); ++i)
            painter.drawPoint(points[i] + startDraw);
            //painter.drawPoint(scaleMatrix.map(points[i]));
    }
    painter.resetTransform();
}
/*void ImageWidget::resizeEvent(QResizeEvent *event)
{
    QSize sizeWidget = event->size();
    qreal imageWidth = image.getImage().width();
    qreal imageHeight = image.getImage().height();

    qreal xOffset = 0.0, yOffset = 0.0;
    if((imageWidth * scale) < sizeWidget.width())
        xOffset = (sizeWidget.width() - (imageWidth * scale)) / 2.0;
    if((imageHeight * scale) < sizeWidget.height())
        yOffset = (sizeWidget.height() - (imageHeight * scale)) / 2.0;
    startDraw = QPointF(xOffset, yOffset);

    update();
}*/
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

    transform.scale(zoom * scale, zoom * scale);
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
/*void ImageWidget::wheelEvent(QWheelEvent *event)
{
    qreal zoomLocal = 0;
    zoomLocal = event->angleDelta().y() > 0 ? 1.1 : 0.9;

    QPointF startState = event->position();
    zoom = zoomLocal;
    transform.scale(zoom, zoom);
    QPointF endState = transform.map(startState);

    //QPointF endState = QPointF(startState.x() * zoom, startState.y() * zoom);
    dx =  startState.x() - endState.x();
    dy =  startState.y() - endState.y();
    qDebug() << dx << dy;
    transform.translate(dx, dy);

    update();
}*/
void ImageWidget::wheelEvent(QWheelEvent *event)
{
    qreal zoomMin = 0.1;
    qreal zoomMax = 10.0;
    qreal zoomLocal = zoom;
    zoomLocal += event->angleDelta().y() > 0 ? 0.2 : -0.2;
    if(zoomLocal >= zoomMin && zoomLocal <= zoomMax)
    {
        QPointF startState = event->position();
        zoom = zoomLocal;
        QTransform scaleMatrix(zoom, 0, 0, zoom, 0, 0);
        transform = scaleMatrix;
        QPointF endState = transform.map(startState);

        dx =  startState.x() - endState.x();
        dy =  startState.y() - endState.y();
        QTransform commonMatrix(zoom, 0, 0, zoom, dx, dy);
        transform = commonMatrix;
        update();
    }
}
