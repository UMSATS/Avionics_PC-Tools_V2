//
// Created by vasil on 15/11/2020.
//

#include "DebuggingToolsWidget.h"
#include "utilities/QDirFinder.h"

#include <QVBoxLayout>
#include <QtWidgets/QFileDialog>
#include <QtCore/QDirIterator>
#include <QtCore/QSettings>

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

DebuggingToolsWidget::DebuggingToolsWidget(QWidget *parent) : QGroupBox(parent)
{
    setTitle("Extra ARM Debugging Tools:");

    auto root = new QGridLayout;

    auto specifyArmGDBBtn      = new QPushButton("Select ARM-GDB executable");
    mArmGDBTextEdit           = new QLineEdit("");

    auto specifyOpenOCDExec     = new QPushButton("Select OpenOCD executable");
    mOpenOCDExecTextEdit        = new QLineEdit("");

    auto specifyOpenOCDBoardConfig  = new QPushButton("Select OpenOCD board config file");
    mOpenOCDBoardConfigTextEdit     = new QLineEdit("");

    root->addWidget(specifyArmGDBBtn, 0, 0);
    root->addWidget(mArmGDBTextEdit, 0, 1);

    root->addWidget(specifyOpenOCDExec, 1, 0);
    root->addWidget(mOpenOCDExecTextEdit, 1, 1);

    root->addWidget(specifyOpenOCDBoardConfig, 2, 0);
    root->addWidget(mOpenOCDBoardConfigTextEdit, 2, 1);


    root->setContentsMargins(10, 10, 10, 10);
    setLayout(root);

    connect(mArmGDBTextEdit, &QLineEdit::editingFinished, [=]
    {
        mArmGDBTextEdit->setPlaceholderText("");
        if(mArmGDBTextEdit->text().isEmpty())
            detectArmGdbExecutableFile();
    });

    connect(mOpenOCDExecTextEdit, &QLineEdit::editingFinished, [=]{
        mOpenOCDExecTextEdit->setPlaceholderText("");
        if(mOpenOCDExecTextEdit->text().isEmpty())
            detectOpenOCDExecutableFile();
    });

    connect(mOpenOCDBoardConfigTextEdit, &QLineEdit::editingFinished, [=]{
        mOpenOCDBoardConfigTextEdit->setPlaceholderText("");
        if(mOpenOCDBoardConfigTextEdit->text().isEmpty())
            detectOpenOCDBoardConfigFile();
    });


    connect(specifyArmGDBBtn, &QPushButton::released, [=]
    {
        auto rootDirectory = QDir::homePath();

        if(!mArmGDBTextEdit->text().isEmpty())
        {
            QDir dir (mArmGDBTextEdit->text().left(mArmGDBTextEdit->text().lastIndexOf('\\')));
            if(dir.exists())
            {
                rootDirectory = dir.path();
            }
        }

        if(!mArmGDBTextEdit->placeholderText().isEmpty())
        {
            QDir dir (mArmGDBTextEdit->placeholderText().left(mArmGDBTextEdit->placeholderText().lastIndexOf('\\')));
            if(dir.exists())
            {
                rootDirectory = dir.path();
            }
        }

        auto file = QFileDialog::getOpenFileName(this,
                                                 "Choose ARM-GDB executable...", rootDirectory,
                                                 QString("EXECUTABLE FILE (*.exe)"),
                                                 {}, QFileDialog::DontUseNativeDialog);
        if (file == nullptr)
            return;

        mArmGDBTextEdit->setText(file);
        emit armGDBChanged(file);
    });

    connect(specifyOpenOCDExec, &QPushButton::released, [=]
    {
        auto rootDirectory = QDir::homePath();

        if(!mOpenOCDExecTextEdit->text().isEmpty())
        {
            QDir dir (mOpenOCDExecTextEdit->text().left(mOpenOCDExecTextEdit->text().lastIndexOf('\\')));
            if(dir.exists())
            {
                rootDirectory = dir.path();
            }
        }

        if(!mOpenOCDExecTextEdit->placeholderText().isEmpty())
        {
            QDir dir (mOpenOCDExecTextEdit->placeholderText().left(mOpenOCDExecTextEdit->placeholderText().lastIndexOf('\\')));
            if(dir.exists())
            {
                rootDirectory = dir.path();
            }
        }


        auto file = QFileDialog::getOpenFileName(this,
                                                 "Choose OpenOCD executable...", rootDirectory,
                                                 QString("EXECUTABLE FILE (*.exe)"),
                                                 {}, QFileDialog::DontUseNativeDialog);

        if (file == nullptr)
            return;

        mOpenOCDExecTextEdit->setText(file);
        emit openOCDExecutableFileChanged(file);
    });

    connect(specifyOpenOCDBoardConfig, &QPushButton::released, [=]
    {
        auto rootDirectory = QDir::homePath();

        if(!mOpenOCDBoardConfigTextEdit->text().isEmpty())
        {
            QDir dir (mOpenOCDBoardConfigTextEdit->text().left(mOpenOCDBoardConfigTextEdit->text().lastIndexOf('\\')));
            if(dir.exists())
            {
                rootDirectory = dir.path();
            }
        }

        if(!mOpenOCDBoardConfigTextEdit->placeholderText().isEmpty())
        {
            QDir dir (mOpenOCDBoardConfigTextEdit->placeholderText().left(mOpenOCDBoardConfigTextEdit->placeholderText().lastIndexOf('\\')));
            if(dir.exists())
            {
                rootDirectory = dir.path();
            }
        }

        auto file = QFileDialog::getOpenFileName(this,
                                                 "Choose OpenOCD board config file...", rootDirectory,
                                                 QString("CONFIG FILE (*.cfg)"),
                                                 {}, QFileDialog::DontUseNativeDialog);

        if (file == nullptr)
            return;

        mOpenOCDBoardConfigTextEdit->setText(file);
        emit openOCDBoardConfigFileChanged(file);
    });

    detectArmGdbExecutableFile();
    detectOpenOCDExecutableFile();
    detectOpenOCDBoardConfigFile();
}


