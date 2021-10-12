#ifndef IMAGE_H
#define IMAGE_H
#include <QWidget>
#include <QLabel>

class Image : public QWidget
{
    Q_OBJECT

public:
    Image(QWidget *parent = nullptr);
    bool openImage(const QString &fileName);
    int get() {return intensity;};
signals:
    void signal_im(QSize s);

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    void resizeImage(QImage *image, const QSize &newSize);
    void createText();
    QImage picture;
    bool open=false;
    int intensity;
    QLabel *text;
};
#endif // IMAGE_H
