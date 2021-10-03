#ifndef IMAGE_H
#define IMAGE_H
#include <QWidget>

class Image : public QWidget
{
    Q_OBJECT

public:
    Image(QWidget *parent = nullptr);

    bool openImage(const QString &fileName);
    QString get() {return intensity;};

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void resizeImage(QImage *image, const QSize &newSize);
    QImage picture;
    //QPoint lastPoint;
    int x, y;
    QString intensity;
};
#endif // IMAGE_H
