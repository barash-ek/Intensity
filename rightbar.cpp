#include "rightbar.h"
#include <QHBoxLayout>
#include <QColorDialog>
#include <QDebug>
#include <QPalette>
#include <QFrame>
#include <QResizeEvent>
#include <QLayout>
rightBar::rightBar(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_StaticContents);
    QPalette pall;
    pall.setColor(this->backgroundRole(), Qt::white);
    setPalette(pall);
    setAutoFillBackground(true);
    frameBar = new QFrame(this);
    frameBar->setFrameStyle(QFrame::Box| QFrame::Plain);
    frameBar->setLineWidth(2);
    transparency = new QLabel("Прозрачность:");
    transparency->setAlignment(Qt::AlignCenter);
    transparency->setFixedHeight(50);
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
    sliderTransparency->setValue(255);
    sliderTransparency->setSingleStep(1);

    QVBoxLayout *layoutSlider = new QVBoxLayout;
    layoutSlider->addWidget(transparency);
    layoutSlider->setSpacing(0);
    layoutSlider->addWidget(sliderTransparency);
    //setLayout(layoutSlider);

    intensity = new QLabel("Пределы интенсивности:");
    lineIntensity = new QLineEdit;
    lineIntensity->setMaxLength(3);
    lineIntensity->setFixedWidth(30);
    QVBoxLayout *layoutLine = new QVBoxLayout;
    QVBoxLayout *layline = new QVBoxLayout;
    layline->QLayout::setContentsMargins(30, 0, 30, 0);
    layline->addWidget(lineIntensity);
    layoutLine->addWidget(intensity);
    layoutLine->setSpacing(0);
    layoutLine->addLayout(layline);
    //setLayout(layoutLine);

    colorChoice = new QPushButton("Выбор цвета...");
    colorChoice->setFlat(true);
    connect(colorChoice, SIGNAL(clicked()), this, SLOT(colorDialog()));

    QVBoxLayout *layoutRightBar = new QVBoxLayout;
    layoutRightBar->addLayout(layoutSlider);
    layoutRightBar->setSpacing(1);
    layoutRightBar->addLayout(layoutLine);
    layoutRightBar->setSpacing(1);
    layoutRightBar->addWidget(colorChoice);
    layoutRightBar->QLayout::setContentsMargins(5, 0, 5, 400);
    setLayout(layoutRightBar);
    resize(100, 500);
}
void rightBar::resizeEvent(QResizeEvent *event)
{
    frameBar->resize(event->size());
    QWidget::resizeEvent(event);
}
void rightBar::colorDialog()
{
    QColor color = QColorDialog::getColor("red");
    if (color.isValid() ) {
    colorChosen = color;
    }
}
