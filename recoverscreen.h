#ifndef RECOVERSCREEN_H
#define RECOVERSCREEN_H

#include <QWidget>

namespace Ui {
class RecoverScreen;
}

class RecoverScreen : public QWidget
{
    Q_OBJECT

public:
    explicit RecoverScreen(QWidget *parent = nullptr);
    ~RecoverScreen();

private:
    Ui::RecoverScreen *ui;
};

#endif // RECOVERSCREEN_H
