#include "rightbar.h"

RightBar::RightBar(QWidget *parent) : QWidget(parent),
    transparency(Q_NULLPTR),
    accuracy(Q_NULLPTR),
    intensity(Q_NULLPTR),
    nodes(Q_NULLPTR),
    valueIntensity(Q_NULLPTR),
    fallibility(Q_NULLPTR),
    countNodes(Q_NULLPTR),
    valueTransparency(Q_NULLPTR),
    valueFallibility(Q_NULLPTR),
    valueAccuracy(Q_NULLPTR),
    colorChoice(Q_NULLPTR),
    displayArea(Q_NULLPTR),
    innerContours(Q_NULLPTR),
    layoutRightBar(Q_NULLPTR),
    layoutLabel(Q_NULLPTR),
    layoutNodes(Q_NULLPTR)
{
    setAttribute(Qt::WA_StaticContents);

    createIntensity();
    createSliderTranparency();
    createFallibility();
    createLine();
    createButton();
    createCheckBoxes();
    createCountNodes();

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
    layoutRightBar->addLayout(layoutNodes);
    layoutRightBar->addStretch();
    setLayout(layoutRightBar);
}
void RightBar::createIntensity()
{
    intensity = new QLabel("Интенсивность:");
    intensity->setAlignment(Qt::AlignLeft);
    valueIntensity = new QLabel;

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
    connect(valueTransparency, &QSlider::sliderMoved, this, &RightBar::signalSliderTransparency);
}
void RightBar::createFallibility()
{
    fallibility = new QLabel("Порог:");
    fallibility->setAlignment(Qt::AlignLeft);
    valueFallibility = new QSpinBox;
    valueFallibility->setFixedWidth(50);
    valueFallibility->setMinimum(0);
    valueFallibility->setMaximum(50);

    connect(valueFallibility, SIGNAL(valueChanged(int)), this, SIGNAL(signalFallibility(int)));
}
void RightBar::createLine()
{
    accuracy = new QLabel("Точность построения области:");
    valueAccuracy = new QSpinBox;
    valueAccuracy->setFixedWidth(50);
    valueAccuracy->setMinimum(0);
    valueAccuracy->setMaximum(255);

    connect(valueAccuracy, SIGNAL(valueChanged(int)), this, SIGNAL(accuracyChanged(int)));
}
void RightBar::createButton()
{
    colorChoice = new QPushButton("Выбор цвета...");
    connect(colorChoice, &QPushButton::clicked, this, &RightBar::signalColor);
}

void RightBar::createCheckBoxes()
{
    displayArea = new QCheckBox("Отображать область");
    innerContours = new QCheckBox("Отображать внутренние контуры");
    connect(displayArea, &QCheckBox::stateChanged, this, &RightBar::checkBoxDisplayAreaStateChanged);
    connect(innerContours, &QCheckBox::stateChanged, this, &RightBar::checkBoxInnerContoursStateChanged);
}

void RightBar::createCountNodes()
{
    nodes = new QLabel("Количество узлов:");
    nodes->setAlignment(Qt::AlignLeft);
    countNodes = new QLabel;

    layoutNodes = new QHBoxLayout;
    layoutNodes->addWidget(nodes);
    layoutNodes->addWidget(countNodes);
}
void RightBar::setValueIntensity(QString a)
{
    valueIntensity->setText(a);
}

void RightBar::setCountNodes(QString a)
{
    countNodes->setText(a);
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
