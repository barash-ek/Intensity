#include "imagewidget.h"
#include "contourbuilder.h"
#include "contour.h"
#include <QtMath>
#include <QDrag>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDropEvent>

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
    transform(zoom, 0, 0, zoom, dx, dy),
    isContourExist(false),
    dragStartPosition(0, 0),
    draggableNode(0x0),
    isFirstNode(false)
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
    int xScaled = event->pos().x() - startDraw.x();
    int yScaled = event->pos().y() - startDraw.y();
    QPointF point = transform.inverted().map(QPoint(xScaled, yScaled));
    //qDebug() << point;

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
                dragStartPosition = event->pos();
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
    if(dragStartPosition != QPoint(0, 0))
    {
        static int k = 0;
        k++;

        if (!(event->buttons() & Qt::LeftButton))
            return;
        if ((event->pos() - dragStartPosition).manhattanLength() < QApplication::startDragDistance())
            return;

        QDrag *drag = new QDrag(this);
        QMimeData *mimeData = new QMimeData;

        mimeData->setText("QPoint");
        drag->setMimeData(mimeData);

        Qt::DropAction dropAction = drag->exec(Qt::CopyAction | Qt::MoveAction);
        if(dropAction == Qt::MoveAction)
            update();
    }
    else
    {
        int xScaled = event->pos().x() - startDraw.x();
        int yScaled = event->pos().y() - startDraw.y();
        QPoint drawStart(startDraw.x(), startDraw.y());
        QPoint point = transform.inverted().map(QPoint(xScaled, yScaled));
        //qDebug() << point;

        int a = image.getIntensity(point);
        if(a >= 0)
            emit mouseMoved(a);
    }
}
void ImageWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasText())
        event->acceptProposedAction();
}
void ImageWidget::dropEvent(QDropEvent *event)
{
    QPoint oldPositionDraggablePoint = (*draggableNode);
    (*draggableNode) = event->pos();
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
    event->acceptProposedAction();
}
void ImageWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setTransform(transform);

    startDraw = QPointF((this->width() - image.getImage().width()) / 2, (this->height() - image.getImage().height()) / 2);
    //qDebug() << startDraw << "startDraw here";

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
    isContourExist = false;
    isFirstNode = false;
    dragStartPosition = QPoint(0, 0);
    zoom = 1.0;
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
                if(qSqrt(qPow(point.x() - node.x(), 2) + qPow(point.y() - node.y(), 2)) <= 3)  // magic number!! 3
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
