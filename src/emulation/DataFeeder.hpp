#ifndef __DATA_FEEDER_H
#define __DATA_FEEDER_H

#include "IRunnable.hpp"

#include <string>
#include <deque>
#include <thread>
#include <memory>
#include <mutex>
#include <functional>
#include <QMutex>
#include <QMap>

#include "definitions.h"

class DataFeeder : public IRunnable
{
Q_OBJECT
    struct xyz_data
    {
        float timestamp;
        float x;
        float y;
        float z;
    };

public:
    explicit DataFeeder(QString file);
    DataFeeder();

    ~DataFeeder() override;

    void setFile(QString file);
    void setHeaderFormat(int style, const QVector<QPair<int, int>> & header);
    QString file() const;
    void getGyroscopeMeasurement(IMUData&);
    void getAccelerometerMeasurement(IMUData&);
    void getPressureMeasurement(PressureData&);
signals:
    void onProgressChanged(int progress);

private:
    float acc2g(int16_t acc_value);
    float g2acc(float g);
    float rot2deg_per_sec(int16_t gyro_value);
    float deg_per_sec2rot(float deg_per_sec);

    void run() override;
    QVariant createType(int type);
    std::deque <xyz_data>       gyro_queue;
    std::deque <xyz_data>       acc_queue;
    std::deque <PressureDataU>  press_queue;
    QString                     mCSVFile;
    QMutex                      mGuard;
    int mLinesTotal         = 0;
    int mCurrentLine        = 0;
    int mPreviousProgress   = 0;
    int mCurrentProgress    = 0;

    QMap<int, QVariant> mRowValues;
    static const int MAX_ITEMS;
    int mHeaderFormatStyle = 0;
};

#endif // __DATA_FEEDER_H

