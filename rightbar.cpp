#include "rightbar.h"

RightBar::RightBar(QWidget *parent) : QWidget(parent), transparency(0x0), accuracy(0x0), intensity(0x0),
    valueIntensity(0x0), sliderTransparency(0x0), lineIntensity(0x0), colorChoice(0x0),
    layoutRightBar(0x0), layoutLabel(0x0), layoutLine(0x0)
{
    setAttribute(Qt::WA_StaticContents);

    createIntensity();
    createSlider();
    createLine();
    createButton();

    layoutRightBar = new QVBoxLayout;
    layoutRightBar->addLayout(layoutLabel);
    layoutRightBar->addWidget(transparency);
    layoutRightBar->addWidget(sliderTransparency);
    layoutRightBar->addLayout(layoutLine);
    layoutRightBar->addWidget(colorChoice);
    layoutRightBar->addStretch();
    setLayout(layoutRightBar);
}
void RightBar::createIntensity()
{
    intensity = new QLabel("Интенсивность:");
    intensity->setAlignment(Qt::AlignLeft);
    valueIntensity = new QLabel;
    valueIntensity->setFixedWidth(20);

    layoutLabel = new QHBoxLayout;
    layoutLabel->addWidget(intensity);
    layoutLabel->addWidget(valueIntensity);
}
void RightBar::createSlider()
{
    transparency = new QLabel("Прозрачность:");
    transparency->setAlignment(Qt::AlignLeft);
    sliderTransparency=new QSlider(Qt::Horizontal);
    sliderTransparency->setStyleSheet("QSlider::groove:horizontal {"
                        "border: 1px solid #999999;"
                        "height: 4px;"
                        "width: 90px;"
                        "background: palette(midlight);"
                        "margin: 8px 0;"
                    "}"
                        "QSlider::handle:horizontal {"
                            "background: palette(window);"
                            "border: 1px solid #999999;"
                            "width: 30px;"
                            "margin: -8px 0;"
                            "border-radius: 3px;"
                    "}");
    sliderTransparency->setRange(0, 255);
    sliderTransparency->setSingleStep(1);
    connect(sliderTransparency, SIGNAL(sliderMoved(int)), this, SIGNAL(signalSlider(int)));
}
void RightBar::createLine()
{
    accuracy = new QLabel("Точность:");
    lineIntensity = new QSpinBox;
    lineIntensity->setMinimum(0);
    lineIntensity->setMaximum(255);

    layoutLine = new QHBoxLayout;
    layoutLine->addWidget(accuracy);
    layoutLine->addWidget(lineIntensity);

    connect(lineIntensity, SIGNAL(valueChanged(int)), this, SIGNAL(accuracyChanged(int)));
}
void RightBar::createButton()
{
    colorChoice = new QPushButton("Выбор цвета...");
    connect(colorChoice, SIGNAL(clicked()), this, SIGNAL(signalColor()));
}
void RightBar::setValueIntensity(int a)
{
    valueIntensity->setText(QString::number(a));
}
void RightBar::setInitialValueSlider(int a)
{
    sliderTransparency->setValue(a);
}
void RightBar::setInitialValueLine(int a)
{
    lineIntensity->setValue(a);
}
void RightBar::setEnabledSpinBox(bool condition)
{
    lineIntensity->setEnabled(condition);
}