void DebuggingToolsWidget::detectOpenOCDExecutableFile()
{
#ifdef __linux__
    static const char * OPENOCD_EXECUTABLE = "openocd";
    static const char * SYSTEM_PATH = "/";
#elif _WIN32
    static const char * OPENOCD_EXECUTABLE = "openocd.exe";
    static const char * SYSTEM_PATH = "C://";
#endif

    QTimer::singleShot(0, [this]
    {
        QSettings mySettings (QSettings::IniFormat, QSettings::UserScope, "UMSATS", "AVIONICS_V3");
        if( ! mySettings.value("mOpenOCDExecTextEdit").isNull ( ) )
        {
            mOpenOCDExecTextEdit->setPlaceholderText( mySettings.value ( "mOpenOCDExecTextEdit" ).toString () );
        }
        else
        {
            mOpenOCDExecTextEdit->setPlaceholderText ( "Detecting..." );

            auto * finder = new QDirFinder ( this );
            finder->setGlobalSearchTargetName ( OPENOCD_EXECUTABLE, Qt::CaseSensitivity::CaseSensitive );
            finder->setMatchExactly ( true );
            finder->setSingleResult ( true );

            finder->setPath ( SYSTEM_PATH );
            finder->setTargetFilters ( QDir::Files | QDir::Dirs );
            finder->setGlobalSearchDepthLevel ( 10 );
            finder->search ( );

            connect ( finder, &QDirFinder::onResult, [this] ( const QString & result )
            {
                mOpenOCDExecTextEdit->setPlaceholderText ( result );
                emit openOCDExecutableFileChanged ( result );

                QSettings mySettings (QSettings::IniFormat, QSettings::UserScope, "UMSATS", "AVIONICS_V3");
                mySettings.setValue("mOpenOCDExecTextEdit", result);
            } );

            connect ( this, &DebuggingToolsWidget::interruptFinders, finder, &QDirFinder::interrupt );
        }
    });
}

