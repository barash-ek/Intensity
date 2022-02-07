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
    zoom(1.0),
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
    image = Image(fileName);
    //QSize size = image.getImage().size();
    //emit signalWidget(size);
    clearScreen();
    emit imageLoaded();
}
void ImageWidget::mousePressEvent(QMouseEvent *event)
{
    const QPointF point = transformMatrix.inverted().map(event->pos());

    const int x = point.x();
    const int y = point.y();

    const int imageHeight = image.getImage().height();
    const int imageWidth = image.getImage().width();
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
                area = ImageArea(image, QPoint(xMouse, yMouse), accuracy);
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
    const QPointF cursorPositionf = transformMatrix.inverted().map(event->pos());
    const QPoint cursorPosition(cursorPositionf.x(), cursorPositionf.y());

    int a = image.getIntensity(cursorPosition);
    if(a >= 0)
        emit mouseMoved(a);

    if(dragStartPosition != QPointF(0, 0))
    {
        editingNodes(cursorPosition);
        update();
    }
}
void ImageWidget::mouseReleaseEvent(QMouseEvent *event)
{
    const QPointF cursorPositionf = transformMatrix.inverted().map(event->pos());
    const QPoint cursorPosition(cursorPositionf.x(), cursorPositionf.y());

    if(dragStartPosition != QPointF(0, 0))
    {
        editingNodes(cursorPosition);
        dragStartPosition = QPointF(0, 0);
        update();
    }
}
void ImageWidget::editingNodes(const QPoint &cursorPosition)
{
    const QPoint oldPositionDraggablePoint = (*draggableNode);
    (*draggableNode) = cursorPosition;
    if(isFirstNode)
        editingFirstnode(oldPositionDraggablePoint);
}
void ImageWidget::editingFirstnode(const QPoint &oldPositionDraggablePoint)
{
    QVector<QVector<QPoint>> *pointsApproximation;           // qFind() may will be used
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
void ImageWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setMatrix(transformMatrix);

    //startDraw = QPointF((this->width() - image.getImage().width()) / 2, (this->height() - image.getImage().height()) / 2);

    painter.drawImage(startDraw, image.getImage());
    painter.drawImage(startDraw, areaImage);

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
void ImageWidget::scaleImage()
{
    transformMatrix.reset();
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
        area = ImageArea(image, QPoint(xMouse, yMouse), accuracy);
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
    areaImage = QImage();
    contour = ContoursSet();
    isContourExist = false;
    isFirstNode = false;
    dragStartPosition = QPointF(0, 0);
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
bool ImageWidget::pointInContour(const QPoint &cursorPosition)
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
                //if(qSqrt(qPow(cursorPosition.x() - node.x(), 2) + qPow(cursorPosition.y() - node.y(), 2)) <= thicknessPenNodes)
                if((cursorPosition - node).manhattanLength() < thicknessPenNodes)
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
