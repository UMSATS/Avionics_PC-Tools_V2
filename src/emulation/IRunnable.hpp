//
// Created by vasil on 15/11/2020.
//

#ifndef AVIONICS_FLIGHT_EMULATOR_RUNNABLE_HPP
#define AVIONICS_FLIGHT_EMULATOR_RUNNABLE_HPP

#include <thread>
#include <iostream>
#include <chrono>
#include <vector>
#include <mutex>
#include <algorithm>
#include <future>
#include <functional>
#include <QRunnable>
#include <QObject>
#include <QtCore/QThreadPool>

class IRunnable : public QObject, public QRunnable
{
Q_OBJECT
public:
    IRunnable()
    {   QRunnable::setAutoDelete(true);     }

    inline void startRunning()
    {
        if( !mIsRunning )
        {
            mIsRunning = true;
            QThreadPool::globalInstance()->start([this]{ run();});
        }
    }

    inline bool isRunning() const
    {
        return mIsRunning;
    }

    inline void stopRunning()
    {
        mIsRunning = false;
    }

    static inline void sleepMillis(int millis)
    {
        QThread::msleep(millis);
    }

signals:
    void finished();
    void started();
protected:
    virtual void run() = 0;
private:
    bool mIsRunning = false;
};

#endif //AVIONICS_FLIGHT_EMULATOR_RUNNABLE_HPP


//class IRunnable : public QRunnable
//{
//public:
//    inline void startRunning()
//    {
//        if( !mIsRunning )
//        {
//            std::lock_guard<std::mutex> lock(mThreadMutex);
//            mWorkers.push_back(std::thread([this]()
//                                           {
//                                               run();
//                                               std::async(&IRunnable::removeWorker, this, std::this_thread::get_id());
//                                           }));
//
//            mIsRunning = true;
//        }
//    }
//
//    [[nodiscard]] inline bool isRunning() const
//    {
//        return mIsRunning;
//    }
//
//    inline void stopRunning()
//    {
//        mIsRunning = false;
//    }
//
//    inline void waitToFinish()
//    {
//        if(!mWorkers.empty())
//        {
//            if( mWorkers.back().joinable())
//                mWorkers.back().join();
//        }
//    }
//
//    static inline void sleepMillis(int millis)
//    {
//        std::this_thread::sleep_for(std::chrono::milliseconds(millis));
//    }
//
//protected:
//    virtual void run() = 0;
//private:
//
//    void removeWorker(std::thread::id id)
//    {
//        std::lock_guard<std::mutex> lock(mThreadMutex);
//        auto iter = std::find_if(mWorkers.begin(), mWorkers.end(), [=](std::thread &t) { return (t.get_id() == id); });
//        if (iter != mWorkers.end())
//        {
//            iter->detach();
//            mWorkers.erase(iter);
//        }
//    }
//
//    bool mIsRunning = false;
//    std::vector<std::thread> mWorkers;
//    std::mutex mThreadMutex;
//};
