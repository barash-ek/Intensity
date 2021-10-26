#ifndef IMAGE_H
#define IMAGE_H
#include <QWidget>
#include <QLabel>
class rightBar;

class Image : public QWidget
{
    Q_OBJECT

public:
    Image(QWidget *parent = nullptr);
    bool openImage(const QString &fileName);
    void getterTransparency(rightBar &bar);
    void getterAccuracy(rightBar &bar);
    void getterColor(rightBar &bar);
signals:
    void signal_im(QSize s);
public slots:
    void clearImage();
protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
   // void drawPoint(const QPoint &pressPoint);
    void drawArea(const QPoint &pressPoint);
    void resizeImage(QImage *image, const QSize &newSize);
    void createText();
    int valueIntensity(const QPoint &pointIntensity);
    QImage picture;
    QImage convertedPicture;
    QImage pictureArea;
    QSize sizePicture;
    int xPicture, yPicture;
    bool open=false;
    QLabel *text;
    QRect *ourRect;
    int transparency;
    QString accuracy;
    QColor color;
};
#endif // IMAGE_H
