#ifndef SYSTEMINFO_H
#define SYSTEMINFO_H
#include <QObject>
#include <QMutex>

class SystemInfo : public QObject
{
    Q_OBJECT
public:
    explicit SystemInfo(QObject *parent = nullptr);
    ~SystemInfo();

    float getCpuValue();
    bool getSdaInfo(qint64 &bytesTotal, qint64 &bytesAvailable, qint64 &bytesFree);
    bool getMemoryInfo(int &total,int &free,int &used);

private:
    float m_cpuValue;
    QMutex m_mutex;

    void start();
    void stop();

    bool getCpuInfo(float &value);

    void setCpuValue(float value);

public slots:
    void threadFunc();

signals:
    void sigThreadFinished();


};

#endif // SYSTEMINFO_H
