#include "image.h"

Image::Image()
{}
Image::Image(const QString &fileName)
{
    QImage loadedImage;
    if (loadedImage.load(fileName))
    {
        loadedImage = loadedImage.convertToFormat(QImage::Format_Grayscale8);
        image = loadedImage;
    }
}
int Image::getIntensity(const QPoint &point)
{
    const int x = point.x();
    const int y = point.y();
    unsigned char *a;
    if(!(x >= 0 && x < image.width()) || !(y >= 0 && y < image.height()))
        return -1;
    a = image.scanLine(y);
    int intens = -1;
    if(a)
        intens = int(a[x]);
    else
        qDebug() <<"a is a null pointer";
    return intens;
}
QImage& Image::getImage()
{
    return image;
}
