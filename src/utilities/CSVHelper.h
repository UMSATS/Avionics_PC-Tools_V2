//
// Created by vasil on 16/11/2020.
//

#ifndef AVIONICS_PC_TOOLS_GUI_CSVHELPER_H
#define AVIONICS_PC_TOOLS_GUI_CSVHELPER_H

#include <QFile>
#include <QDebug>

namespace io::CSVHelper
{
    enum class data_label
    {
        TMSTMP,
        GYRO_X,
        GYRO_Y,
        GYRO_Z,
        ACC_X,
        ACC_Y,
        ACC_Z,
        MAG_X,
        MAG_Y,
        MAG_Z,
        PRESS,
        TEMP,
        ALT_MSL,
        FLAG_LAUNCH_DETECT,
        FLAG_APOGEE_DETECT,
        FLAG_A_ON,
        FLAG_B_ON,
        UNKNOWN
    };

    enum class data_type
    {
        UINT8,
        UINT16,
        UINT32,
        UINT64,
        INT8,
        INT16,
        INT32,
        INT64,
        LONG,
        FLOAT,
        DOUBLE,
        UNKNOWN
    };

    enum HeaderFormatStyle { SRAD, COTS, CUSTOM };


    static inline QVariant getSmallestValueAt(const QString & csvFile, int column)
    {
        QFile file(csvFile);
        if (!file.open(QIODevice::ReadOnly))
        {
            return 1;
        }

        long long int minValue = std::numeric_limits<qint64>::max();

        while (!file.atEnd())
        {
            std::string line = file.readLine().toStdString();
            auto list = QString::fromStdString(line).split(',');

            if(list.size() <= column)
            {
                break;
            }

            auto value = QString(list[column]).toFloat();

            if(value <= minValue)
                minValue = value;
        }

        return minValue;
    }

    static inline QVariant getBiggestValueAt(const QString & csvFile, int column)
    {
        QFile file(csvFile);
        if (!file.open(QIODevice::ReadOnly))
        {
            return 1;
        }

        long long int maxValue = std::numeric_limits<qint64>::min();

        while (!file.atEnd())
        {
            std::string line = file.readLine().toStdString();
            auto list = QString::fromStdString(line).split(',');

            if(list.size() <= column)
            {
                break;
            }

            auto value = QString(list[column]).toFloat();

            if(value >= maxValue)
                maxValue = value;
        }

        return maxValue;
    }
}

#endif //AVIONICS_PC_TOOLS_GUI_CSVHELPER_H
