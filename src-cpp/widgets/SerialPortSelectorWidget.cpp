//
// Created by vasil on 13/11/2020.
//

#include "SerialPortSelectorWidget.h"
#include <QLabel>
#include <QComboBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>

#include <QTimer>
#include <QSet>
#include <QtCore/QThreadPool>
#include <QDebug>
#include <iostream>
#include <QGridLayout>
#include <QDebug>


void Worker::run ( )
{
    isRunning = true;
    while ( isRunning )
    {
        while(mSerialBackend.isOpen ())
        {
            while ( mSerialBackend.available () > 0 )
            {
                auto bytes_read = mSerialBackend.readAll(mBuffer);
                emit onDataReady(QByteArray::fromRawData((char*)mBuffer, bytes_read));
                memset(mBuffer, 0, 2048);
            }
        }
    }

    emit finished();
}

SerialPortSelectorWidget::SerialPortSelectorWidget(QWidget *parent): QGroupBox(parent)
{
    setTitle("Serial Port:");

    auto root = new QHBoxLayout;
    auto left_root = new QGridLayout;
    auto right_root = new QGridLayout;

    auto portLbl = new QLabel("Port:");
    auto portCmb = new QComboBox();
    left_root->addWidget(portLbl, 0, 0);
    left_root->addWidget(portCmb, 0 ,1);

    auto descriptionLbl = new QLabel("Description:");
    auto descriptionVLbl = new QLabel();

    left_root->addWidget(descriptionLbl, 1, 0);
    left_root->addWidget(descriptionVLbl, 1, 1);

    auto manufacturerLbl = new QLabel("Manufacturer:");
    auto manufacturerVLbl = new QLabel();

    left_root->addWidget(manufacturerLbl, 2, 0);
    left_root->addWidget(manufacturerVLbl, 2, 1);

//    auto serialNoLbl = new QLabel("Serial #:");
//    auto serialNoVLbl = new QLabel();

//    left_root->addWidget(serialNoLbl, 3, 0);
//    left_root->addWidget(serialNoVLbl, 3 ,1);

//    auto locationLbl = new QLabel("System Location:");
//    auto locationVLbl = new QLabel();

//    left_root->addWidget(locationLbl, 4, 0);
//    left_root->addWidget(locationVLbl, 4 ,1);

    auto VIDLbl = new QLabel("VID/PID/Manufacturer/Vendor:");
    auto VID_VLbl = new QLabel();

//    left_root->addWidget(VIDLbl, 5 , 0);
//    left_root->addWidget(VID_VLbl, 5, 1);

//    auto PIDLbl = new QLabel("PID:");
//    auto PID_VLbl = new QLabel();

//    left_root->addWidget(PIDLbl, 6, 0);
//    left_root->addWidget(PID_VLbl, 6, 1);

    left_root->setContentsMargins(10,10,10,10);


    auto baudRateLbl = new QLabel("Baud Rate:");
    auto baudRateCmb = new QComboBox();
    baudRateLbl->setMinimumWidth(200);
    baudRateCmb->setMinimumWidth(100);

    right_root->addWidget(baudRateLbl, 0 , 0);
    right_root->addWidget(baudRateCmb, 0, 1);

    auto bitsLbl = new QLabel("Bits:");
    auto bitsCmb = new QComboBox();
    bitsLbl->setMinimumWidth(200);
    bitsCmb->setMinimumWidth(100);

    right_root->addWidget(bitsLbl, 1, 0);
    right_root->addWidget(bitsCmb, 1, 1);

    auto parityLbl = new QLabel("Parity:");
    auto parityCmb = new QComboBox();
    parityLbl->setMinimumWidth(200);
    parityCmb->setMinimumWidth(100);

    right_root->addWidget(parityLbl, 2, 0);
    right_root->addWidget(parityCmb, 2, 1);

    auto stopBitsLbl = new QLabel("Stop Bits:");
    auto stopBitsCmb = new QComboBox();
    stopBitsLbl->setMinimumWidth(200);
    stopBitsCmb->setMinimumWidth(100);

    right_root->addWidget(stopBitsLbl, 3, 0);
    right_root->addWidget(stopBitsCmb, 3, 1);

    auto flowControlLbl = new QLabel("Flow Control:");
    auto flowControlCmb = new QComboBox();
    flowControlLbl->setMinimumWidth(200);
    flowControlCmb->setMinimumWidth(100);

    right_root->addWidget(flowControlLbl, 4, 0);
    right_root->addWidget(flowControlCmb, 4, 1);

    auto connectButton = new QPushButton("CONNECT");
    right_root->addWidget(connectButton, 5, 1);

    right_root->setContentsMargins(10,10,10,10);

    root->addLayout(left_root);
    root->addSpacing(10);
    root->addLayout(right_root);
    setLayout(root);

    adjustSize();

    connect(portCmb, &QComboBox::currentTextChanged, [=](const QString & port)
    {
        auto availablePorts = serial::list_ports();

        auto iter = availablePorts.begin();

        while( iter != availablePorts.end() )
        {
            std::string device = *iter++;

            if (port == device.c_str())
            {
                descriptionVLbl->setText("N/A");
                manufacturerVLbl->setText("N/A");
                VID_VLbl->setText("N/A");
                break;
            }


        }
    });

    static auto enablePortConfigs = [=](bool enable)
    {
        baudRateCmb->setEnabled(enable);
        bitsCmb->setEnabled(enable);
        parityCmb->setEnabled(enable);
        stopBitsCmb->setEnabled(enable);
        flowControlCmb->setEnabled(enable);
        portCmb->setEnabled(enable);
    };

    static auto configurePorts = [=]
    {
        if(!mSerialBackend.isOpen())
        {
            switch (baudRateCmb->currentIndex())
            {
                case 0:
                    mSerialBackend.setBaudRate ( serial::baudrate_t::baudrate_0 );
                    break;
                case 1:
                    mSerialBackend.setBaudRate ( serial::baudrate_t::baudrate_50 );
                    break;
                case 2:
                    mSerialBackend.setBaudRate ( serial::baudrate_t::baudrate_100 );
                    break;
                case 3:
                    mSerialBackend.setBaudRate ( serial::baudrate_t::baudrate_110 );
                    break;
                case 4:
                    mSerialBackend.setBaudRate ( serial::baudrate_t::baudrate_134 );
                    break;
                case 5:
                    mSerialBackend.setBaudRate ( serial::baudrate_t::baudrate_150 );
                    break;
                case 6:
                    mSerialBackend.setBaudRate ( serial::baudrate_t::baudrate_200 );
                    break;
                case 7:
                    mSerialBackend.setBaudRate ( serial::baudrate_t::baudrate_300 );
                    break;
                case 8:
                    mSerialBackend.setBaudRate ( serial::baudrate_t::baudrate_600 );
                    break;
                case 9:
                    mSerialBackend.setBaudRate ( serial::baudrate_t::baudrate_1200 );
                    break;
                case 10:
                    mSerialBackend.setBaudRate ( serial::baudrate_t::baudrate_1800 );
                    break;
                case 11:
                    mSerialBackend.setBaudRate ( serial::baudrate_t::baudrate_2400 );
                    break;
                case 12:
                    mSerialBackend.setBaudRate ( serial::baudrate_t::baudrate_4800 );
                    break;
                case 13:
                    mSerialBackend.setBaudRate ( serial::baudrate_t::baudrate_9600 );
                    break;
                case 14:
                    mSerialBackend.setBaudRate ( serial::baudrate_t::baudrate_19200 );
                    break;
                case 15:
                    mSerialBackend.setBaudRate ( serial::baudrate_t::baudrate_38400 );
                    break;
                case 16:
                    mSerialBackend.setBaudRate ( serial::baudrate_t::baudrate_115200 );
                    break;
                case 17:
                    mSerialBackend.setBaudRate ( serial::baudrate_t::baudrate_4800 );
                    break;
                default:
                    break;
            }


            switch (bitsCmb->currentIndex())
            {
                case 0:
                    mSerialBackend.setDataBits ( serial::bytesize_t::fivebits );
                    break;
                case 1:
                    mSerialBackend.setDataBits ( serial::bytesize_t::sixbits );
                    break;
                case 2:
                    mSerialBackend.setDataBits ( serial::bytesize_t::sevenbits );
                    break;
                case 3:
                    mSerialBackend.setDataBits ( serial::bytesize_t::eightbits );
                    break;
                default:
                    break;
            }

            switch (stopBitsCmb->currentIndex())
            {
                case 0:
                    mSerialBackend.setStopBits ( serial::stopbits_t ::stopbits_one );
                    break;
                case 1:
                    mSerialBackend.setStopBits ( serial::stopbits_t::stopbits_one_point_five );
                    break;
                case 2:
                    mSerialBackend.setStopBits(serial::stopbits_t::stopbits_two );
                    break;
                default:
                    break;
            }

            switch (parityCmb->currentIndex())
            {
                case 0:
                    mSerialBackend.setParity(serial::parity_t::parity_none );
                    break;
                case 1:
                    mSerialBackend.setParity(serial::parity_t::parity_even );
                    break;
                case 2:
                    mSerialBackend.setParity(serial::parity_t::parity_odd );
                    break;
                case 3:
                    mSerialBackend.setParity(serial::parity_t::parity_space );
                    break;
                case 4:
                    mSerialBackend.setParity(serial::parity_t::parity_mark );
                    break;
                default:
                    break;
            }

            switch (flowControlCmb->currentIndex())
            {
                case 0:
                    mSerialBackend.setFlowControl ( serial::flowcontrol_t::flowcontrol_none );
                    break;
                case 1:
                    mSerialBackend.setFlowControl ( serial::flowcontrol_t::flowcontrol_hardware );
                    break;
                case 2:
                    mSerialBackend.setFlowControl ( serial::flowcontrol_t::flowcontrol_software );
                    break;
                default:
                    break;
            }
        }
    };

    connect(connectButton, &QPushButton::released, [=]
    {
        if(!mSerialBackend.isOpen())
        {
            mSerialBackend.setPortName(portCmb->currentText().toStdString());

            configurePorts();

            mSerialBackend.open();
            if (mSerialBackend.isOpen())
            {
                connectButton->setText("DISCONNECT");
                enablePortConfigs(false);
                // emit onDataReceived(QByteArray::fromStdString("Info: Connected to \"" + portCmb->currentText().toStdString() + "\""), true);
            }
        }
        else
        {
            mSerialBackend.close();
            if(!mSerialBackend.isOpen())
            {
                connectButton->setText("CONNECT");
                enablePortConfigs(true);
                // emit onDataReceived(QByteArray::fromStdString("Info: Disconnected from \"" + portCmb->currentText().toStdString() + "\""), true);
            }
        }
    });

    mWorker = new Worker ( mSerialBackend );
    QThreadPool::globalInstance()->start ( mWorker );

    connect ( mWorker, &Worker::onDataReady, [=] ( const QByteArray & data )
    {
        emit onDataReceived (data );
    });

    for (auto const & port : serial::list_ports())
        portCmb->addItem(QString::fromStdString ( port ) );

    baudRateCmb->addItems({"75", "110", "134", "150", "300", "600", "1200", "1800", "2400", "4800", "7200", "9600", "14400", "19200", "38400", "57600", "115200", "128000"});
    baudRateCmb->setCurrentIndex(16);

    bitsCmb->addItems({"5", "6", "7", "8"});
    bitsCmb->setCurrentIndex(3);

    parityCmb->addItems({"None", "Even", "Odd", "Space", "Mark"});
    parityCmb->setCurrentIndex(0);

    stopBitsCmb->addItems({"1", "1.5", "2"});
    stopBitsCmb->setCurrentIndex(0);

    flowControlCmb->addItems({"None", "Hardware", "Software"});
    flowControlCmb->setCurrentIndex(0);
}

void SerialPortSelectorWidget::writeData(const QByteArray &data)
{
    if(isSerialPortOpen())
    {
        mSerialBackend.write(data.toStdString() + "\r");
        mSerialBackend.flush();
    }
    else
    {
        emitSerialPortError();
    }
}

SerialPortSelectorWidget::~SerialPortSelectorWidget()
{
    mWorker->stop ();
}

bool SerialPortSelectorWidget::isSerialPortOpen() const
{
    return mSerialBackend.isOpen();
}

void SerialPortSelectorWidget::emitSerialPortError()
{
    emit onDataReceived(QByteArray::fromStdString("Error: No serial port is open! Abort."), true);
}



