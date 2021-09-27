#include"DataFeeder.hpp"
#include <string>
#include <iostream>
#include <chrono>
#include <utility>
#include <fstream>
#include <QVariant>
#include <QtCore/QFile>
#include <QDebug>
#include <cmath>

#include "utilities/csv.h"
#include "utilities/CSVHelper.h"

const int DataFeeder::MAX_ITEMS = 100;

DataFeeder::DataFeeder(QString file)
{
   setFile(std::move(file));
   mHeaderFormatStyle = io::CSVHelper::HeaderFormatStyle::SRAD;
}

DataFeeder::DataFeeder() : DataFeeder("")
{

}

void DataFeeder::setFile(QString file)
{
    mCSVFile = std::move(file);
    std::ifstream inFile(mCSVFile.toStdString());
    mLinesTotal = std::count(std::istreambuf_iterator<char>(inFile),
                             std::istreambuf_iterator<char>(), '\n');
}

void DataFeeder::setHeaderFormat(int style, const QVector<QPair<int, int>> &header)
{
    for (int i = 0 ; i < header.size(); ++i)
        mRowValues.insert(header[i].first, createType(header[i].second));

    mHeaderFormatStyle = style;
}

QString DataFeeder::file() const {
    return mCSVFile;
}

void DataFeeder::getGyroscopeMeasurement(IMUData & container)
{
    if ( !gyro_queue.empty( ) )
    {
        mGuard.lock();
        auto ref = gyro_queue.front( );
        container.data.timestamp = ref.timestamp;
        container.data.gyroscope[0] = ref.x;
        container.data.gyroscope[1] = ref.y;
        container.data.gyroscope[2] = ref.z;
        container.updated = true;
        gyro_queue.pop_front( );
        mGuard.unlock();
    }
}

void DataFeeder::getAccelerometerMeasurement(IMUData & container)
{
    if ( !acc_queue.empty( ) )
    {
        mGuard.lock();
        auto ref = acc_queue.front( );
        container.data.timestamp = ref.timestamp;
        container.data.accelerometer[0] = ref.x;
        container.data.accelerometer[1] = ref.y;
        container.data.accelerometer[2] = ref.z;
        container.updated = true;
        acc_queue.pop_front( );
        mGuard.unlock();
    }
}

void DataFeeder::getPressureMeasurement(PressureData & container)
{
    if ( !press_queue.empty( ) )
    {
        mGuard.lock();
        auto ref = press_queue.front( );
        container.data.timestamp     = ref.timestamp;
        container.data.temperature   = ref.temperature;
        container.data.pressure      = ref.pressure;
        container.updated = true;
        press_queue.pop_front( );
        mGuard.unlock();
    }
}

