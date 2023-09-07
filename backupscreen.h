#ifndef BACKUPSCREEN_H
#define BACKUPSCREEN_H

#include <QWidget>
#include <QButtonGroup>
#include <filesystem>
#include <vector>
#include <variant>

//#include "errand.h"
#include "schedule.h"

/**
 * @class BackupArgs
 * @brief Manages the backup arguments used for creating backup tasks.
 *
 * This class stores and manages various settings for backups,
 * including the type of backup, source directory, scheduling, and time.
 */
class BackupArgs {
private:
    BackupType backupType = BackupType::FULL;             ///< Specifies the type of backup (e.g., Full, Incremental, etc.)
    std::filesystem::path sourceBackupDirPath = ""; ///< Path to the source directory to be backed up
    ScheduleRecurrence backupRecurrence = ScheduleRecurrence::DAILY; ///< Specifies the backup schedule recurrence (e.g., Daily, Weekly, etc.)
    std::array<int, 6> dateArgs = {-1, -1, -1, -1, -1, -1};       ///< Date and time arguments for the backup

public:
    enum DateArgs {
        YEAR,
        MONTH,
        DAYOFWEEK,
        DAYOFMONTH,
        HOUR,
        MINUTE,
    };

    /**
     * @brief Clears all backup arguments.
     */
    void clear();

    /**
     * @brief Checks if the date is complete (YEAR, MONTH, and DAYOFMONTH are set).
     * @return True if the date is complete, false otherwise.
     */
    bool isDateComplete();

    //Setter methods for backup arguments
    void setBackupRecurrence(ScheduleRecurrence recurrence);
    void setBackupType(BackupType type);
     void setSourcePath(const std::filesystem::path& path);
    void setDate(DateArgs type, int value);

    //Getter methods for backup arguments
    ScheduleRecurrence getBackupRecurrence();
    BackupType getBackupType();
    std::filesystem::path getSourcePath();
    int getDate(DateArgs type) const;
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
    /**
     * @brief Initializes recurrence radio buttons and date-related combo boxes.
     */
    void initRecurrAndDateBtns();

    /**
     * @brief Initializes the "Days of the Week" buttons and groups them.
     */
    void initDaysOfWeekBtns();

    /**
     * @brief Opens a dialog for the user to browse for a source folder for backup.
     */
    void browseForSourceFolder();

    /**
     * @brief Handles changes in the source path text field.
     *
     * @param newPath The new path as a QString.
     */
    void onSourcePathEdited(const QString& newPath);

    /**
     * @brief Opens a calendar dialog for the user to select a specific date.
     */
    // TODO: resolve "on" error prone in func name
    void on_selectDateB_clicked();

    /**
     * @brief Handles the toggling of radio buttons for selecting the backup recurrence.
     *
     * @param isChecked Flag indicating whether the button is checked.
     */
    void handleRadioButtonToggle(bool isChecked);

    /**
     * @brief Handles item changes in the date-related combo boxes.
     *
     * @param index The new index of the combo box.
     */
    void handleComboBoxIndexChanged(int index);

    /**
     * @brief Handles the toggling of dayOfWeek buttons for selecting the day of week for backup
     * @param dayOfWeek The toggled button of day of week in int 1...7 format (start from Monday)
     */
    void handleDayOfWeekButtonClicked(QAbstractButton* button, bool isChecked);

    /**
     * @brief Updates the label displaying the chosen date based on current backup arguments.
     *
     * @param date The current backup arguments.
     */
    void updateChosenDateLabel();

    /**
     * @brief Creates a backup task or schedule based on the current settings.
     */
    void createBackupBclicked();

private:
    Ui::BackupScreen *ui;

    BackupArgs backupArgs;
    QButtonGroup* daysOfWeekBtnGroup;
    QMap<int, QString> dayOfWeekMap;
    /**
     * @brief Connects specified radio buttons and combo boxes with their corresponding handler functions.
     *
     * @tparam T The widget type (QRadioButton or QComboBox).
     * @tparam U The enumeration type representing the recurrence or date argument.
     * @param mapping A list of widget and enumeration type pairs.
     */
    template <typename T, typename U>
    void connectRecurrAndDateBtns(const QList<QPair<T*, U>>& mapping);

    /**
     * @brief Adds a leading zero to a number 0-9 for formatting.
     *
     * @param number The number to format.
     * @return A QString containing the formatted number.
     */
    QString addZero(int number);

    /**
     * @brief Formats date to QString in "DD.MM.YYYY at HH:MM" format
     * @param day
     * @param month
     * @param year
     * @param hour
     * @param minute
     * @return QString date in "DD.MM.YYYY at HH:MM" format
     */
    QString formatDate(int day,
                       int month,
                       int year,
                       int hour,
                       int minute);

    /**
     * @brief Formats the time in "HH:MM" format
     * @param hour
     * @param minute
     * @return QString in "HH:MM" format
     */
    QString getTimeString(int hour, int minute);
};

#endif // BACKUPSCREEN_H
