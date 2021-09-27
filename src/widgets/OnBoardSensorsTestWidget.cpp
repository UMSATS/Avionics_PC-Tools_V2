#include "OnBoardSensorsTestWidget.h"
#include <QVBoxLayout>
#include "graphics/cube/rotationcube.h"
#include <iostream>
#include <QPushButton>
#include <QLineEdit>

#include <QVBoxLayout>
#include <QtWidgets/QFileDialog>
#include <QtCore/QDirIterator>
#include <QtCore/QSettings>
#include <QtCharts/QtCharts>

#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>

#include <QTimer>
#include <QSet>
#include <QtCore/QThreadPool>
#include <QDebug>
#include <iostream>
#include <QtCore/QProcess>

namespace
{
    //Groups both sensor readings and a time stamp.
    typedef struct imu_sensor_data
    {
        uint32_t timestamp; // time of sensor reading in ticks.

        float acc_x;
        float acc_y;
        float acc_z;

        float gyro_x;
        float gyro_y;
        float gyro_z;

    } IMUSensorData;

    typedef union
    {
        struct
        {
            IMUSensorData values;
        };
        uint8_t bytes[sizeof ( IMUSensorData )];
    } IMUSensorDataU;

    IMUSensorDataU parsed;
}



OnBoardSensorsTestWidget::OnBoardSensorsTestWidget(QWidget *_parent)
: QGroupBox(_parent), mRotationCube(new RotationCube(this))
{
    mChartView = new SensorPlotWidget(this);
    setTitle("Sensor Testing");

    auto root = new QVBoxLayout;
    auto subroot = new QHBoxLayout;

    auto left = new QVBoxLayout;
    auto resetCubeBtn      = new QPushButton("Reset");

    left->addWidget(mRotationCube);
    left->addWidget(resetCubeBtn);

    connect(resetCubeBtn, &QPushButton::released, [=]{ mRotationCube->resetToDefaultPosition(); });


    auto right = new QGridLayout;

    auto mGyroLbl             = new QLabel("Gyroscope");
    mGyroXLineEdit            = new QLineEdit("--");
    mGyroYLineEdit            = new QLineEdit("--");
    mGyroZLineEdit            = new QLineEdit("--");

    auto mAccLbl              = new QLabel("Accelerometer");
    mAccXLineEdit             = new QLineEdit("--");
    mAccYLineEdit             = new QLineEdit("--");
    mAccZLineEdit             = new QLineEdit("--");

    auto mMagLbl              = new QLabel("Magnetometer");
    mMagXLineEdit             = new QLineEdit("--");
    mMagYLineEdit             = new QLineEdit("--");
    mMagZLineEdit             = new QLineEdit("--");

    mAccXLineEdit->setReadOnly(true);
    mAccYLineEdit->setReadOnly(true);
    mAccZLineEdit->setReadOnly(true);

    mGyroXLineEdit->setReadOnly(true);
    mGyroYLineEdit->setReadOnly(true);
    mGyroZLineEdit->setReadOnly(true);

    mMagXLineEdit->setReadOnly(true);
    mMagZLineEdit->setReadOnly(true);
    mMagZLineEdit->setReadOnly(true);


    right->addWidget(mGyroLbl, 0, 0, Qt::AlignLeft);
    right->addWidget(new QLabel("X: "), 0, 1);
    right->addWidget(new QLabel("Y: "), 0, 3);
    right->addWidget(new QLabel("Z: "), 0, 5);

    right->addWidget(mGyroXLineEdit, 0, 2);
    right->addWidget(mGyroYLineEdit, 0, 4);
    right->addWidget(mGyroZLineEdit, 0, 6);

    right->addWidget(mAccLbl, 1, 0, Qt::AlignLeft);
    right->addWidget(new QLabel("X: "), 1, 1);
    right->addWidget(new QLabel("Y: "), 1, 3);
    right->addWidget(new QLabel("Z: "), 1, 5);

    right->addWidget(mAccXLineEdit, 1, 2);
    right->addWidget(mAccYLineEdit, 1, 4);
    right->addWidget(mAccZLineEdit, 1, 6);

    right->addWidget(mMagLbl, 2, 0, Qt::AlignLeft);

    right->addWidget(new QLabel("X: "), 2, 1);
    right->addWidget(new QLabel("Y: "), 2, 3);
    right->addWidget(new QLabel("Z: "), 2, 5);

    right->addWidget(mMagXLineEdit, 2, 2);
    right->addWidget(mMagYLineEdit, 2, 4);
    right->addWidget(mMagZLineEdit, 2, 6);

    subroot->addLayout(left);
    subroot->addLayout(right);

    root->addLayout(subroot);
    root->addWidget(mChartView);

    setLayout(root);

    setMinimumHeight(1200);
    setMinimumWidth(2000);


    mChartView->clear();
}