void DebuggingToolsWidget::detectOpenOCDBoardConfigFile()
{
#ifdef __linux__
    static const char * SYSTEM_PATH = "/";
#elif _WIN32
    static const char * SYSTEM_PATH = "C://";
#endif

    static const char * OPENOCD_BOARD_CONFIG_FILE = "stm32f4discovery.cfg";

    QTimer::singleShot(0, [this]
    {
        QSettings mySettings (QSettings::IniFormat, QSettings::UserScope, "UMSATS", "AVIONICS_V3");
        if( ! mySettings.value("mOpenOCDBoardConfigTextEdit").isNull ( ) )
        {
            mOpenOCDBoardConfigTextEdit->setPlaceholderText ( mySettings.value ( "mOpenOCDBoardConfigTextEdit" ).toString () );
        }
        else
        {
            mOpenOCDBoardConfigTextEdit->setPlaceholderText ( "Detecting..." );

            auto * finder = new QDirFinder ( this );
            finder->setGlobalSearchTargetName ( OPENOCD_BOARD_CONFIG_FILE, Qt::CaseSensitivity::CaseSensitive );
            finder->setMatchExactly ( true );
            finder->setSingleResult ( true );

            finder->setPath ( SYSTEM_PATH );
            finder->setTargetFilters ( QDir::Files | QDir::Dirs );
            finder->setGlobalSearchDepthLevel ( 10 );
            finder->search ( );


            connect ( finder, &QDirFinder::onResult, [ this ] ( const QString & result )
            {
                mOpenOCDBoardConfigTextEdit->setPlaceholderText ( result );
                emit openOCDBoardConfigFileChanged ( result );

                QSettings mySettings (QSettings::IniFormat, QSettings::UserScope, "UMSATS", "AVIONICS_V3");
                mySettings.setValue("mOpenOCDBoardConfigTextEdit", result);
            } );

            connect ( this, &DebuggingToolsWidget::interruptFinders, finder, &QDirFinder::interrupt );
        }
    });
}

void DebuggingToolsWidget::detectArmGdbExecutableFile()
{
#ifdef __linux__
    static const char * ARM_GDB_EXECUTABLE = "arm-atollic-eabi-gdb";
    static const char * SYSTEM_PATH = "/";
#elif _WIN32
    static const char * ARM_GDB_EXECUTABLE = "arm-atollic-eabi-gdb.exe";
    static const char * SYSTEM_PATH = "C://";
#endif

    QTimer::singleShot(0, [this]
    {
        QSettings mySettings (QSettings::IniFormat, QSettings::UserScope, "UMSATS", "AVIONICS_V3");
        if( ! mySettings.value("mArmGDBTextEdit").isNull ( ) )
        {
            mArmGDBTextEdit->setPlaceholderText( mySettings.value ( "mArmGDBTextEdit" ).toString () );
        }
        else
        {
            mArmGDBTextEdit->setPlaceholderText ( "Detecting..." );

            auto * finder = new QDirFinder ( this );
            finder->setGlobalSearchTargetName ( ARM_GDB_EXECUTABLE, Qt::CaseSensitivity::CaseSensitive );
            finder->setMatchExactly ( true );
            finder->setSingleResult ( true );

            finder->setPath ( SYSTEM_PATH );
            finder->setTargetFilters ( QDir::Files | QDir::Dirs );
            finder->setGlobalSearchDepthLevel ( 10 );
            finder->search ( );

            connect ( finder, &QDirFinder::onResult, [ this ] ( const QString & result )
            {
                mArmGDBTextEdit->setPlaceholderText ( result );
                emit armGDBChanged ( result );

                QSettings mySettings (QSettings::IniFormat, QSettings::UserScope, "UMSATS", "AVIONICS_V3");
                mySettings.setValue("mArmGDBTextEdit", result);
            } );

            connect ( this, &DebuggingToolsWidget::interruptFinders, finder, &QDirFinder::interrupt );
        }
    });
}

