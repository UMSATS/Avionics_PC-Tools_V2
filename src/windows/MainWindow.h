#ifndef AVIONICS_PC_TOOLS_GUI_MAINWINDOW_H
#define AVIONICS_PC_TOOLS_GUI_MAINWINDOW_H

#include <QMainWindow>

class MainWindow : public QMainWindow {
    Q_OBJECT
protected:
    void closeEvent(QCloseEvent *event) override;
public:
    MainWindow();
signals:
    void onExitButtonPressed();
};


#endif //UNTITLED1_MAINWINDOW_H
