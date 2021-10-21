#ifndef RIGHTBAR_H
#define RIGHTBAR_H
#include <QWidget>
#include <QLabel>
#include <QSlider>
#include <QLineEdit>
#include <QPushButton>
#include <QFrame>

class rightBar: public QWidget
{
    Q_OBJECT
public:
    rightBar(QWidget *parent = nullptr);
protected:
    void resizeEvent(QResizeEvent *event) override;
private slots:
    void colorDialog();
private:
    QFrame *frameBar;
    QLabel *transparency, *intensity;
    QSlider *sliderTransparency;
    QLineEdit *lineIntensity;
    QPushButton *colorChoice;
    QColor colorChosen;
};

#endif // RIGHTBAR_H