void OnBoardSensorsTestWidget::parseSensorDataPackage(QByteArray & data)
{
    memcpy(&parsed, data, sizeof(IMUSensorData));
    if (parsed.values.gyro_x == 0.0 || parsed.values.gyro_y == 0.0 || parsed.values.gyro_z == 0.0)
        return;

//    mAccXLineEdit->setText(mNumber.number(parsed.values.acc_x));
//    mAccYLineEdit->setText(mNumber.number(parsed.values.acc_y));
//    mAccZLineEdit->setText(mNumber.number(parsed.values.acc_z));
//
//    mGyroXLineEdit->setText(mNumber.number(parsed.values.gyro_x));
//    mGyroYLineEdit->setText(mNumber.number(parsed.values.gyro_y));
//    mGyroZLineEdit->setText(mNumber.number(parsed.values.gyro_z));

    updateSensorData(parsed.values.acc_x, parsed.values.acc_y, parsed.values.acc_z, parsed.values.gyro_x, parsed.values.gyro_y, parsed.values.gyro_z, 0, 0, 0);

    mChartView->addGyroscopeData(parsed.values.gyro_x, parsed.values.gyro_y, parsed.values.gyro_z);
    mChartView->addAccelerometerData(parsed.values.acc_x, parsed.values.acc_y, parsed.values.acc_z);

    memset(&parsed, 0, sizeof(IMUSensorData));
}

void OnBoardSensorsTestWidget::updateSensorData (float gyro_x, float gyro_y, float gyro_z, float acc_x, float acc_y, float acc_z, float mag_x, float mag_y, float mag_z)
{
    mRotationCube->updateSensorData ( gyro_x, gyro_y, gyro_z, acc_x, acc_y, acc_z, mag_x, mag_y, mag_z );
}


























//
// Created by vasil on 16/11/2020.
//

#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QValueAxis>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QLegend>

#include <QDebug>

