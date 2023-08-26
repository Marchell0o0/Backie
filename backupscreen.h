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

    void on_selectDateB_clicked();

    void handleRadioButtonToggle(bool isChecked);

    void printOutDateComponents(std::vector<int> dateComponents);

private:
    Ui::BackupScreen *ui;
    std::vector<int> dateComponents;

};

#endif // BACKUPSCREEN_H
