//
// Created by vasil on 16/11/2020.
//

#include "FlightDataFormatDialog.hpp"
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDebug>
#include <QTimer>

#include <QComboBox>

FlightDataFormatDialog::FlightDataFormatDialog(QWidget *_parent)
{
    mDataLabels <<  "timestamp" << "gyro[x]" << "gyro[y]" << "gyro[z]" << "acc[x]" << "acc[y]" << "acc[z]"  << "mag[x]" << "mag[y]"
                << "mag[z]"  << "pressure"  << "temperature" <<  "altMSL" << "FlagLaunchDetected" << "flagApogeeDetected" << "flagAOn" << "flagBOn";

    mDataTypes << "uint8" << "uint16" << "uint32" << "uint64"<< "int8" << "int16"
               << "int32" << "int64" << "long" << "float" << "double";

    setMinimumWidth(400);

    setWindowTitle("Please specify the Flight Data CSV header");
    auto root = new QVBoxLayout;
    mColumnsVLayout = new QVBoxLayout;
    auto title = new QLabel("CSV Header Format:");
    auto setCOTSFormatBtn = new QPushButton("Set Default (COTS)");
    auto setSRADFormatBtn = new QPushButton("Set Default (SRAD)");

    auto dialogCtrlLayout = new QHBoxLayout;
    auto acceptBtn = new QPushButton("Accept");
    auto rejectBtn = new QPushButton("Cancel");
    dialogCtrlLayout->addWidget(rejectBtn);
    dialogCtrlLayout->addWidget(acceptBtn);

    auto addOneMoreColBtn = new QPushButton(" Add Column");
    addOneMoreColBtn->setContentsMargins(0,0,0,0);
    addOneMoreColBtn->setIcon(QIcon(":/images/icon_plus.png"));

    root->addWidget(title);
    root->addWidget(setCOTSFormatBtn);
    root->addWidget(setSRADFormatBtn);

    root->addLayout(mColumnsVLayout);
    root->addWidget(addOneMoreColBtn);
    root->addLayout(dialogCtrlLayout);

    setLayout(root);

    connect(addOneMoreColBtn, &QPushButton::released, [=]
    {
        addColumn();
    });

    connect(setCOTSFormatBtn, &QPushButton::released, [=]
    {
        setCOTSFlightDataFormat();
    });

    connect(setSRADFormatBtn, &QPushButton::released, [=]
    {
        setSRADFlightDataFormat();
    });

    connect(acceptBtn, &QPushButton::released, [=]
    {
        close();
        emit onFlightDataFormatChanged(mHeaderFormatStyle, mColumns);
    });

    connect(rejectBtn, &QPushButton::released, [=]
    {
        close();
    });
}

void FlightDataFormatDialog::open()
{
    QDialog::open();
}

