#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "systeminfo.h"
#include <QGraphicsScene>
#include <QMutex>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    SystemInfo m_systemInfo;

private:
    void draw();
    void resizeEvent(QResizeEvent* event);
    QMutex m_mutex;

public slots:
    void slotTimer();
    
private:
    Ui::MainWindow *ui;
    QTimer m_timer;
    QVector<int> m_cpuUsage;
    QGraphicsScene* m_scene;
};
#endif // MAINWINDOW_H