SensorPlotWidget::SensorPlotWidget(QWidget * parent): QWidget (parent)
{
    auto root = new QHBoxLayout;
    mChart = new QtCharts::QChart;
    mChart->setTitle("Sensors Data Plot");
    mChart->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    mChart->setMargins(QMargins(5, 5, 5, 5));

    auto xAxis = new QtCharts::QValueAxis();
    auto yAxis = new QtCharts::QValueAxis();
    xAxis->setTitleText("Timestamp");
    yAxis->setTitleText("Value");
    xAxis->setMin(mXAxisMin = std::clock());
    xAxis->setMax(mXAxisMax = std::clock() + 1000);

    yAxis->setRange(mYAxisMin = -5, mYAxisMax = 5);

    mChart->addAxis(xAxis, Qt::AlignBottom);
    mChart->addAxis(yAxis, Qt::AlignLeft);



    mChart->legend()->setMarkerShape(QtCharts::QLegend::MarkerShapeFromSeries);

    mGyroSeriesX = new QtCharts::QLineSeries();
    mGyroSeriesX->setName("gyro_x");

    mGyroSeriesY = new QtCharts::QLineSeries();
    mGyroSeriesY->setName("gyro_y");

    mGyroSeriesZ = new QtCharts::QLineSeries();
    mGyroSeriesZ->setName("gyro_z");

    mAccSeriesX = new QtCharts::QLineSeries();
    mAccSeriesX->setName("acc_x");

    mAccSeriesY = new QtCharts::QLineSeries();
    mAccSeriesY->setName("acc_y");

    mAccSeriesZ = new QtCharts::QLineSeries();
    mAccSeriesZ->setName("acc_z");

    mMagSeriesX = new QtCharts::QLineSeries();
    mMagSeriesX->setName("mag_x");

    mMagSeriesY = new QtCharts::QLineSeries();
    mMagSeriesY->setName("mag_y");

    mMagSeriesZ = new QtCharts::QLineSeries();
    mMagSeriesZ->setName("mag_z");

    QPen red(Qt::red);
    red.setWidth(5);
    mGyroSeriesX->setPen(red);
    red.setColor(Qt::black);
    mGyroSeriesY->setPen(red);
    red.setColor(Qt::yellow);
    mGyroSeriesZ->setPen(red);

    mChart->addSeries(mGyroSeriesX);
    mChart->addSeries(mGyroSeriesY);
    mChart->addSeries(mGyroSeriesZ);

    mGyroSeriesX->attachAxis(xAxis);
    mGyroSeriesX->attachAxis(yAxis);

    mGyroSeriesY->attachAxis(xAxis);
    mGyroSeriesY->attachAxis(yAxis);

    mGyroSeriesZ->attachAxis(xAxis);
    mGyroSeriesZ->attachAxis(yAxis);



    QPen green(Qt::blue);
    green.setWidth(8);
    mAccSeriesX->setPen(green);
    red.setColor(Qt::gray);
    mAccSeriesY->setPen(green);
    red.setColor(Qt::darkCyan);
    mAccSeriesZ->setPen(green);

    mChart->addSeries(mAccSeriesX);
    mChart->addSeries(mAccSeriesY);
    mChart->addSeries(mAccSeriesZ);

    mAccSeriesX->attachAxis(xAxis);
    mAccSeriesX->attachAxis(yAxis);

    mAccSeriesY->attachAxis(xAxis);
    mAccSeriesY->attachAxis(yAxis);

    mAccSeriesZ->attachAxis(xAxis);
    mAccSeriesZ->attachAxis(yAxis);



    QPen blue(Qt::green);
    blue.setWidth(3);
    mMagSeriesX->setPen(blue);
    red.setColor(Qt::magenta);
    mMagSeriesY->setPen(blue);
    red.setColor(Qt::darkBlue);
    mMagSeriesZ->setPen(blue);

    mChart->addSeries(mMagSeriesX);
    mChart->addSeries(mMagSeriesY);
    mChart->addSeries(mMagSeriesZ);

    mMagSeriesX->attachAxis(xAxis);
    mMagSeriesX->attachAxis(yAxis);

    mMagSeriesY->attachAxis(xAxis);
    mMagSeriesY->attachAxis(yAxis);

    mMagSeriesZ->attachAxis(xAxis);
    mMagSeriesZ->attachAxis(yAxis);



    mChart->setMinimumWidth(1200);
    mChart->setMinimumHeight(400);

    mChart->show();

    mChartView = new QtCharts::QChartView(mChart, this);
    mChartView->setRenderHint(QPainter::Antialiasing);
    mChartView->setContentsMargins(5, 5, 5, 5);
    mChartView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mChartView->setMinimumSize({1200, 400});
    mChartView->show();

    root->addWidget(mChartView);

    setLayout(root);





//    connect(mGyroSeriesX, &QtCharts::QLineSeries::pointAdded, [=](int index)
//    {
//        qreal y = mAccSeriesX->at(index).y();
//
//        if(y < yMin || y > yMax)
//        {
//            if(y < yMin) {
//                yMin = y;
//            }
//            if(y > yMax) {
//                yMax = y;
//            }
//
//            mChart->axisY()->setRange(yMin-200, yMax+200);
//        }
//
//        qreal x = mAccSeriesX->at(index).x();
//        if(x < xMin || x > xMax){
//            if(x < xMin) {
//                xMin = x;
//            }
//            if(x > xMax) {
//                xMax = x;
//                xMin = xMax - 20;
//            }
//
//            mChart->axisX()->setRange(xMin-500, xMax+500);
//        }
//
//    });

    mStart = std::clock();
}

