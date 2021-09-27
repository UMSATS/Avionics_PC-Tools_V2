//
// Created by vasil on 15/11/2020.
//

#ifndef AVIONICS_FLIGHT_EMULATOR_EVENTDETECTOR_H
#define AVIONICS_FLIGHT_EMULATOR_EVENTDETECTOR_H

#include <QObject>
#include <cinttypes>
#include <QtCore/QObject>
#include "definitions.h"

#if (userconf_EVENT_DETECTION_AVERAGING_SUPPORT_ON == 1)
#include "event-detection/data_window.h"
#endif

class EventDetector : public QObject
{
    Q_OBJECT
public:
    EventDetector();
    explicit EventDetector(uint32_t ground_pressure);
    FlightEventStatus feed (Data & data);
    float currentAltitude() const;
    float currentPressure() const;
    void reset();

    bool isInFlight() const;
    void setGroundPressure(uint32_t ground_pressure);

signals:
    void onFlightStateChanged(float currentAltitude, FlightEventStatus flightState);

public:
    static float calculate_altitude ( float pressure );
private:
    static bool detectLaunch    ( float vertical_acceleration_in_g );
    static bool detectApogee    ( float acceleration_x_in_g, float acceleration_y_in_g, float acceleration_z_in_g );
    static bool detectAltitude  ( float target_altitude, uint32_t ground_pressure, uint32_t current_pressure );
    static bool detectLanding   ( float gyro_x_in_deg_per_sec, float gyro_y_in_deg_per_sec, float gyro_z_in_deg_per_sec );

    FlightEventStatus mFlightState;

    float mCurrentPressure    = 0;
    float mCurrentAltitude    = 0;
    uint32_t GROUND_PRESSURE  = 0;
    float GROUND_ALTITUDE     = 0;
};


#endif //AVIONICS_FLIGHT_EMULATOR_EVENTDETECTOR_H
