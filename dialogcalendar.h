#ifndef DIALOGCALENDAR_H
#define DIALOGCALENDAR_H

#include <QDialog>

namespace Ui {
class dialogCalendar;
}

class dialogCalendar : public QDialog
{
    Q_OBJECT

public:
    explicit dialogCalendar(QWidget *parent = nullptr);
    ~dialogCalendar();

    QDate selectedDate() const;

private:
    Ui::dialogCalendar *ui;
};

#endif // DIALOGCALENDAR_H
