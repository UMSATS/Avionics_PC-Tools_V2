//
// Created by vasil on 14/11/2020.
//

#include "QDirFinder.h"

#include <QtCore/QThreadPool>
#include <QDebug>

QDirFinder::QDirFinder(QObject * _parent): QObject(_parent) {}

void QDirFinder::setPath(QString const &path)
{
    mPath = path;
}

void QDirFinder::setGlobalSearchDepthLevel(int searchDepthLevel)
{
    mDesiredGlobalTargetDepthLevel = searchDepthLevel;
}

void QDirFinder::setGlobalSearchTargetName(const QString &targetName, Qt::CaseSensitivity caseSensitivity)
{
    mGlobalTargetName = targetName;
    mGlobalTargetSensitivity = caseSensitivity;
}

void QDirFinder::setTargetFilters(QDir::Filters filters)
{
    mGlobalTargetFilters = filters | QDir::NoDotAndDotDot;
}

void QDirFinder::setExcludeTargets(const QStringList &excludeTargets)
{
    mExcludeTargets = excludeTargets;
}

void QDirFinder::addExcludeTarget(const QString &excludeTarget)
{
    mExcludeTargets += excludeTarget;
}

void QDirFinder::addSpecialTargets(const QMap<QString, int> &specialTargets)
{
    mSpecialTargets = specialTargets;
}

void QDirFinder::addSpecialTarget(const QString &targetName, int searchDepthLevel)
{
    mSpecialTargets.insert(targetName, searchDepthLevel);
}

void QDirFinder::setSingleResult(bool findOne)
{
    mFindOneOnly = findOne;
}

void QDirFinder::setMatchExactly(bool flag)
{
    isMatchExactly = flag;
}

void QDirFinder::setExtension(const QString &extension)
{
    mExtension = extension;
}

void QDirFinder::interrupt() {
    mIsJobDone = true;
}

void QDirFinder::search()
{
    QDir source(mPath);
    if (!source.exists())
        return;

    auto folders = source.entryList(mGlobalTargetFilters);

    for (int i = 0; i < folders.size(); i++)
    {

        const auto & name = folders.at(i);

        auto fullPathName = (mPath + QDir::separator() + name).toStdString();
        auto fullPathNameQt = QString::fromStdString(fullPathName);

        QThreadPool::globalInstance()->start([=]()
        {
            recursive_search(fullPathNameQt, 0);
        });
    }
}

bool QDirFinder::recursive_search(const QString & currentTarget, int depth)
{
    QDir source ( currentTarget );

    if ( !source.exists() )
        return false;

    for ( const auto & name : source.entryList (mGlobalTargetFilters) )
    {
        auto fullPathName = ( currentTarget + QDir::separator() + name );

        if (isMatchExactly ? ( name == mGlobalTargetName ) : ( name.contains(mGlobalTargetName, mGlobalTargetSensitivity)) )
        {
            if( !mExtension.isEmpty() )
            {
                if( name.contains(mExtension, mGlobalTargetSensitivity) )
                {
                    // qDebug() << "[Thread]: Found - " << fullPathName;
                    emit onResult ( fullPathName );
                    if(mFindOneOnly)
                    {
                        mIsJobDone = true;
                        return true;
                    }
                    else {
                        continue;
                    }
                }
            }
            else
            {
                // qDebug() << "[Thread]: Found - " << fullPathName;
                emit onResult ( fullPathName );
                if(mFindOneOnly)
                {
                    mIsJobDone = true;
                    return true;
                }
                else {
                    continue;
                }
            }
        }
        else
        {
            if(depth == mDesiredGlobalTargetDepthLevel)
            {
                continue;
            }
            else
            {
                if(QFileInfo(fullPathName).isDir())
                {
                    if(mIsJobDone || recursive_search(fullPathName, depth + 1))
                        return true;
                }
                else
                {
                    continue;
                }
            }
        }
    }

    return false;
}


