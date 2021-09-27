//
// Created by vasil on 16/11/2020.
//

#ifndef AVIONICS_PC_TOOLS_GUI_FLIGHTDATAFORMATDIALOG_HPP
#define AVIONICS_PC_TOOLS_GUI_FLIGHTDATAFORMATDIALOG_HPP

#include <QDialog>
#include <QtCore/QItemSelection>
#include <QtGui/QStandardItem>
#include <QtWidgets/QListView>
#include <QVBoxLayout>

#include "utilities/CSVHelper.h"

class QWidget;
class QPushButton;
class FlightDataFormatDialog : public QDialog
{
private:
    Q_OBJECT
public:
    explicit FlightDataFormatDialog(QWidget *_parent = nullptr);
    void open();
signals:
    void onFlightDataFormatChanged(int format_style, const QVector<QPair<int, int>> & header);
private:
    void addColumn(io::CSVHelper::data_label label = io::CSVHelper::data_label::UNKNOWN, io::CSVHelper::data_type type = io::CSVHelper::data_type::UNKNOWN);
    void removeColumn(int index = -1);
    void setCOTSFlightDataFormat();
    void setSRADFlightDataFormat();

    QStringList mDataLabels;
    QStringList mDataTypes;
    int mColumnCount = 0;
    QVBoxLayout * mColumnsVLayout;
    QVector<QHBoxLayout*> mColumnHLayoutItems;
    QVector<QPair<int, int>> mColumns;
    int mHeaderFormatStyle = io::CSVHelper::HeaderFormatStyle::SRAD;
};

#endif //AVIONICS_PC_TOOLS_GUI_FLIGHTDATAFORMATDIALOG_HPP
