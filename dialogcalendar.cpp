#include "dialogcalendar.h"
#include "ui_dialogcalendar.h"

dialogCalendar::dialogCalendar(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dialogCalendar)
{
    ui->setupUi(this);

    calendar = ui->calendarWidget;
    calendar->setMinimumDate(QDate::currentDate());
    calendar->setMaximumDate(QDate::currentDate().addYears(5));
}

dialogCalendar::~dialogCalendar()
{
    delete ui;
}

QDate dialogCalendar::selectedDate() const {
    return ui->calendarWidget->selectedDate();
}
