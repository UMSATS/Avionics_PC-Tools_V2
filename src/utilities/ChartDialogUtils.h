//
// Created by vasil on 16/11/2020.
//

#ifndef AVIONICS_PC_TOOLS_GUI_CHARTDIALOGUTILS_H
#define AVIONICS_PC_TOOLS_GUI_CHARTDIALOGUTILS_H

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

class ChartDialogUtils: public QObject
{
public:
    ChartDialogUtils(const QString &title, QObject * parent = nullptr);
    void show();
    void hide();
    void clear();
    void addAltitudeData(float altitude, float timestamp);
    void addEventData(float altitude, float timestamp);

    void setXAxisName(const QString & name);
    void setYAxisName(const QString & name);

    void rescale(QPointF xAxis, QPointF yAxis);
private:
    QtCharts::QChartView * mChartView;
    QtCharts::QLineSeries * mAltitudeSeries;
    QtCharts::QScatterSeries * mEventSeries;

    QtCharts::QChart * mChart;

    qreal yMin;
    qreal yMax;
    qreal xMin;
    qreal xMax;
};


#endif //AVIONICS_PC_TOOLS_GUI_CHARTDIALOGUTILS_H
