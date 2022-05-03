#ifndef RIGHTBAR_H
#define RIGHTBAR_H
#include "image.h"
#include "imagewidget.h"
#include "QCheckBox"

class RightBar: public QWidget
{
    Q_OBJECT
private:
    void createIntensity();
    void createSliderTranparency();
    void createFallibility();
    void createLine();
    void createButton();
    void createCheckBoxes();
    QLabel *transparency, *accuracy, *intensity, *valueIntensity, *fallibility;
    QSlider *valueTransparency;
    QSpinBox *valueFallibility;
    QSpinBox *valueAccuracy;
    QPushButton *colorChoice;
    QCheckBox *displayArea;
    QCheckBox *innerContours;
    QVBoxLayout *layoutRightBar;
    QHBoxLayout *layoutLabel;

public:
    RightBar(QWidget *parent = nullptr);
    void setInitialValueSlider(int a);
    void setInitialValueAccuracy(int a);
    void setInitialValueFallibility(int a);
    void setInitialStateCheckBoxDisplayArea(int state);
    void setInitialStateCheckBoxInnerContours(int state);

signals:
    void signalSliderTransparency(int a);
    void signalFallibility(int a);
    void signalColor();
    void accuracyChanged(int a);
    void checkBoxDisplayAreaStateChanged(int a);
    void checkBoxInnerContoursStateChanged(int a);

public slots:
    void setValueIntensity(int a);
};

#endif // RIGHTBAR_H
