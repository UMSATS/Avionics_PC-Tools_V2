//
// Created by vasil on 15/11/2020.
//

#include "EventDetector.h"
#include <cmath>
#include <ios>
#include <QDebug>
#include <QThread>

#define userconf_EVENT_DETECTION_AVERAGING_SUPPORT_ON 1

#if (userconf_EVENT_DETECTION_AVERAGING_SUPPORT_ON == 1)
#include "utilities/data_window.h"
#endif

#define CRITICAL_VERTICAL_ACCELERATION  6.9 // [g]
#define APOGEE_ACCELERATION             0.1 // [g]
#define MAIN_CHUTE_ALTITUDE             381 // [m] (converted from 1,250ft)
#define LANDING_ROTATION_SPEED          5   // [deg / s]

#define ALTITUDE_SENSITIVITY_THRESHOLD  5


#if (userconf_EVENT_DETECTION_AVERAGING_SUPPORT_ON == 1)
    moving_data_buffer altitude_data_window, vertical_acc_data_window;
    float mean( float * array, size_t length, size_t start, size_t end);
    int AVERAGE_PRESSURE_SAMPLING_RATE = 0;
    int AVERAGE_IMU_SAMPLING_RATE      = 0;
#endif



EventDetector::EventDetector(): EventDetector(0)
{

}

void EventDetector::setGroundPressure(uint32_t ground_pressure)
{
    GROUND_PRESSURE = ground_pressure;
    GROUND_ALTITUDE = calculate_altitude (GROUND_PRESSURE );
//    qDebug() << "GROUND_PRESSURE" << GROUND_PRESSURE;
//    qDebug() << "GROUND_ALTITUDE" << GROUND_ALTITUDE;
}

EventDetector::EventDetector(uint32_t ground_pressure) :
    GROUND_PRESSURE                    ( ground_pressure          ),
    GROUND_ALTITUDE(calculate_altitude (GROUND_PRESSURE )),
    mFlightState                       ( Launchpad                )
{

#if (userconf_EVENT_DETECTION_AVERAGING_SUPPORT_ON == 1)
    data_window_init ( &altitude_data_window );
    data_window_init ( &vertical_acc_data_window );
#endif

}

FlightEventStatus EventDetector::feed(Data &data)
{
    if(data.pressure.updated)
    {
        if(GROUND_PRESSURE == 0)
        {
            setGroundPressure(data.pressure.data.pressure);
            return mFlightState;
        }

        mCurrentAltitude = calculate_altitude(data.pressure.data.pressure) - GROUND_ALTITUDE;
        mCurrentPressure = data.pressure.data.pressure;

#if (userconf_EVENT_DETECTION_AVERAGING_SUPPORT_ON == 1)
        data_window_insert( &altitude_data_window, &mCurrentAltitude );
#endif

    }

    if(!data.inertial.updated)
        return mFlightState;

    switch ( mFlightState )
    {
        case Launchpad:
        {
            if(data.inertial.updated)
            {
                if ( detectLaunch( data.inertial.data.accelerometer[ 0 ] ) )
                {
                    printf("FLIGHT_STATE_LAUNCHPAD: Detected Launch at %fm\n", mCurrentAltitude);
                    emit onFlightStateChanged(mCurrentAltitude, PreApogee);
                    mFlightState = PreApogee;
                }
            }
        }

        break;
        case PreApogee:
        {
            if(data.inertial.updated)
            {

#if (userconf_EVENT_DETECTION_AVERAGING_SUPPORT_ON == 1)
                // Here we need to start looking at the average altitude and see the differences in the gradient sign
                // the idea is that if the gradient changes the sign then we reached the apogee since the altitude
                // is now decreasing instead of increasing. Also to avoid mechanical errors if the sign changes
                // dramatically then it cannot represent the real world scenario, as in the real world the inertia
                // makes it stop really slowly as well as falling down, therefore the difference needs to be small.

                float previous_average_altitude = mean( altitude_data_window.linear_repr, sizeof( altitude_data_window.linear_repr ), 0, MOVING_BUFFER_RANGE - 1 );
                float last_average_altitude     = mean( altitude_data_window.linear_repr, sizeof( altitude_data_window.linear_repr ), 1, MOVING_BUFFER_RANGE );

                float difference = last_average_altitude - previous_average_altitude;
                float absolute_difference = fabs(fabs(last_average_altitude) - fabs(previous_average_altitude));

                if((difference < 0) && absolute_difference < 5 && absolute_difference > 0.2)
                {
                    printf( "FLIGHT_STATE_PRE_APOGEE: Detected APOGEE at %fm\n", mCurrentAltitude);
                    mFlightState = Apogee;
                }
#else

                if ( detectApogee( data.inertial.data.accelerometer[ 0 ], data.inertial.data.accelerometer[ 1 ], data.inertial.data.accelerometer[ 2 ] ) )
                {
                    printf( "FLIGHT_STATE_PRE_APOGEE: Detected APOGEE at %fm\n", mCurrentAltitude);
                    emit onFlightStateChanged(mCurrentAltitude, Apogee);
                    mFlightState = Apogee;
                }

#endif

            }
        }

        break;
        case Apogee:
        {
            printf( "FLIGHT_STATE_APOGEE: Igniting recovery circuit drogue\n");
            emit onFlightStateChanged(mCurrentAltitude, PostApogee);
            mFlightState = PostApogee;
        }

        break;
        case PostApogee:
        {
            if(data.pressure.updated)
            {
                if ( detectAltitude(MAIN_CHUTE_ALTITUDE, GROUND_ALTITUDE, data.pressure.data.pressure ) )
                {
                    printf( "FLIGHT_STATE_POST_APOGEE: Detected Main Chute at %fm\n", mCurrentAltitude);
                    emit onFlightStateChanged(mCurrentAltitude, MainChute);
                    mFlightState = MainChute;
                }
            }
        }

        break;
        case MainChute:
        {
            printf("FLIGHT_STATE_MAIN_CHUTE: Igniting recovery circuit for the main chute\n");
            emit onFlightStateChanged(mCurrentAltitude, PostMain);
            mFlightState = PostMain;
        }

        break;
        case PostMain:
        {
            if(data.inertial.updated)
            {
                if ( detectLanding( data.inertial.data.gyroscope[ 0 ], data.inertial.data.gyroscope[ 1 ],
                                    data.inertial.data.gyroscope[ 2 ] ) )
                {
                    printf( "FLIGHT_STATE_POST_MAIN: Detected landing at %fm\n", mCurrentAltitude);
                    emit onFlightStateChanged(mCurrentAltitude, Landed);
                    mFlightState = Landed;
                }
            }

            // OR

            if(data.pressure.updated)
            {
                if ( detectAltitude(0, GROUND_ALTITUDE, data.pressure.data.pressure ) )
                {
                    printf( "FLIGHT_STATE_POST_MAIN: Detected landing at %fm\n", mCurrentAltitude);
                    emit onFlightStateChanged(mCurrentAltitude, Landed);
                    mFlightState = Landed;
                }
            }
        }

        break;
        case Landed:
        {
            printf("FLIGHT_STATE_LANDED: Rocket landed!\n");
            emit onFlightStateChanged(mCurrentAltitude, Exited);
            mFlightState = Exited;
        }

        break;
        case Exited:
        {

        }
    }

    return mFlightState;
}

