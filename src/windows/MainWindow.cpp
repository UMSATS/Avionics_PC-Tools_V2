//
// Created by vasil on 02/11/2020.
//

#include "MainWindow.h"
#include <QVBoxLayout>
#include <QtGui/QtGui>
#include <QDockWidget>
#include <iostream>

#include "widgets/TerminalWidget.h"
#include "widgets/SerialPortSelectorWidget.h"
#include "widgets/DebuggingToolsWidget.h"
#include "widgets/FlightEmulationWidget.h"
#include "widgets/OnBoardSensorsTestWidget.h"

MainWindow::MainWindow()
{
    setWindowTitle("UMSATS Avionics Flight-Computer GUI Controller");

    auto sensor_interface = new OnBoardSensorsTestWidget();
    auto terminal         = new TerminalWidget();
    auto emulator         = new FlightEmulationWidget();

    auto confWidget = new QWidget;
    auto confRoot = new QVBoxLayout;
    confWidget->setLayout(confRoot);
    auto serialPortConf = new SerialPortSelectorWidget();
    auto debuggingTools = new DebuggingToolsWidget();
    confRoot->addWidget(serialPortConf);
    confRoot->addWidget(debuggingTools);
    confRoot->addWidget(sensor_interface);
    confRoot->addWidget(emulator);

    auto terminalDock = new QDockWidget(this);
    auto confDock = new QDockWidget(this);

    terminalDock->setWidget(terminal);
    terminalDock->setFloating(false);

    confDock->setWidget(confWidget);
    confDock->setFloating(false);

    addDockWidget(Qt::TopDockWidgetArea, confDock);
    addDockWidget(Qt::BottomDockWidgetArea, terminalDock);

    connect(serialPortConf, &SerialPortSelectorWidget::onDataReceived, [=](QByteArray data, bool isError)
    {
         emit terminal->onBeginResponse(data, isError);
         emit terminal->onEndResponse(true);
        // sensor_interface->parseSensorDataPackage(data);
    });

    connect(terminal, &TerminalWidget::onRequest, [=](const QString & data)
    {
        serialPortConf->writeData(QByteArray::fromStdString(data.toStdString()));
    });

    connect(terminal, &TerminalWidget::resetRemoteTargetCommand, [=]
    {
        if(serialPortConf->isSerialPortOpen())
        {
            debuggingTools->resetRemoteTarget();
        }
        else
        {
            serialPortConf->emitSerialPortError();
        }
    });


    connect(this, &MainWindow::onExitButtonPressed, [=]
    {
        emit debuggingTools->interruptFinders();
    });
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    emit onExitButtonPressed();
    QWidget::closeEvent(event);
}
