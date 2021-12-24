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
    void setInitialValueAccuracy(int a);
    void setInitialValueFallibility(int a);

signals:
    void signalSliderTransparency(int a);
    void signalFallibility(int a);
    void signalColor();
    void accuracyChanged(int a);

public slots:
    void setValueIntensity(int a);
    void setEnabledSpinBox(bool condition);

private:
    void createIntensity();
    void createSliderTranparency();
    void createFallibility();
    void createLine();
    void createButton();
    QLabel *transparency, *accuracy, *intensity, *valueIntensity, *fallibility;
    QSlider *valueTransparency;
    QSpinBox *valueFallibility;
    QSpinBox *valueAccuracy;
    QPushButton *colorChoice;
    QVBoxLayout *layoutRightBar;
    QHBoxLayout *layoutLabel;

//friend void ImageWidget::userAccuracy(RightBar &A);
};

#endif // RIGHTBAR_H
