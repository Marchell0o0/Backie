#ifndef RECOVERYSCREEN_H
#define RECOVERYSCREEN_H

#include <QWidget>

namespace Ui {
class RecoverScreen;
}

class RecoveryScreen : public QWidget
{
    Q_OBJECT

public:
    explicit RecoveryScreen(QWidget *parent = nullptr);
    ~RecoveryScreen();

private:
    Ui::RecoverScreen *ui;
};

#endif // RECOVERYSCREEN_H
