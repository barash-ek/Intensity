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
    startDraw(0.0, 0.0),
    isContourExist(false),
    dragStartPosition(0, 0),
    draggableNode(0x0),
    isFirstNode(false),
    thicknessPenNodes(3),
    thicknessPenLines(2)
{
    setMouseTracking(true);

    setAttribute(Qt::WA_StaticContents);
    setAcceptDrops(true);
    connect(this, SIGNAL(imageLoaded()), this, SLOT(scaleImage()));
}
void ImageWidget::openImage(const QString &fileName)
{
    Image openImage(fileName);
    image = openImage;
    QSize size = image.getImage().size();
    emit signalWidget(size);
    clearScreen();
    //emit imageLoaded();
}
void ImageWidget::mousePressEvent(QMouseEvent *event)
{
    const QPoint point = transformMatrix.inverted().map(event->pos());

    int x = point.x();
    int y = point.y();

    int imageHeight = image.getImage().height();
    int imageWidth = image.getImage().width();
    if(x < imageWidth && y < imageHeight)
    {
        if (event->button() == Qt::LeftButton)
        {
            if(pointInContour(QPoint(x, y)))
            {
                dragStartPosition = point;
            }
            else
            {
                xMouse = x;
                yMouse = y;
                ImageArea clickedArea(image, QPoint(xMouse, yMouse), accuracy);
                area = clickedArea;
                ContourBuilder contourNew(&area);
                contour = contourNew.getSetContours();
                contour.buildApproximation(fallibility);
                isContourExist = true;
                areaImage = area.drawArea(color);
                update();
            }
        }
    }
}
void ImageWidget::mouseMoveEvent(QMouseEvent *event)
{
    const QPoint point = transformMatrix.inverted().map(event->pos());

    int a = image.getIntensity(point);
    if(a >= 0)
        emit mouseMoved(a);

    if(dragStartPosition != QPoint(0, 0))
    {
        QPoint oldPositionDraggablePoint = (*draggableNode);
        (*draggableNode) = point;
        if(isFirstNode)
        {
            QVector<QVector<QPoint>> *pointsApproximation;
            pointsApproximation = contour.getNodesApproximation();
            for(int i = 0; i < (*pointsApproximation).size(); ++i)
            {
                for(int j = 0; j < (*pointsApproximation)[i].size(); ++j)
                {
                    if((*pointsApproximation)[i][j] == oldPositionDraggablePoint)
                        (*pointsApproximation)[i][j] = (*draggableNode);
                }
            }
        }
        update();
    }
}
void ImageWidget::mouseReleaseEvent(QMouseEvent *event)
{
    const QPoint point = transformMatrix.inverted().map(event->pos());

    if(dragStartPosition != QPoint(0, 0))
    {
        QPoint oldPositionDraggablePoint = (*draggableNode);
        (*draggableNode) = point;
        if(isFirstNode)
        {
            QVector<QVector<QPoint>> *pointsApproximation;
            pointsApproximation = contour.getNodesApproximation();
            for(int i = 0; i < (*pointsApproximation).size(); ++i)
            {
                for(int j = 0; j < (*pointsApproximation)[i].size(); ++j)
                {
                    if((*pointsApproximation)[i][j] == oldPositionDraggablePoint)
                        (*pointsApproximation)[i][j] = (*draggableNode);
                }
            }
        }
        dragStartPosition = QPoint(0, 0);
        update();
    }
}
void ImageWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setMatrix(transformMatrix);

    //startDraw = QPointF((this->width() - image.getImage().width()) / 2, (this->height() - image.getImage().height()) / 2);
    //qDebug() << startDraw << "startDraw here";

    painter.drawImage(QPointF(0, 0), image.getImage());
    painter.drawImage(QPointF(0, 0), areaImage);

    QVector<QVector<QPoint>> *pointsApproximation;
    pointsApproximation = contour.getNodesApproximation();
    for(int k = 0; k < (*pointsApproximation).size(); ++k)
    {
        QVector<QPoint> points = (*pointsApproximation)[k];
        painter.setPen(QPen(Qt::blue, thicknessPenLines));
        for(int i = 0 ; i < (points.size() - 1); ++i)
            painter.drawLine(points[i] + startDraw, points[i + 1] + startDraw);

        painter.setPen(QPen(Qt::magenta, thicknessPenNodes));
        for(int i = 0; i < points.size(); ++i)
            painter.drawPoint(points[i] + startDraw);
    }
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

    transformMatrix.scale(zoom * scale, zoom * scale);
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
    isContourExist = false;
    isFirstNode = false;
    dragStartPosition = QPoint(0, 0);
    zoom = 1.0;
}
void ImageWidget::wheelEvent(QWheelEvent *event)
{
    static const qreal zoomMin = 0.1;
    static const qreal zoomMax = 10.0;
    const qreal zoomValue = event->angleDelta().y() > 0 ? 1.1 : 0.9;
    zoom *= zoomValue;
    if(zoom >= zoomMin && zoom <= zoomMax)
        {
        const QPointF scalePoint = transformMatrix.inverted().map(event->position());
        transformMatrix.translate(scalePoint.x(), scalePoint.y());
        transformMatrix.scale(zoomValue, zoomValue);
        transformMatrix.translate(-scalePoint.x(), -scalePoint.y());
        update();
    }

}
bool ImageWidget::pointInContour(const QPoint &point)
{
    if(isContourExist)
    {
        QVector<QVector<QPoint>> *pointsApproximation;
        pointsApproximation = contour.getNodesApproximation();
        for(int i = 0; i < (*pointsApproximation).size(); ++i)
        {
            for(int j = 0; j < (*pointsApproximation)[i].size(); ++j)
            {
                QPoint node = (*pointsApproximation)[i][j];
                if(qSqrt(qPow(point.x() - node.x(), 2) + qPow(point.y() - node.y(), 2)) <= thicknessPenNodes)
                {
                    if(j == 0)
                        isFirstNode = true;
                    draggableNode = &(*pointsApproximation)[i][j];
                    return true;
                }
            }
        }
    }
    else
        return false;
    return false;
}