void DataFeeder::run()
{
    double timestamp {0};
    xyz_data gyro = {0}, acc = {0}, mag = {0};
    PressureDataU press = {0};
    double altMSL;
    uint8_t flags[4];

    double alt;
    uint32_t ev;

    mPreviousProgress = 0;
    mCurrentProgress = 0;
    mCurrentLine = 0;

    uint32_t lastTimeStamp = 0;


    if(mHeaderFormatStyle == io::CSVHelper::COTS)
    {
        io::CSVReader<17> reader (mCSVFile.toStdString());
        printf("C++ DataFeeder has successfully started.\n");
        emit started();
        // time,acceleration,pres,altMSL,temp,latxacc,latyacc,gyrox,gyroy,gyroz,magx,magy,magz,launch_detect,apogee_detect,Aon,Bon
        while (isRunning() && reader.read_row(timestamp, acc.x, press.pressure, altMSL, press.temperature, acc.y, acc.z, gyro.x, gyro.y, gyro.z, mag.x, mag.y, mag.z, flags[0], flags[1], flags[2], flags[3]))
        {
            acc.timestamp   = timestamp;
            gyro.timestamp  = timestamp;
            press.timestamp = timestamp;

            mGuard.lock();

            if ( acc_queue.size( ) >= MAX_ITEMS )
                acc_queue.pop_front( );
            acc_queue.push_back( acc );

            if ( gyro_queue.size( ) >= MAX_ITEMS )
                gyro_queue.pop_front( );
            gyro_queue.push_back( gyro );

            if ( press_queue.size( ) >= MAX_ITEMS )
                press_queue.pop_front( );
            press_queue.push_back( press );

            mGuard.unlock();

            sleepMillis((2));
            lastTimeStamp = timestamp;

            mCurrentLine++;
            mCurrentProgress = mCurrentLine * 100 / mLinesTotal;
            if(mCurrentProgress > mPreviousProgress)
            {
                mPreviousProgress = mCurrentProgress;
                emit onProgressChanged(mPreviousProgress);
            }

        }
    }
    else if(mHeaderFormatStyle == io::CSVHelper::SRAD || mHeaderFormatStyle == io::CSVHelper::CUSTOM )
    {
        // TODO: need an improvement for the current logic of CSV formatting

        io::CSVReader<9> reader (mCSVFile.toStdString());

        printf("C++ DataFeeder has successfully started.\n");
        emit started();
        // time,accx,accy,accz,rotx,roty,rotz,temp,pres,alt,Flags
        while ( isRunning() && reader.read_row (timestamp, acc.x, acc.y, acc.z, gyro.x, gyro.y, gyro.z, press.pressure, press.temperature ) )
        {
            acc.timestamp   = timestamp;
            gyro.timestamp  = timestamp;
            press.timestamp = timestamp;

            acc.x           = acc2g(acc.x);
            acc.y           = acc2g(acc.y);
            acc.z           = acc2g(acc.z);
            gyro.x          = rot2deg_per_sec(gyro.x);
            gyro.y          = rot2deg_per_sec(gyro.y);
            gyro.z          = rot2deg_per_sec(gyro.z);

            press.pressure = press.pressure / 100;

            mGuard.lock();

            if ( acc_queue.size( ) >= MAX_ITEMS )
                acc_queue.pop_front( );
            acc_queue.push_back( acc );

            if ( gyro_queue.size( ) >= MAX_ITEMS )
                gyro_queue.pop_front( );
            gyro_queue.push_back( gyro );

            if ( press_queue.size( ) >= MAX_ITEMS )
                press_queue.pop_front( );
            press_queue.push_back( press );

            mGuard.unlock();

            sleepMillis((3));
            lastTimeStamp = timestamp;

            mCurrentLine++;
            mCurrentProgress = mCurrentLine * 100 / mLinesTotal;
            if(mCurrentProgress > mPreviousProgress)
            {
                mPreviousProgress = mCurrentProgress;
            }

            emit onProgressChanged(mPreviousProgress);
        }
    }

    stopRunning();
    printf("C++ DataFeeder has successfully exited.\n");
    emit finished();
}

DataFeeder::~DataFeeder() {
    stopRunning();
}

QVariant DataFeeder::createType(int iType)
{
    auto type = static_cast<io::CSVHelper::data_type>(iType);
    switch(type)
    {
        case io::CSVHelper::data_type::UINT8:
            return QVariant::fromValue(static_cast<uint8_t>(0));
        case io::CSVHelper::data_type::UINT16:
            return QVariant::fromValue(static_cast<uint16_t>(0));
        case io::CSVHelper::data_type::UINT32:
            return QVariant::fromValue(static_cast<uint32_t>(0));
        case io::CSVHelper::data_type::UINT64:
            return QVariant::fromValue(static_cast<uint64_t>(0));
        case io::CSVHelper::data_type::INT8:
            return QVariant::fromValue(static_cast<int8_t>(0));
        case io::CSVHelper::data_type::INT16:
            return QVariant::fromValue(static_cast<int16_t>(0));
        case io::CSVHelper::data_type::INT32:
            return QVariant::fromValue(static_cast<int32_t>(0));
        case io::CSVHelper::data_type::INT64:
            return QVariant::fromValue(static_cast<int64_t>(0));
        case io::CSVHelper::data_type::LONG:
            return QVariant::fromValue(static_cast<long>(0));
        case io::CSVHelper::data_type::FLOAT:
            return QVariant::fromValue(static_cast<float>(0));
        case io::CSVHelper::data_type::DOUBLE:
            return QVariant::fromValue(static_cast<double>(0));
        case io::CSVHelper::data_type::UNKNOWN:
            return QVariant();
    }
}
