void FlightDataFormatDialog::addColumn(io::CSVHelper::data_label label, io::CSVHelper::data_type type)
{
    auto colContainer = new QHBoxLayout;

    auto columnName = new QLabel("[Col" + QString::number(mColumnCount++) + "] ");
    columnName->setProperty("id", mColumnCount);
    colContainer->addWidget(columnName);

    colContainer->addWidget(new QLabel("Label: "));
    auto firstColLblCmb = new QComboBox;
    firstColLblCmb->addItems(mDataLabels);
    colContainer->addWidget(firstColLblCmb);

    colContainer->addWidget(new QLabel("Type: "));
    auto firstColTypeCmb = new QComboBox;
    firstColTypeCmb->addItems(mDataTypes);
    colContainer->addWidget(firstColTypeCmb);

    auto removeOneColBtn = new QPushButton(" Remove");
    removeOneColBtn->setContentsMargins(0,0,0,0);
    removeOneColBtn->setIcon(QIcon(":/images/icon_minus.png"));
    colContainer->addWidget(removeOneColBtn);

    mColumnHLayoutItems.push_back(colContainer);
    mColumnsVLayout->addLayout(mColumnHLayoutItems.back());

    int columnCount = mColumnCount;
    connect(removeOneColBtn, &QPushButton::released, [this, columnCount]
    {
        for (int i = 0; i < mColumnHLayoutItems.size(); ++i)
        {
            auto row = mColumnHLayoutItems.at(i);
            auto columnsContainer = row->layout();
            for (int j = 0; j < columnsContainer->count(); ++j)
            {
                auto label = dynamic_cast<QLabel*>(columnsContainer->layout()->itemAt(j)->widget());
                if(label != nullptr)
                {
                    if(columnCount == label->property("id"))
                    {
                        removeColumn(i);
                        break;
                    }
                }
            }
        }
    });

    if(label == io::CSVHelper::data_label::UNKNOWN && type == io::CSVHelper::data_type::UNKNOWN)
    {
        mColumns.push_back({firstColLblCmb->currentIndex(), firstColTypeCmb->currentIndex()});
    }
    else
    {
        firstColLblCmb->setCurrentIndex(static_cast<int>(label));
        firstColTypeCmb->setCurrentIndex(static_cast<int>(type));
        mColumns.push_back({static_cast<int>(label), static_cast<int>(type)} );
    }

    connect(firstColLblCmb, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index)
    {
        for (int i = 0; i < mColumnHLayoutItems.size(); ++i)
        {
            auto row = mColumnHLayoutItems.at(i);
            auto columnsContainer = row->layout();
            for (int j = 0; j < columnsContainer->count(); ++j)
            {
                auto label = dynamic_cast<QLabel*>(columnsContainer->layout()->itemAt(j)->widget());
                if(label != nullptr)
                {
                    if(columnCount == label->property("id"))
                    {
                        qDebug() << "Col #" + QString::number(i) << " was ["
                                 <<  QString::number(static_cast<int>(mColumns[i].first))
                                 << ", " << QString::number(static_cast<int>(mColumns[i].second))
                                 << "], but was updated to [" << index << ", "
                                 << QString::number(static_cast<int>(mColumns[i].second)) << "].";

                        mColumns[i].first = index;
                        break;
                    }
                }
            }
        }

        mHeaderFormatStyle = io::CSVHelper::HeaderFormatStyle::CUSTOM;
    });

    connect(firstColTypeCmb, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index)
    {
        for (int i = 0; i < mColumnHLayoutItems.size(); ++i)
        {
            auto row = mColumnHLayoutItems.at(i);
            auto columnsContainer = row->layout();
            for (int j = 0; j < columnsContainer->count(); ++j)
            {
                auto labelW = dynamic_cast<QLabel*>(columnsContainer->layout()->itemAt(j)->widget());
                if(labelW != nullptr)
                {
                    if(columnCount == labelW->property("id"))
                    {
                        qDebug() << "Col #" + QString::number(i) << " was ["
                                 <<  QString::number(static_cast<int>(mColumns[i].first))
                                 << ", " << QString::number(static_cast<int>(mColumns[i].second))
                                 << "], but was updated to [" << QString::number(static_cast<int>(mColumns[i].first)) << ", "
                                 << index << "].";

                        mColumns[i].second = index;
                        break;
                    }
                }
            }
        }

        mHeaderFormatStyle = io::CSVHelper::HeaderFormatStyle::CUSTOM;
    });


    mHeaderFormatStyle = io::CSVHelper::HeaderFormatStyle::CUSTOM;
}

