//
// Created by vasil on 14/11/2020.
//

#ifndef AVIONICS_PC_TOOLS_GUI_QDIRFINDER_H
#define AVIONICS_PC_TOOLS_GUI_QDIRFINDER_H

#include <QObject>
#include <QString>
#include <QDir>
#include <QMap>

class QDirFinder  : public QObject
{
    Q_OBJECT
public:
    QDirFinder(QObject * _parent = nullptr);
    void setPath(QString const& path);
    void setGlobalSearchDepthLevel(int searchDepthLevel);
    void setGlobalSearchTargetName(const QString & targetName, Qt::CaseSensitivity caseSensitivity);
    void setTargetFilters(QDir::Filters filters);
    void setExcludeTargets(const QStringList & excludeTargets);
    void addExcludeTarget(const QString & excludeTarget);
    void addSpecialTargets(const QMap<QString, int> & specialTargets);
    void addSpecialTarget(const QString & targetName, int searchDepthLevel);
    void setSingleResult(bool findOne);
    void setMatchExactly(bool flag);
    void setExtension(const QString & extension);

    void search();
    void interrupt();

Q_SIGNALS:
    void onResult(const QString & result);

private:
    bool recursive_search(const QString & currentTarget, int depth);

    QString mPath;
    int mDesiredGlobalTargetDepthLevel;
    QString mGlobalTargetName;
    Qt::CaseSensitivity mGlobalTargetSensitivity;
    QDir::Filters mGlobalTargetFilters;
    QStringList mExcludeTargets;
    QString mExtension;
    QMap<QString, int> mSpecialTargets;
    bool mFindOneOnly;
    bool mIsJobDone = false;
    bool isMatchExactly = false;
};



#endif //UNTITLED1_QDIRFINDER_H
