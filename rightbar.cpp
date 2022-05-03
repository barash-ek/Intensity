#include "rightbar.h"

RightBar::RightBar(QWidget *parent) : QWidget(parent),
    transparency(0x0),
    accuracy(0x0),
    intensity(0x0),
    valueIntensity(0x0),
    fallibility(0x0),
    valueTransparency(0x0),
    valueFallibility(0x0),
    valueAccuracy(0x0),
    colorChoice(0x0),
    displayArea(Q_NULLPTR),
    innerContours(Q_NULLPTR),
    layoutRightBar(0x0),
    layoutLabel(0x0)
{
    setAttribute(Qt::WA_StaticContents);

    createIntensity();
    createSliderTranparency();
    createFallibility();
    createLine();
    createButton();
    createCheckBoxes();

    layoutRightBar = new QVBoxLayout;
    layoutRightBar->addLayout(layoutLabel);
    layoutRightBar->addWidget(transparency);
    layoutRightBar->addWidget(valueTransparency);
    layoutRightBar->addWidget(fallibility);
    layoutRightBar->addWidget(valueFallibility);
    layoutRightBar->addWidget(accuracy);
    layoutRightBar->addWidget(valueAccuracy);
    layoutRightBar->addWidget(colorChoice);
    layoutRightBar->addWidget(displayArea);
    layoutRightBar->addWidget(innerContours);
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
void RightBar::createSliderTranparency()
{
    transparency = new QLabel("Прозрачность:");
    transparency->setAlignment(Qt::AlignLeft);
    valueTransparency = new QSlider(Qt::Horizontal);
    valueTransparency->setStyleSheet("QSlider::groove:horizontal {"
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
    valueTransparency->setRange(0, 255);
    valueTransparency->setSingleStep(1);
    connect(valueTransparency, SIGNAL(sliderMoved(int)), this, SIGNAL(signalSliderTransparency(int)));
}
void RightBar::createFallibility()
{
    fallibility = new QLabel("Погрешность (для контура):");
    fallibility->setAlignment(Qt::AlignLeft);
    valueFallibility = new QSpinBox;
    valueFallibility->setFixedWidth(50);
    valueFallibility->setMinimum(0);
    valueFallibility->setMaximum(1000);

    connect(valueFallibility, SIGNAL(valueChanged(int)), this, SIGNAL(signalFallibility(int)));
}
void RightBar::createLine()
{
    accuracy = new QLabel("Точность (для области):");
    valueAccuracy = new QSpinBox;
    valueAccuracy->setFixedWidth(50);
    valueAccuracy->setMinimum(0);
    valueAccuracy->setMaximum(255);

    connect(valueAccuracy, SIGNAL(valueChanged(int)), this, SIGNAL(accuracyChanged(int)));
}
void RightBar::createButton()
{
    colorChoice = new QPushButton("Выбор цвета...");
    connect(colorChoice, SIGNAL(clicked()), this, SIGNAL(signalColor()));
}

void RightBar::createCheckBoxes()
{
    displayArea = new QCheckBox("Отображать область");
    innerContours = new QCheckBox("Отображать внутренние контуры");
    connect(displayArea, &QCheckBox::stateChanged, this, &RightBar::checkBoxDisplayAreaStateChanged);
    connect(innerContours, &QCheckBox::stateChanged, this, &RightBar::checkBoxInnerContoursStateChanged);
}
void RightBar::setValueIntensity(int a)
{
    valueIntensity->setText(QString::number(a));
}
void RightBar::setInitialValueSlider(int a)
{
    valueTransparency->setValue(a);
}
void RightBar::setInitialValueAccuracy(int a)
{
    valueAccuracy->setValue(a);
}
void RightBar::setInitialValueFallibility(int a)
{
    valueFallibility->setValue(a);
}

void RightBar::setInitialStateCheckBoxDisplayArea(int state)
{
    displayArea->setChecked(state);
}

void RightBar::setInitialStateCheckBoxInnerContours(int state)
{
    innerContours->setChecked(state);
}