void FlightDataFormatDialog::removeColumn(int index)
{
    assert(index == -1 || index < mColumnHLayoutItems.size());

    auto columnsContainer = index == -1 ? mColumnHLayoutItems.back()->layout() : mColumnHLayoutItems.at(index)->layout();

    for (int i = 0; i < columnsContainer->count(); ++i)
        columnsContainer->layout()->itemAt(i)->widget()->deleteLater();

    index == -1 ? mColumnHLayoutItems.removeLast() : mColumnHLayoutItems.remove ( index ) ;

    QTimer::singleShot(50, [=]
    {   adjustSize();   });

    index == -1 ? mColumns.removeLast() : mColumns.remove ( index ) ;

    mColumnCount--;

    for (int i = 0; i < mColumnHLayoutItems.size(); ++i)
    {
        auto row = mColumnHLayoutItems.at(i);
        columnsContainer = row->layout();
        for (int j = 0; j < columnsContainer->count(); ++j)
        {
            auto label = dynamic_cast<QLabel*>(columnsContainer->layout()->itemAt(j)->widget());
            if(label != nullptr)
            {
                if(!label->property("id").isNull())
                {
                    label->setText("[Col" + QString::number(i) + "] ");
                }
            }
        }
    }

    mHeaderFormatStyle = io::CSVHelper::HeaderFormatStyle::CUSTOM;
    return;
}



void FlightDataFormatDialog::setCOTSFlightDataFormat()
{
    auto size = mColumnHLayoutItems.size();
    for (int i = 0; i < size; ++i)
        removeColumn();

    using namespace io::CSVHelper;

    addColumn(data_label::TMSTMP, data_type::DOUBLE);
    addColumn(data_label::ACC_X, data_type::FLOAT);
    addColumn(data_label::PRESS, data_type::INT64);
    addColumn(data_label::ALT_MSL, data_type::DOUBLE);
    addColumn(data_label::TEMP, data_type::FLOAT);
    addColumn(data_label::ACC_Y, data_type::FLOAT);
    addColumn(data_label::ACC_Z, data_type::FLOAT);
    addColumn(data_label::GYRO_X, data_type::FLOAT);
    addColumn(data_label::GYRO_Y, data_type::FLOAT);
    addColumn(data_label::GYRO_Z, data_type::FLOAT);
    addColumn(data_label::MAG_X, data_type::FLOAT);
    addColumn(data_label::MAG_Y, data_type::FLOAT);
    addColumn(data_label::MAG_Z, data_type::FLOAT);
    addColumn(data_label::FLAG_LAUNCH_DETECT, data_type::UINT8);
    addColumn(data_label::FLAG_APOGEE_DETECT, data_type::UINT8);
    addColumn(data_label::FLAG_A_ON, data_type::UINT8);
    addColumn(data_label::FLAG_B_ON, data_type::UINT8);

    mHeaderFormatStyle = HeaderFormatStyle::COTS;;
}


void FlightDataFormatDialog::setSRADFlightDataFormat()
{
    auto size = mColumnHLayoutItems.size();
    for (int i = 0; i < size; ++i)
        removeColumn();

    using namespace io::CSVHelper;

    addColumn(data_label::TMSTMP,data_type::DOUBLE);
    addColumn(data_label::ACC_X, data_type::INT16);
    addColumn(data_label::ACC_Y, data_type::INT16);
    addColumn(data_label::ACC_Z, data_type::INT16);
    addColumn(data_label::PRESS, data_type::INT64);
    addColumn(data_label::TEMP,  data_type::INT64);
    addColumn(data_label::GYRO_X, data_type::INT64);
    addColumn(data_label::GYRO_Y, data_type::INT64);
    addColumn(data_label::GYRO_Z, data_type::INT64);

    mHeaderFormatStyle = HeaderFormatStyle::SRAD;
}



//bool FlightDataFormatDialog::updateColumnList(int index, io::CSVReader::data_type type)
//{
//    bool flag = false;
//    foreach(auto column, mColumns)
//    {
//        if(column.first == label)
//        {
//            flag = true;
//            break;
//        }
//    }
//
//    if(!flag)
//    {
//
//    }
//    return false;
//}

//bool FlightDataFormatDialog::updateColumnList(int index, io::CSVReader::data_label label)
//{
//    bool flag = false;
//    foreach(auto column, mColumns)
//    {
//        if(column.first == label)
//        {
//            flag = true;
//            break;
//        }
//    }
//
//    if(!flag)
//    {
//
//    }
//    return false;
//}
