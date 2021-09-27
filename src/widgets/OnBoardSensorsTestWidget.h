//
// Created by vasil on 6/20/2021.
//

#ifndef AVIONICS_PC_TOOLS_GUI_ONBOARDSENSORSTESTWIDGET_H
#define AVIONICS_PC_TOOLS_GUI_ONBOARDSENSORSTESTWIDGET_H


#include <QGroupBox>

namespace QtCharts
{
    class QChartView;
    class QLineSeries;
    class QChart;
    class QScatterSeries;
}

class SensorPlotWidget;
class QLineEdit;
class RotationCube;
class OnBoardSensorsTestWidget : public QGroupBox
{
public:
    explicit OnBoardSensorsTestWidget(QWidget * _parent = nullptr);
    void updateSensorData(float acc_x, float acc_y, float acc_z, float gyro_x, float gyro_y, float gyro_z, float mag_x, float mag_y, float mag_z);
    void parseSensorDataPackage(QByteArray & data);
private:
    RotationCube * mRotationCube;

    QLineEdit * mGyroXLineEdit;
    QLineEdit * mGyroYLineEdit;
    QLineEdit * mGyroZLineEdit;

    QLineEdit * mAccXLineEdit ;
    QLineEdit * mAccYLineEdit ;
    QLineEdit * mAccZLineEdit ;

    QLineEdit * mMagXLineEdit ;
    QLineEdit * mMagYLineEdit ;
    QLineEdit * mMagZLineEdit ;

    SensorPlotWidget * mChartView;

    QString mNumber;
};


#include <QObject>
#include <QString>
#include <QtCore/QPointF>

namespace QtCharts
{
    class QChartView;
    class QLineSeries;
    class QChart;
    class QScatterSeries;
}

class SensorPlotWidget: public QWidget
{
public:
    SensorPlotWidget(QWidget * parent = nullptr);
    QtCharts::QChart * getHandler() { return mChart; }
    void clear();

    void setXAxisName(const QString & name);
    void setYAxisName(const QString & name);

    void rescale(QPointF xAxis, QPointF yAxis);

    void addAccelerometerData(float x, float y, float z);
    void addGyroscopeData(float x, float y, float z);
    void addMagnetometerData(float x, float y, float z);

private:
    void adjustAxesRange(float x, float y1, float y2, float y3);
    QtCharts::QChartView * mChartView;
    QtCharts::QChart * mChart;
    qreal mXAxisMin, mXAxisMax;
    qreal mYAxisMin, mYAxisMax;

    QtCharts::QLineSeries *mGyroSeriesX;
    QtCharts::QLineSeries *mGyroSeriesY;
    QtCharts::QLineSeries *mGyroSeriesZ;

    QtCharts::QLineSeries *mAccSeriesX;
    QtCharts::QLineSeries *mAccSeriesY;
    QtCharts::QLineSeries *mAccSeriesZ;

    QtCharts::QLineSeries *mMagSeriesX;
    QtCharts::QLineSeries *mMagSeriesY;
    QtCharts::QLineSeries *mMagSeriesZ;

    std::clock_t    mStart;


    qreal yMin;
    qreal yMax;
    qreal xMin;
    qreal xMax;
};


#endif //AVIONICS_PC_TOOLS_GUI_ONBOARDSENSORSTESTWIDGET_H
