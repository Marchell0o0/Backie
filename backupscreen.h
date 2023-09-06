#ifndef BACKUPSCREEN_H
#define BACKUPSCREEN_H

#include <QWidget>
#include <filesystem>
#include <vector>
#include <variant>

//#include "errand.h"
#include "schedule.h"

class BackupArgs {
private:
    BackupType backupType = BackupType::FULL;
    std::filesystem::path sourceBackupDirPath = "";
    ScheduleRecurrence backupRecurrence = ScheduleRecurrence::DAILY;
    std::array<int, 6> dateArgs = {-1, -1, -1, -1, -1, -1};

public:
    enum DateArgs {
        YEAR,
        MONTH,
        DAYOFWEEK,
        DAYOFMONTH,
        HOUR,
        MINUTE,
    };

    void clear();

    void setDate(DateArgs type, int value);
    int getDate(DateArgs type) const;

    std::array<int, 6> getDateArgs();

    void setSourcePath(const std::filesystem::path& path);
    std::filesystem::path getSourcePath();

    void setBackupType(BackupType type);
    BackupType getBackupType();

    void setBackupRecurrence(ScheduleRecurrence recurrence);
    ScheduleRecurrence getBackupRecurrence();
};

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

    void initRecurrAndDateBtns();
    void initDaysOfWeekBtns();

    void browseForSourceFolder();
    void onSourcePathEdited(const QString& newPath);

    // TODO: resolve "on" error prone in func name
    void on_selectDateB_clicked();

    void handleRadioButtonToggle(bool isChecked);
    void handleComboBoxIndexChanged(int index);

    void updateChosenDateLabel(BackupArgs date);

    void createBackupBclicked();

private:
    Ui::BackupScreen *ui;

    BackupArgs backupArgs;

    template <typename T, typename U>
    void connectRecurrAndDateBtns(const QList<QPair<T*, U>>& mapping);

    int* deleteExtraSymbolsFromDate(BackupArgs date);

    void printOutDateArgs(std::vector<int> dateArgs);
    QString addZero(int number);
};

#endif // BACKUPSCREEN_H
