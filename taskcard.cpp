#include "utils.h"
#include "taskcard.h"
#include "ui_taskcard.h"

TaskCard::TaskCard(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::TaskCard)
{
    ui->setupUi(this);

    loadStyleSheet(":/styles/taskCard.css", ui->stylingTaskCardWidget);

    detailsPart = ui->detailsCardPart;
    foldedPart = ui->foldedCardPart;

    detailsPart->setMaximumHeight(0);

    animation = new QPropertyAnimation(detailsPart, "maximumHeight", this);
    animation->setDuration(animationDuration);

    // capture click events for the folded part of card
    foldedPart->installEventFilter(this);
}

TaskCard::~TaskCard()
{
    delete ui;
}
void TaskCard::toggle() {
    if (detailsPart->maximumHeight() == 0) {
        animation->setStartValue(0);
        animation->setEndValue(150);
    } else {
        animation->setStartValue(150);
        animation->setEndValue(0);
    }
    animation->start();
}
bool TaskCard::eventFilter(QObject *watched, QEvent *event) {
    if (watched == foldedPart && event->type() == QEvent::MouseButtonPress) {
        toggle();
        return true;
    }
    return QWidget::eventFilter(watched, event);
}