void DebuggingToolsWidget::resetRemoteTarget()
{
    auto mOpenOCDProcess = new QProcess();
    mOpenOCDProcess->setProgram(mOpenOCDExecTextEdit->text().isEmpty() ? mOpenOCDExecTextEdit->placeholderText() : mOpenOCDExecTextEdit->text());
    mOpenOCDProcess->setNativeArguments("-f "  +  (mOpenOCDBoardConfigTextEdit->text().isEmpty() ? mOpenOCDBoardConfigTextEdit->placeholderText() : mOpenOCDBoardConfigTextEdit->text()));

    connect(mOpenOCDProcess, &QProcess::readyReadStandardError, [=] {
        std::cerr << mOpenOCDProcess->readAllStandardError().toStdString() << std::flush;
    });

    connect(mOpenOCDProcess, &QProcess::readyReadStandardOutput, [=] {
        std::cout <<  mOpenOCDProcess->readAllStandardOutput().toStdString() << std::flush;
    });

    connect(mOpenOCDProcess, &QProcess::started, [=] {
        std::cout << "OpenOCD has been started!" << std::endl;
    });

    connect(mOpenOCDProcess, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), [=] {
        std::cout << "OpenOCD has finished!" << std::endl;
    });


    if(!mOpenOCDProcess->isOpen())
    {
        mOpenOCDProcess->start();
    }


    auto mGDBProcess = new QProcess();
    mGDBProcess->setProgram(mArmGDBTextEdit->text().isEmpty() ? mArmGDBTextEdit->placeholderText() : mArmGDBTextEdit->text());
    mGDBProcess->setNativeArguments("-iex \"target remote tcp:localhost:3333\" -iex \"monitor reset\" -iex \"disconnect\" -iex \"quit\"");

    qDebug().noquote() << "gdb: "<< mGDBProcess->nativeArguments();

    connect(mGDBProcess, &QProcess::readyReadStandardError, [=] {
        std::cerr << mOpenOCDProcess->readAllStandardError().toStdString() << std::flush;
    });

    connect(mGDBProcess, &QProcess::readyReadStandardOutput, [=] {
        std::cout << mOpenOCDProcess->readAllStandardOutput().toStdString() << std::flush;
    });

    connect(mGDBProcess, &QProcess::started, [=] {
        std::cout <<  "GDB has been started!" << std::endl;
    });

    connect(mGDBProcess, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), [=] {
        std::cout << "GDB has finished!" << std::endl;
        mOpenOCDProcess->kill();
    });

    if(!mGDBProcess->isOpen())
    {
        mGDBProcess->start();
    }
}

void DebuggingToolsWidget::uploadToRemoteTarget(const QString & executable)
{
    auto mOpenOCDProcess = new QProcess();
    mOpenOCDProcess->setProgram(mOpenOCDExecTextEdit->text().isEmpty() ? mOpenOCDExecTextEdit->placeholderText() : mOpenOCDExecTextEdit->text());
    mOpenOCDProcess->setNativeArguments("-f "  +  (mOpenOCDBoardConfigTextEdit->text().isEmpty() ? mOpenOCDBoardConfigTextEdit->placeholderText() : mOpenOCDBoardConfigTextEdit->text()));

    connect(mOpenOCDProcess, &QProcess::readyReadStandardError, [=] {
        std::cerr << mOpenOCDProcess->readAllStandardError().toStdString() << std::flush;
    });

    connect(mOpenOCDProcess, &QProcess::readyReadStandardOutput, [=] {
        std::cout <<  mOpenOCDProcess->readAllStandardOutput().toStdString() << std::flush;
    });

    connect(mOpenOCDProcess, &QProcess::started, [=] {
        std::cout << "OpenOCD has been started!" << std::endl;
    });

    connect(mOpenOCDProcess, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), [=] {
        std::cout << "OpenOCD has finished!" << std::endl;
    });


    if(!mOpenOCDProcess->isOpen())
    {
        mOpenOCDProcess->start();
    }


    auto mGDBProcess = new QProcess();
    mGDBProcess->setProgram(mArmGDBTextEdit->text().isEmpty() ? mArmGDBTextEdit->placeholderText() : mArmGDBTextEdit->text());
    mGDBProcess->setNativeArguments(QString("-iex \"target remote tcp:localhost:3333\" -iex \"monitor program ") + executable + QString("\" -iex \"monitor reset init\" -iex \"disconnect\" -iex \"quit\""));

    qDebug().noquote() << "gdb: "<< mGDBProcess->nativeArguments();

    connect(mGDBProcess, &QProcess::readyReadStandardError, [=] {
        std::cerr << mOpenOCDProcess->readAllStandardError().toStdString() << std::flush;
    });

    connect(mGDBProcess, &QProcess::readyReadStandardOutput, [=] {
        std::cout << mOpenOCDProcess->readAllStandardOutput().toStdString() << std::flush;
    });

    connect(mGDBProcess, &QProcess::started, [=] {
        std::cout <<  "GDB has been started!" << std::endl;
    });

    connect(mGDBProcess, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), [=] {
        std::cout << "GDB has finished!" << std::endl;
        mOpenOCDProcess->kill();
    });

    if(!mGDBProcess->isOpen())
    {
        mGDBProcess->start();
    }
}
