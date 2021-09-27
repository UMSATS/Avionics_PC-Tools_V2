//
// Created by vasil on 16/11/2020.
//

#include "ChartDialogUtils.h"
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QValueAxis>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QLegend>

#include <QDebug>

ChartDialogUtils::ChartDialogUtils(const QString &title, QObject * parent) : QObject(parent)
{
    mAltitudeSeries = new QtCharts::QLineSeries();
    mAltitudeSeries->setName("Altitude");

    mEventSeries = new QtCharts::QScatterSeries();
    mEventSeries->setName("Flight Event");
    mEventSeries->setMarkerShape(QtCharts::QScatterSeries::MarkerShapeRectangle);
    mEventSeries->setMarkerSize(10.0);

    mChart = new QtCharts::QChart();
    mChart->setTitle(title);
    mChart->legend()->show();

    mChart->setMargins(QMargins(5,5,5,5));

    mChartView = new QtCharts::QChartView(mChart);
    mChartView->setRenderHint(QPainter::Antialiasing);
    mChartView->setContentsMargins(5,5,5,5);
    mChartView->setMinimumSize({400, 400});

    QPen green(Qt::red);
    green.setWidth(3);
    mAltitudeSeries->setPen(green);
    mChart->addSeries(mAltitudeSeries);
    mChart->addSeries(mEventSeries);

    auto xAxis = new QtCharts::QValueAxis();
    auto yAxis = new QtCharts::QValueAxis();
    xAxis->setTitleText("Timestamp");
    yAxis->setTitleText("Altitude");

    mChart->addAxis(xAxis, Qt::AlignBottom);
    mChart->addAxis(yAxis, Qt::AlignLeft);

    mEventSeries->attachAxis(xAxis);
    mEventSeries->attachAxis(yAxis);

    mAltitudeSeries->attachAxis(xAxis);
    mAltitudeSeries->attachAxis(yAxis);

    yMax = 10;
    yMin = 0;
    xMax = 0;
    xMin = 10;

    mChart->legend()->setMarkerShape(QtCharts::QLegend::MarkerShapeFromSeries);


//    connect(mAltitudeSeries, &QtCharts::QLineSeries::pointAdded, [=](int index)
//    {
//        qreal y = mAltitudeSeries->at(index).y();
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
//        qreal x = mAltitudeSeries->at(index).x();
//
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
}


void ChartDialogUtils::show()
{
    mChartView->show();
}

void ChartDialogUtils::hide()
{
    mChartView->hide();
}

void ChartDialogUtils::clear()
{
    mAltitudeSeries->clear();
    mChartView->update();
    mChartView->repaint();
}

void ChartDialogUtils::addAltitudeData(float altitude, float timestamp)
{
     *mAltitudeSeries << QPointF(timestamp, altitude);
}

void ChartDialogUtils::addEventData(float altitude, float timestamp)
{
    *mEventSeries << QPointF(timestamp, altitude);
}

void ChartDialogUtils::rescale(QPointF xAxis, QPointF yAxis)
{
    mChart->axisX()->setRange(xAxis.x(), xAxis.y());
    mChart->axisY()->setRange(yAxis.x(), yAxis.y());
}

void ChartDialogUtils::setXAxisName(const QString &name)
{

}

void ChartDialogUtils::setYAxisName(const QString &name)
{

}

