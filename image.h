#ifndef IMAGE_H
#define IMAGE_H

class Image
{
    QImage image;

public:
    Image();
    Image(const QString &fileName);
    int getIntensity(const QPoint &point);
    QImage& getImage();
};

#endif // IMAGE_H
