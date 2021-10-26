#ifndef RIGHTBAR_H
#define RIGHTBAR_H
#include "image.h"
#include <QWidget>
#include <QLabel>
#include <QSlider>
#include <QLineEdit>
#include <QPushButton>

class rightBar: public QWidget
{
    Q_OBJECT
public:
    rightBar(QWidget *parent = nullptr);
    friend void Image::getterTransparency(rightBar &bar);
    friend void Image::getterAccuracy(rightBar &bar);
    friend void Image::getterColor(rightBar &bar);
protected:
    void resizeEvent(QResizeEvent *event) override;
private slots:
    void getTransparency(int a);
    void getAccuracy();
    void colorDialog();
private:
    QLabel *transparency, *intensity;
    QSlider *sliderTransparency;
    QLineEdit *lineIntensity;
    QPushButton *colorChoice;
    QColor colorChosen;
    QString accuracy;
    int valueTransparency;
};

#endif // RIGHTBAR_H
