//
// Created by vasil on 16/11/2020.
//

#ifndef AVIONICS_PC_TOOLS_GUI_EMULATIONCONTROLLER_HPP
#define AVIONICS_PC_TOOLS_GUI_EMULATIONCONTROLLER_HPP

#include "IRunnable.hpp"
#include "DataFeeder.hpp"
#include "EventDetector.h"

class EmulationController : public IRunnable
{
    Q_OBJECT
public:
    EmulationController();
    EmulationController(const QString & flightData);
    void setFlightData(const QString & fileName);
    void setFlightDataHeaderFormat(int headerFormatStyle, const QVector<QPair<int, int>> & header);
signals:
    void onFlightStateChanged( FlightEventStatus state, float altitude, float timestamp);
    void onFlightProgressChanged ( int progress, float altitude, float timestamp);
protected:
    void run() override;
private:
    void notify ( FlightEventStatus state);
    static void reset ( Data & data );
    void update( Data & data );

    Data mFlightData;
    DataFeeder mDataFeeder;
    EventDetector mEventDetector;
};


#endif //AVIONICS_PC_TOOLS_GUI_EMULATIONCONTROLLER_HPP
