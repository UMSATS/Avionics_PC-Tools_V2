//
// Created by vasil on 15/11/2020.
//

#ifndef AVIONICS_PC_TOOLS_GUI_FLIGHTEMULATIONWIDGET_H
#define AVIONICS_PC_TOOLS_GUI_FLIGHTEMULATIONWIDGET_H

#include <QtWidgets/QGroupBox>
#include "emulation/EmulationController.hpp"
#include "dialogs/FlightDataFormatDialog.hpp"
#include "utilities/ChartDialogUtils.h"

class QLineEdit;
class QProgressBar;
class QPushButton;
class QLabel;
class FlightEmulationWidget : public QGroupBox
{
    Q_OBJECT
public:
    explicit FlightEmulationWidget(QWidget * _parent = nullptr);
    bool isEmulationRunning() const;
signals:
    void flightDataFileChanged(const QString & path);
private:
    void detectFlightDataFile();
    void startEmulation(const QString & file);
    void stopEmulation();

    QLineEdit * mFlightDataFile;
    QProgressBar * mEmulationProgress;
    QPushButton * mEmulateBtn;
    QPushButton * mSpecifyFlightDataFormat;
    QLabel * mFlightStatusVLbl;
    QLabel * mCurrAltVLbl;
    EmulationController mEmulator;
    std::unique_ptr<FlightDataFormatDialog> mDialog;
    std::unique_ptr<ChartDialogUtils> mChartWindow;

};


#endif //AVIONICS_PC_TOOLS_GUI_FLIGHTEMULATIONWIDGET_H
