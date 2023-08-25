#include "dialogcalendar.h"
#include "ui_dialogcalendar.h"

dialogCalendar::dialogCalendar(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dialogCalendar)
{
    ui->setupUi(this);
}

dialogCalendar::~dialogCalendar()
{
    delete ui;
}

QDate dialogCalendar::selectedDate() const {
    return ui->calendarWidget->selectedDate();
}
