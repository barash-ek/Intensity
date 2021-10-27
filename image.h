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

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
   // void drawPoint(const QPoint &pressPoint);
    void drawArea(const QPoint &pressPoint);
    void createText();
    int valueIntensity(const QPoint &pointIntensity);
    QImage picture;
    QImage pictureArea;
    QLabel *text;
    int transparency;
    QString accuracy;
    QColor color;
};
#endif // IMAGE_H
