#ifndef IMAGE_H
#define IMAGE_H
#include <QWidget>

class Image : public QWidget
{
    Q_OBJECT

public:
    Image(QWidget *parent = nullptr);

    bool openImage(const QString &fileName);
    int get() {return intensity;};
signals:
 void clicked(QMouseEvent *); //объект класса будет испускать этот сигнал

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void resizeImage(QImage *image, const QSize &newSize);
    QImage picture;
    //QPoint lastPoint;
    int x, y;
    int intensity;
};
#endif // IMAGE_H