namespace
{
#define CONFIG_IMU_SENSOR_DEFAULT_ACC_BANDWIDTH	        UINT8_C(0x02)
#define CONFIG_IMU_SENSOR_DEFAULT_ACC_ODR			    UINT8_C(0x08)
#define CONFIG_IMU_SENSOR_DEFAULT_ACC_RANGE		        UINT8_C(0x02)
#define CONFIG_IMU_SENSOR_DEFAULT_ACC_POWER			    UINT8_C(0x00)

#define CONFIG_IMU_SENSOR_DEFAULT_GYRO_BANDWIDTH	    UINT8_C(0x04)
#define CONFIG_IMU_SENSOR_DEFAULT_GYRO_ODR		        UINT8_C(0x04)
#define CONFIG_IMU_SENSOR_DEFAULT_GYRO_RANGE		    UINT8_C(0x01)
#define CONFIG_IMU_SENSOR_DEFAULT_GYRO_POWER		    UINT8_C(0x00)

    typedef struct imu_sensor_configuration
    {
        uint8_t accel_bandwidth;
        uint8_t accel_output_data_rate;
        uint8_t accel_range;
        uint8_t accel_power;

        uint8_t gyro_bandwidth;
        uint8_t gyro_output_data_rate;
        uint8_t gyro_range;
        uint8_t gyro_power;

    } IMUSensorConfiguration;

    const IMUSensorConfiguration DEFAULT_CONFIGURATION = {

            .accel_bandwidth                = CONFIG_IMU_SENSOR_DEFAULT_ACC_BANDWIDTH,
            .accel_output_data_rate         = CONFIG_IMU_SENSOR_DEFAULT_ACC_ODR,
            .accel_range                    = CONFIG_IMU_SENSOR_DEFAULT_ACC_RANGE,
            .accel_power                    = CONFIG_IMU_SENSOR_DEFAULT_ACC_POWER,

            .gyro_bandwidth                 = CONFIG_IMU_SENSOR_DEFAULT_GYRO_BANDWIDTH,
            .gyro_output_data_rate          = CONFIG_IMU_SENSOR_DEFAULT_GYRO_ODR,
            .gyro_range                     = CONFIG_IMU_SENSOR_DEFAULT_GYRO_RANGE,
            .gyro_power                     = CONFIG_IMU_SENSOR_DEFAULT_GYRO_POWER,
    };

    IMUSensorConfiguration CURRENT_CONFIGURATION = DEFAULT_CONFIGURATION;
}

float DataFeeder::acc2g(int16_t acc_value)
{
    const int16_t range = pow(2, (CURRENT_CONFIGURATION.accel_range + 1)) * 1.5;
    const float result = (float) acc_value / 32768 * range;
    return result;
}

float DataFeeder::g2acc(float g)
{
    const uint16_t result = (int16_t) (g * 32768 / (pow(2, (CURRENT_CONFIGURATION.accel_range + 1)) * 1.5));
    return result;
}

float DataFeeder::rot2deg_per_sec(int16_t gyro_value)
{
//    /**
//    * Registers containing the angular velocity sensor output. The sensor output is stored as signed 16-bit
//    * number in 2’s complement format in each 2 registers. From the registers, the gyro values can be
//    * calculated as follows:
//    * Rate_X: RATE_X_MSB * 256 + RATE_X_LSB
//    * Rate_Y: RATE_Y_MSB * 256 +
//    * Rate_Z: RATE_Z_MSB * 256 + RATE_Z_LSB
//    */
//
//    union
//    {
//        int16_t value;
//        uint8_t bytes[sizeof( int16_t )];
//    } container;
//
//    container.value = gyro_value;
//    int16_t actual_value = to_int16_t( container.bytes );

    const int16_t range = pow(2, (CURRENT_CONFIGURATION.gyro_range + 1)) * 1.5;
    const float result = (float) gyro_value / 32768 * range;
    return result;
}

float DataFeeder::deg_per_sec2rot(float deg_per_sec) {
    return ( (int16_t) (deg_per_sec * 32768) / (pow(2, (CURRENT_CONFIGURATION.gyro_range + 1)) * 1.5) );
}

