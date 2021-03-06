#include "imagewidget.h"
#include "contourbuilder.h"
#include "contour.h"

ImageWidget::ImageWidget(QWidget *parent): QWidget(parent),
    xMouse(-1),
    yMouse(-1),
    transparency(255),
    color(255, 0, 0, transparency),
    accuracy(10),
    fallibility(2),
    stateArea(false),
    stateInnerContours(true),
    zoom(1.0),
    isContourExist(false),
    dragStartPosition(0, 0),
    draggableNode(0x0),
    thicknessPenNodes(3),
    thicknessPenLines(2)
{
    setMouseTracking(true);

    setAttribute(Qt::WA_StaticContents);
    connect(this, &ImageWidget::imageLoaded, this, &ImageWidget::scaleImage);
}
bool ImageWidget::openImage(const QString &fileName)
{
    clearScreen();
    image = Image(fileName);
    if(image.getImage().isNull())
        return false;
    else
    {
        emit imageLoaded();   // чтобы масштабировать изображение под размеры окна
        return true;
    }
}
void ImageWidget::mousePressEvent(QMouseEvent *event)
{
    const QPointF point = transformMatrix.inverted().map(event->pos());

    const int x = point.x();
    const int y = point.y();

    const int imageHeight = image.getImage().height();
    const int imageWidth = image.getImage().width();
    if((0 <= x && x < imageWidth) && (0 <= y && y < imageHeight))
    {
        if (event->button() == Qt::LeftButton)
        {
            if(isPointNode(QPoint(x, y)))
            {
                dragStartPosition = point;
            }
            else
            {
                xMouse = x;
                yMouse = y;
                area = ImageArea(image, QPoint(xMouse, yMouse), accuracy);
                ContourBuilder contourNew(&area);
                contours = contourNew.getSetContours();
                contours.buildApproximation(fallibility);
                isContourExist = true;
                areaImage = area.drawArea(color);
                update();
            }
        }
    }
}
void ImageWidget::mouseMoveEvent(QMouseEvent *event)
{
    const QPointF cursorPositionf = transformMatrix.inverted().map(event->pos());
    const QPoint cursorPosition(cursorPositionf.x(), cursorPositionf.y());

    int intensity = image.getIntensity(cursorPosition);
    QString valueIntensity;
    if(intensity >= 0)
       valueIntensity = QString::number(intensity);
    else
        valueIntensity = "";
    emit mouseMoved(valueIntensity);

    if(dragStartPosition != QPointF(0, 0))
    {
        (*draggableNode) = cursorPosition;
        update();
    }
}
void ImageWidget::mouseReleaseEvent(QMouseEvent *event)
{
    const QPointF cursorPositionf = transformMatrix.inverted().map(event->pos());
    const QPoint cursorPosition(cursorPositionf.x(), cursorPositionf.y());

    if(dragStartPosition != QPointF(0, 0))
    {
        (*draggableNode) = cursorPosition;
        dragStartPosition = QPointF(0, 0);
        update();
    }
}
void ImageWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setMatrix(transformMatrix);

    painter.drawImage(QPointF(0, 0), image.getImage());
    if(stateArea)
        painter.drawImage(QPointF(0, 0), areaImage);

    QVector<QVector<QPoint>> *pointsApproximation = Q_NULLPTR;
    QVector<QVector<QPoint>> vectorAuxiliary;
    if(stateInnerContours)
        pointsApproximation = contours.getNodesApproximation();
    else
    {
        QVector<QVector<QPoint>> *nodesApproximation = contours.getNodesApproximation();
        QVector<Contour> setContours = contours.getSetContours();
        for(int i = 0, s = setContours.size(); i < s; ++i)
        {
            if(setContours[i].getStateContour())
                vectorAuxiliary << nodesApproximation->at(i);
        }
        pointsApproximation = &vectorAuxiliary;
    }

    int countNodes = 0;
    for(int k = 0, t = pointsApproximation->size(); k < t; ++k)
    {
        const QVector<QPoint> &points = pointsApproximation->at(k);
        if(points.size() > 2)
        {
            countNodes += points.size();
            emit countNodesChanged(QString::number(countNodes));
            painter.setPen(QPen(Qt::red, thicknessPenLines));
            for(int i = 0, p = points.size() - 1; i < p; ++i)
                painter.drawLine(points[i], points[i + 1]);
            painter.drawLine(points[points.size() - 1], points[0]);    // для соединения с первой точкой контура

            painter.setPen(QPen(Qt::red, thicknessPenNodes));
            for(int i = 0, p = points.size(); i < p; ++i)
                painter.drawEllipse(points[i], thicknessPenNodes / 2, thicknessPenNodes / 2);
        }
    }
}
void ImageWidget::scaleImage()
{
    transformMatrix.reset();       // чтобы можно было другое изображение загрузить без накопившегося машстабирования
    const QSize sizeWidget = this->size();
    const qreal imageWidth = image.getImage().width();
    const qreal imageHeight = image.getImage().height();
    const qreal xScale = (qreal)sizeWidget.width() / imageWidth;
    const qreal yScale = (qreal)sizeWidget.height() / imageHeight;

    zoom *=  qMin<qreal>(xScale, yScale);
    transformMatrix.scale(zoom, zoom);
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
        area = ImageArea(image, QPoint(xMouse, yMouse), accuracy);
        ContourBuilder contourNew(&area);
        contours = contourNew.getSetContours();
        contours.buildApproximation(fallibility);
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
    accuracy = a;
    if(xMouse >= 0 && yMouse >= 0)
    {
        area = ImageArea(image, QPoint(xMouse, yMouse), accuracy);
        ContourBuilder contourNew(&area);
        contours = contourNew.getSetContours();
        contours.buildApproximation(fallibility);
        areaImage = area.drawArea(color);
        update();
    }
}

void ImageWidget::displayArea(int state)
{
    stateArea = state;
    update();
}

void ImageWidget::displayInnerContours(int state)
{
    stateInnerContours = state;
    update();
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

bool ImageWidget::getStateArea()
{
    return stateArea;
}

bool ImageWidget::getStateInnerContours()
{
    return stateInnerContours;
}
void ImageWidget::clearScreen()
{
    xMouse = -1;
    yMouse = -1;
    areaImage = QImage();
    contours = ContoursSet();
    isContourExist = false;
    dragStartPosition = QPointF(0, 0);
    zoom = 1.0;
    emit mouseMoved("");
    emit countNodesChanged("");
}
void ImageWidget::wheelEvent(QWheelEvent *event)
{
    static const qreal zoomMin = 0.1;
    static const qreal zoomMax = 20.0;
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
bool ImageWidget::isPointNode(const QPoint &cursorPosition)
{
    if(!isContourExist)
        return false;

    QVector<QVector<QPoint>> *pointsApproximation;
    pointsApproximation = contours.getNodesApproximation();
    for(int i = 0, t = pointsApproximation->size(); i < t; ++i)
    {
        QVector<QPoint> &rowPoints = (*pointsApproximation)[i];
        for(int j = 0, p = rowPoints.size(); j < p; ++j)
        {
            QPoint node = rowPoints[j];
            if(qSqrt(qPow(cursorPosition.x() - node.x(), 2) + qPow(cursorPosition.y() - node.y(), 2)) <= qreal(thicknessPenNodes) / 2)
            {
                draggableNode = &rowPoints[j];
                return true;
            }
        }
    }

    return false;
}
