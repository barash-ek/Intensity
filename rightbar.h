#ifndef RIGHTBAR_H
#define RIGHTBAR_H
#include "image.h"
#include "imagewidget.h"

class RightBar: public QWidget
{
    Q_OBJECT

public:
    RightBar(QWidget *parent = nullptr);
    void setInitialValueSlider(int a);
    void setInitialValueLine(int a);

signals:
    void signalSlider(int a);
    void signalColor();
    void accuracyChanged(int a);

public slots:
    void setValueIntensity(int a);
    void setEnabledSpinBox(bool condition);

private:
    void createIntensity();
    void createSlider();
    void createLine();
    void createButton();
    QLabel *transparency, *accuracy, *intensity, *valueIntensity;
    QSlider *sliderTransparency;
    QSpinBox *lineIntensity;
    QPushButton *colorChoice;
    QVBoxLayout *layoutRightBar;
    QHBoxLayout *layoutLabel;
    QHBoxLayout *layoutLine;

//friend void ImageWidget::userAccuracy(RightBar &A);
};

#endif // RIGHTBAR_H
