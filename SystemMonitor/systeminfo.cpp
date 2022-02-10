#include "systeminfo.h"
#include <QDebug>
#include <QProcess>
#include <QString>
#include <QRegularExpression>
#include <QStorageInfo>
#include <QThread>
#include <QMutexLocker>

SystemInfo::SystemInfo(QObject *parent)
    : QObject{parent}
{
    m_cpuValue = 0;
    this->moveToThread(new QThread);
    connect(this->thread(), &QThread::started, this, &SystemInfo::threadFunc);
    connect(this, &SystemInfo::sigThreadFinished, this->thread(), &QThread::quit, Qt::DirectConnection);
    start();
}

SystemInfo::~SystemInfo()
{
    qDebug() << "SystemInfo bitiriliyor...";
    stop();
    delete this->thread();
    qDebug() << "SystemInfo bitti...";
}

bool SystemInfo::getCpuInfo(float &value)
{
    QStringList list;
    QString cpuexec = "sar";

    QProcess CpuStat;
    QStringList arg;
    arg << "-u" << "1";

    CpuStat.start(cpuexec, arg);
    CpuStat.waitForFinished(1500);

    QString p_stdout = CpuStat.readAllStandardOutput();
    CpuStat.close();

    list = p_stdout.split(QRegularExpression("\\s+"));

    if(list.size() < 18)
        return false;

    QString str = list.at(17);
    str.replace(",", ".");

    bool b = false;
    value = str.toFloat(&b);

    if(!b)
        return false;

    return true;
}

bool SystemInfo::getSdaInfo(qint64 &bytesTotal, qint64 &bytesAvailable, qint64 &bytesFree)
{
    QStorageInfo storage = QStorageInfo::root();
    bytesTotal = storage.bytesTotal();
    bytesAvailable = storage.bytesAvailable();
    bytesFree = storage.bytesFree();
    return true;
}

bool SystemInfo::getMemoryInfo(int &total, int &free, int &used)
{
    QString commonExec = "/proc/meminfo";                            //terminalde ram bilgisini verecek komutlar
    QString totalExec = "/MemTotal/ { print $2 } ";
    QString freeExec = "/MemFree/ { print $2 } ";


    QProcess memInfo;

    memInfo.start("awk", QStringList() << totalExec << commonExec);
    memInfo.waitForFinished();
    QString memoryTot = memInfo.readAllStandardOutput();
    if(memoryTot.isEmpty())
        return false;
    bool b= false;
    total = memoryTot.toInt(&b)/1024;
    if(!b)        
        return false;

    memInfo.start("awk",QStringList() << freeExec << commonExec);
    memInfo.waitForFinished();
    QString memoryAv = memInfo.readAllStandardOutput();
    if(memoryAv.isEmpty())
        return false;

   free = memoryAv.toInt(&b)/1024;
   if(!b)
       return false;

   used = total - free;

   return true;
}

void SystemInfo::start()
{
    this->thread()->start();
}

void SystemInfo::stop()
{
    this->thread()->requestInterruption();
    this->thread()->wait();
}

float SystemInfo::getCpuValue()
{
    QMutexLocker locker(&m_mutex);
    return m_cpuValue;
}

void SystemInfo::setCpuValue(float value)

{
    QMutexLocker locker(&m_mutex);
    m_cpuValue = value;
}

void SystemInfo::threadFunc()
{
    float cpuInfo = 0;
    while(!this->thread()->isInterruptionRequested())
    {
        if(getCpuInfo(cpuInfo))
            setCpuValue(cpuInfo);
    }

    emit sigThreadFinished();
}