void SensorPlotWidget::clear()
{
    mGyroSeriesX->clear();
    mGyroSeriesY->clear();
    mGyroSeriesZ->clear();

    mAccSeriesX->clear();
    mAccSeriesY->clear();
    mAccSeriesZ->clear();

    mMagSeriesX->clear();
    mMagSeriesY->clear();
    mMagSeriesZ->clear();

    mChartView->update();
    mChartView->repaint();

    mStart = std::clock();
}

void SensorPlotWidget::adjustAxesRange (float x, float y1, float y2, float y3)
{
    if (x > mXAxisMax)
    {
        mXAxisMax = x;
        mChart->axisX()->setMax(mXAxisMax );
        mChart->axisX()->setMin(mXAxisMax - 1000 );
    }

//    if (y1 > mYAxisMax && y1 < 10000)
//    {
//        mYAxisMax = y1;
//        mChart->axisY()->setMax(mYAxisMax );
//    }
//
//    if (y1 < mYAxisMin && y1 > -10000)
//    {
//        mYAxisMin = y1;
//        mChart->axisY()->setMin(mYAxisMin );
//    }
//
//    if (y2 > mYAxisMax && y1 < 10000)
//    {
//        mYAxisMax = y2;
//        mChart->axisY()->setMax(mYAxisMax );
//    }
//
//    if (y2 < mYAxisMin && y1 > -10000)
//    {
//        mYAxisMin = y2;
//        mChart->axisY()->setMin(mYAxisMin );
//    }
//
//    if (y3 > mYAxisMax && y1 < 10000)
//    {
//        mYAxisMax = y3;
//        mChart->axisY()->setMax(mYAxisMax );
//    }
//
//    if (y3 < mYAxisMin && y1 > -10000)
//    {
//        mYAxisMin = y3;
//        mChart->axisY()->setMin(mYAxisMin );
//    }
}


void SensorPlotWidget::addAccelerometerData(float x, float y, float z)
{
    float timestamp = time(NULL) - mStart;
    *mAccSeriesX << QPointF(timestamp, x);
    *mAccSeriesY << QPointF(timestamp, y);
    *mAccSeriesZ << QPointF(timestamp, z);

    // adjustAxesRange(timestamp, x, y, z);
}


void SensorPlotWidget::addGyroscopeData(float x, float y, float z)
{
    float timestamp = (std::clock() - mStart);

    *mGyroSeriesX << QPointF(timestamp, x);
    *mGyroSeriesY << QPointF(timestamp, y);
    *mGyroSeriesZ << QPointF(timestamp, z);

    adjustAxesRange(timestamp, x, y, z);
}

void SensorPlotWidget::addMagnetometerData(float x, float y, float z)
{
    float timestamp = time(NULL) - mStart;
    *mMagSeriesX << QPointF(timestamp, x);
    *mMagSeriesY << QPointF(timestamp, y);
    *mMagSeriesZ << QPointF(timestamp, z);

    adjustAxesRange(timestamp, x, y, z);
}


void SensorPlotWidget::rescale(QPointF xAxis, QPointF yAxis)
{
    mChart->axisX()->setRange(xAxis.x(), xAxis.y());
    mChart->axisY()->setRange(yAxis.x(), yAxis.y());
}

void SensorPlotWidget::setXAxisName(const QString &name)
{

}

void SensorPlotWidget::setYAxisName(const QString &name)
{

}

