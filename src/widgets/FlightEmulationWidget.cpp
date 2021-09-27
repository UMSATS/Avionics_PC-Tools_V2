//
// Created by vasil on 15/11/2020.
//

#include "FlightEmulationWidget.h"

#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QTimer>
#include <QtWidgets/QFileDialog>
#include <QDebug>
#include <QProgressBar>
#include <QLabel>
#include <QtCore/QSettings>
#include "utilities/QDirFinder.h"


FlightEmulationWidget::FlightEmulationWidget(QWidget *_parent) : QGroupBox(_parent), mEmulator{}
{
    mDialog = std::make_unique<FlightDataFormatDialog>();
    mChartWindow = std::make_unique<ChartDialogUtils>("Flight Data Emulation Graph");
    setTitle("Emulation");

    auto root = new QGridLayout;

    auto specifyFlightDataFile = new QPushButton("Select Flight Data File");
    mFlightDataFile            = new QLineEdit("");
    mEmulateBtn                = new QPushButton("Start Emulation");
    mEmulationProgress         = new QProgressBar();
    mSpecifyFlightDataFormat   = new QPushButton("Specify Flight Data Format");
    auto flightStatusLbl       = new QLabel("Flight State: ");
    mFlightStatusVLbl          = new QLabel("N/A");
    auto currAltLbl            = new QLabel("Altitude: ");
    mCurrAltVLbl               = new QLabel("N/A");

    auto container = new QHBoxLayout;
    container->addWidget(mFlightDataFile);
    container->addWidget(mSpecifyFlightDataFormat);

    root->addWidget(specifyFlightDataFile, 0, 0);
    root->addLayout(container, 0, 1);
    root->addWidget(mEmulateBtn, 1, 0);
    root->addWidget(mEmulationProgress, 1, 1);
    root->addWidget(flightStatusLbl, 2, 0);
    root->addWidget(mFlightStatusVLbl, 2, 1);
    root->addWidget(currAltLbl, 3, 0);
    root->addWidget(mCurrAltVLbl, 3, 1);

    root->setContentsMargins(10, 10, 10, 10);
    setLayout(root);

    mEmulationProgress->setMaximum(100);
    mEmulationProgress->setValue(0);

    mEmulateBtn->setEnabled(false);

    detectFlightDataFile();

    connect(mFlightDataFile, &QLineEdit::editingFinished, [=]
    {
        mFlightDataFile->setPlaceholderText("");
        if(mFlightDataFile->text().isEmpty())
            detectFlightDataFile();
    });

    connect(mSpecifyFlightDataFormat, &QPushButton::released, [=]
    {
        mDialog->open();
    });

    connect(mDialog.get(), &FlightDataFormatDialog::onFlightDataFormatChanged, &mEmulator, &EmulationController::setFlightDataHeaderFormat);

    connect(specifyFlightDataFile, &QPushButton::released, [=]
    {
        auto rootDirectory = QDir::homePath();

        if(!mFlightDataFile->text().isEmpty())
        {
            QDir dir (mFlightDataFile->text().left(mFlightDataFile->text().lastIndexOf('\\')));
            if(dir.exists())
            {
                rootDirectory = dir.path();
            }
        }

        if(!mFlightDataFile->placeholderText().isEmpty())
        {
            QDir dir (mFlightDataFile->placeholderText().left(mFlightDataFile->placeholderText().lastIndexOf('\\')));
            if(dir.exists())
            {
                rootDirectory = dir.path();
            }
        }

        auto file = QFileDialog::getOpenFileName(this,
                                                 "Choose OpenOCD executable...", rootDirectory,
                                                 QString("Flight Data FILE (*.csv)"),
                                                 {}, QFileDialog::DontUseNativeDialog);

        if (file == nullptr)
            return;

        mEmulateBtn->setEnabled(true);
        mFlightDataFile->setText(file);
        emit flightDataFileChanged(file);
    });

    connect(mEmulateBtn, &QPushButton::released, [=]
    {
        if(!mFlightDataFile->text().isEmpty())
        {
            !isEmulationRunning() ? startEmulation(mFlightDataFile->text()) : stopEmulation();
            return;
        }

        if(!mFlightDataFile->placeholderText().isEmpty())
        {
            !isEmulationRunning() ? startEmulation(mFlightDataFile->placeholderText()) : stopEmulation();
            return;
        }
    });

    connect(&mEmulator, &EmulationController::finished, this, [=]
    {
        mCurrAltVLbl->setText("N/A");
        mFlightStatusVLbl->setText("N/A");
        mEmulateBtn->setText("Start Emulation");
        mEmulationProgress->setValue(0);
    }, Qt::QueuedConnection);

    connect(&mEmulator, &EmulationController::started, this, [=]
    {
        mEmulateBtn->setText("Stop Emulation");
    }, Qt::QueuedConnection);

    connect(&mEmulator, &EmulationController::onFlightProgressChanged, this, [=](int progress, float altitude, float timestamp)
    {
        if(!mEmulator.isRunning())
            return;

         mCurrAltVLbl->setText(QString::number(altitude) + " m");
         mEmulationProgress->setValue(progress);

        if(progress == 100)
            stopEmulation();

        mChartWindow->addAltitudeData(altitude, timestamp);

    }, Qt::QueuedConnection);


    connect(&mEmulator, &EmulationController::onFlightStateChanged, [=](FlightEventStatus state, float currentAltitude, float timestamp)
    {
        mChartWindow->addEventData(currentAltitude, timestamp);
        switch(state)
        {
            case Launchpad:
                mFlightStatusVLbl->setText("Launchpad");
                break;
            case PreApogee:
                mFlightStatusVLbl->setText("PreApogee");
                break;
            case Apogee:
                mFlightStatusVLbl->setText("Apogee");
                break;
            case PostApogee:
                mFlightStatusVLbl->setText("PostApogee");
                break;
            case MainChute:
                mFlightStatusVLbl->setText("MainChute");
                break;
            case PostMain:
                mFlightStatusVLbl->setText("PostMain");
                break;
            case Landed:
                mFlightStatusVLbl->setText("Landed");
                break;
            case Exited:
                mFlightStatusVLbl->setText("Exited");
                break;
        }
    });
}