float EventDetector::currentAltitude() const
{
    return mCurrentAltitude;
}


float EventDetector::currentPressure() const
{
    return mCurrentPressure;
}

void EventDetector::reset() {
    mFlightState = Launchpad;
    mCurrentAltitude = GROUND_ALTITUDE;
    mCurrentPressure = GROUND_PRESSURE;
}

bool EventDetector::isInFlight() const
{
    return mFlightState != Launchpad;
}


float EventDetector::calculate_altitude(float pressure)
{
    /*
    Pb = static pressure (pressure at sea level) [Pa]
    Tb = standard temperature (temperature at sea level) [K]
    Lb = standard temperature lapse rate [K/m] = -0.0065 [K/m]
    h  = height about sea level [m]
    hb = height at the bottom of atmospheric layer [m]
    R  = universal gas constant = 8.31432
    q0 = gravitational acceleration constant = 9.80665
    M  = molar mass of Earth’s air = 0.0289644 [kg/mol]
    */

    static const float  Pb  = 101325.00f;
    static const float  Tb  = 15.00 + 273.15;
    static const float  Lb  = -0.0065;
    static const int    hb  = 0;
    static const float  R   = 8.31432;
    static const float  g0  = 9.80665;
    static const float  M   = 0.0289644;

    return hb + ( Tb / Lb ) * ( pow ( ( pressure / Pb ), ( -R * Lb ) / ( g0 * M ) ) - 1 );
}

bool EventDetector::detectLaunch    ( float vertical_acceleration_in_g )
{
    return vertical_acceleration_in_g > CRITICAL_VERTICAL_ACCELERATION;
}


bool EventDetector::detectApogee    ( float acceleration_x_in_g, float acceleration_y_in_g, float acceleration_z_in_g )
{
    const float ACCELERATION_VECTOR = sqrt(acceleration_x_in_g*acceleration_x_in_g + acceleration_y_in_g*acceleration_y_in_g + acceleration_z_in_g*acceleration_z_in_g);
    return ACCELERATION_VECTOR < APOGEE_ACCELERATION;
}


bool EventDetector::detectAltitude  ( float target_altitude, uint32_t ground_altitude, uint32_t current_pressure )
{
    float current_altitude = calculate_altitude( current_pressure ) - ground_altitude;
    return fabsf ( ( current_altitude ) - ( target_altitude ) ) < ALTITUDE_SENSITIVITY_THRESHOLD;
}

bool EventDetector::detectLanding   ( float gyro_x_in_deg_per_sec, float gyro_y_in_deg_per_sec, float gyro_z_in_deg_per_sec )
{
    float gyroscope_orientation_vector = sqrt( gyro_x_in_deg_per_sec * gyro_x_in_deg_per_sec + gyro_y_in_deg_per_sec * gyro_y_in_deg_per_sec + gyro_z_in_deg_per_sec * gyro_z_in_deg_per_sec);
    return gyroscope_orientation_vector < LANDING_ROTATION_SPEED;
}



#if (userconf_EVENT_DETECTION_AVERAGING_SUPPORT_ON == 1)
float mean( float * array, size_t length, size_t start, size_t end)
{
    assert(end < length);
    assert(start < end);

    float sum = 0;
    for (size_t i = start; i <= end; i++)
    {
        sum += array[i];
    }

    return sum / (end - start);
}

#endif