#ifndef BACKUPSCREEN_H
#define BACKUPSCREEN_H

#include <QWidget>

namespace Ui {
class BackupScreen;
}

class BackupScreen : public QWidget
{
    Q_OBJECT

public:
    explicit BackupScreen(QWidget *parent = nullptr);
    ~BackupScreen();

private slots:

    void on_selectDate_clicked();

private:
    Ui::BackupScreen *ui;
};

#endif // BACKUPSCREEN_H
