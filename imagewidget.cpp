#include "imagewidget.h"

ImageWidget::ImageWidget(QWidget *parent): QWidget(parent), xMouse(-1), yMouse(-1), transparency(255), color(255, 0, 0, transparency), accuracy(10)
{
    setMouseTracking(true);
    setAttribute(Qt::WA_StaticContents);
}
void ImageWidget::openImage(const QString &fileName)
{
    Image openImage(fileName);
    image = openImage;
    QSize size = image.getImage().size();
    emit signalWidget(size);
    if(!areaImage.isNull())
    {
        QImage newArea;
        areaImage = newArea;
    }
}
void ImageWidget::mouseMoveEvent(QMouseEvent *event)
{
    int a=image.getIntensity(event->pos());
    if(a>=0)
    {
        emit mouseMoved(a);
    }
}
void ImageWidget::mousePressEvent(QMouseEvent *event)
{
    int x = event->x();
    int y = event->y();
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
            areaImage = area.drawArea(color);
            update();
        }
    }
}
void ImageWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawImage(QPoint(0,0), image.getImage());
    painter.drawImage(QPoint(0,0), areaImage);
}
void ImageWidget::userTransparency(int a)
{
   transparency = a;
   color.setAlpha(transparency);
   areaImage = area.drawArea(color);
   update();
}
void ImageWidget::userColor()
{
    QColor colorChosen = QColorDialog::getColor(color);
    if(colorChosen.isValid())
    {
        colorChosen.setAlpha(transparency);
        color = colorChosen;
        if(!areaImage.isNull())
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
        if(!areaImage.isNull())
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