bool FlightEmulationWidget::isEmulationRunning() const
{
    return mEmulator.isRunning();
}

void FlightEmulationWidget::detectFlightDataFile()
{
    static const char * FLIGHT_DATA_FILE_NAME = "srad";
    static const char * FLIGHT_DATA_FILE_EXT  = ".csv";

#ifdef __linux__
    static const char * SYSTEM_PATH = "/";
    QTimer::singleShot(0, [=]
    {
        mFlightDataFile->setPlaceholderText("Detecting...");

        auto *finder = new QDirFinder(this);
        finder->setGlobalSearchTargetName(FLIGHT_DATA_FILE_NAME, Qt::CaseSensitivity::CaseSensitive);
        finder->setExtension(FLIGHT_DATA_FILE_EXT);
        finder->setMatchExactly(false);
        finder->setSingleResult(true);
        finder->setPath(SYSTEM_PATH);
        finder->setTargetFilters(QDir::Files | QDir::Dirs);
        finder->setGlobalSearchDepthLevel(15);
        finder->search();

        connect(finder, &QDirFinder::onResult, [=](const QString &result)
        {
            mFlightDataFile->setPlaceholderText(result);
            emit flightDataFileChanged(result);
        });
    });
#elif _WIN32

    for(const auto & drive : QDir::drives())
    {
        QTimer::singleShot(0, [=]
        {
            QSettings mySettings (QSettings::IniFormat, QSettings::UserScope, "UMSATS", "AVIONICS_V3");
            if( ! mySettings.value("mFlightDataFile").isNull ( ) )
            {
                mFlightDataFile->setPlaceholderText( mySettings.value ( "mFlightDataFile" ).toString () );
            }
            else
            {
                mFlightDataFile->setPlaceholderText ( "Searching..." );
                auto * finder = new QDirFinder ( this );
                finder->setGlobalSearchTargetName ( FLIGHT_DATA_FILE_NAME, Qt::CaseSensitivity::CaseSensitive );
                finder->setExtension ( FLIGHT_DATA_FILE_EXT );
                finder->setMatchExactly ( false );
                finder->setSingleResult ( true );
                finder->setPath ( drive.path ( ) );
                finder->setTargetFilters ( QDir::Files | QDir::Dirs );
                finder->setGlobalSearchDepthLevel ( 15 );
                finder->search ( );

                connect ( finder, &QDirFinder::onResult, [ = ] ( const QString & result )
                {
                    mFlightDataFile->setPlaceholderText ( result );
                    emit flightDataFileChanged ( result );
                    mEmulateBtn->setEnabled ( true );

                    QSettings mySettings (QSettings::IniFormat, QSettings::UserScope, "UMSATS", "AVIONICS_V3");
                    mySettings.setValue("mFlightDataFile", result);
                } );
            }
        });
    }

#endif
}

void FlightEmulationWidget::stopEmulation()
{
    mEmulator.stopRunning();
    mChartWindow->hide();
}

void FlightEmulationWidget::startEmulation(const QString & file)
{
    if (file.isEmpty())
        return;

    mChartWindow->clear();
    QPointF xAxis(io::CSVHelper::getSmallestValueAt(file, 0).toFloat(), io::CSVHelper::getBiggestValueAt(file, 0).toFloat());
    QPointF yAxis1(io::CSVHelper::getSmallestValueAt(file, 2).toFloat(), io::CSVHelper::getBiggestValueAt(file, 2).toFloat());
    QPointF yAxis2(io::CSVHelper::getSmallestValueAt(file, 7).toFloat(), io::CSVHelper::getBiggestValueAt(file, 7).toFloat());
    QPointF yAxis3;

    auto minPress= qMax(yAxis1.x(), yAxis2.x());
    auto maxPress= qMax(yAxis1.y(), yAxis2.y());

    auto minAlt = EventDetector::calculate_altitude(maxPress>100000?maxPress/100:maxPress);
    auto maxAlt = EventDetector::calculate_altitude(minPress>100000?minPress/100:minPress);

    yAxis3.setX(0);
    yAxis3.setY(maxAlt-minAlt);

    mChartWindow->rescale(xAxis, yAxis3);

    if( mEmulator.isRunning())
        return;

    mEmulator.setFlightData(file);
    mEmulator.startRunning();

    mChartWindow->show();
}
