//
// Created by vasil on 16/11/2020.
//

#include "EmulationController.hpp"
#include <QDateTime>

EmulationController::EmulationController(): EmulationController("") {}
EmulationController::EmulationController(const QString &flightData) : mDataFeeder(flightData)
{
    connect(&mDataFeeder, &DataFeeder::onProgressChanged, [=](int progress)
    {
        onFlightProgressChanged(progress, mEventDetector.currentAltitude(), mFlightData.inertial.data.timestamp);
    });
}

void EmulationController::setFlightData(const QString &fileName)
{
    mDataFeeder.setFile(fileName);
}


void EmulationController::setFlightDataHeaderFormat(int style, const QVector<QPair<int, int>> &header)
{
    mDataFeeder.setHeaderFormat(style, header);
}

void EmulationController::run()
{
    if(mDataFeeder.file().isEmpty())
        return;

    FlightEventStatus currentState = Launchpad;

    mDataFeeder.startRunning();

    emit started();
    while ( isRunning() )
    {
        reset(mFlightData);

        update (mFlightData);

        currentState = mEventDetector.feed(mFlightData);

        notify(currentState);
    }

    mDataFeeder.stopRunning();
    mEventDetector.reset();
    emit finished();
}

void EmulationController::update(Data &data)
{
    mDataFeeder.getAccelerometerMeasurement( data.inertial );
    mDataFeeder.getGyroscopeMeasurement( data.inertial );
    mDataFeeder.getPressureMeasurement( data.pressure );
}

void EmulationController::reset( Data & data )
{
    data.inertial.updated   = false;
    data.pressure.updated   = false;
    data.event.updated      = false;
    data.continuity.updated = false;
}


void EmulationController::notify(FlightEventStatus state)
{
    static FlightEventStatus lastState  = Invalid;

    if(state != lastState)
    {
        lastState = state;
        emit onFlightStateChanged(state, mEventDetector.currentAltitude(), mFlightData.inertial.data.timestamp);
    }
}