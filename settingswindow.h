#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QDialog>

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = nullptr);
    ~SettingsWindow();

private slots:
    void on_MyCS_returnPressed();

    void on_buttonBox_accepted();

private:
    Ui::SettingsWindow *ui;
};

#endif // SETTINGSWINDOW_H
