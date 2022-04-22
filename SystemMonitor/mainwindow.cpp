#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QImage>
#include <QPainter>
#include <QGraphicsPixmapItem>
#include <QPen>
#include <QWindow>
#include <QWidget>
#include <QMutexLocker>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(&m_timer, SIGNAL(timeout()), this, SLOT(slotTimer()));
    m_timer.start(1000);
    m_scene = new QGraphicsScene();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_scene;
}

void MainWindow::draw()
{
    QMutexLocker locker(&m_mutex);

    if(m_cpuUsage.count() == 0)
        return;

    int graphSize = 60;
    
    QSize sizeofchart = ui->graphicsView->size();
    QImage img(sizeofchart, QImage::Format_RGB888);
    img.fill(Qt::gray);
    
    QPainter painter(&img);
    QPen pen;

    pen.setWidth(1);
    pen.setColor(Qt::lightGray);
    painter.setPen(pen);
    painter.drawLine(0, img.height()*0.25, img.width(), img.height()*0.25);
    painter.drawLine(0, img.height()*0.5, img.width(), img.height()*0.5);
    painter.drawLine(0, img.height()*0.75, img.width(), img.height()*0.75);
    painter.drawText(0,img.height()*0.75,"%25");
    painter.drawText(0,img.height()*0.5,"%50");
    painter.drawText(0,img.height()*0.25,"%75");
    
    int stepx = img.width()/10;

    for(int i = stepx ; i < img.width(); i = i + stepx)
    {
        painter.drawLine(i, 0, i, img.height());
    }
    
    pen.setWidth(2);
    pen.setColor(Qt::blue);
    painter.setPen(pen);

    float stepHorizontal = (float)img.width() / (float)graphSize;
    float stepVertical = (float)img.height() / 100.f;

    QPoint p1;
    QPoint previous = QPoint(0, img.height() - (float)m_cpuUsage.at(0)*stepVertical);

    for(int i=0; i<m_cpuUsage.size(); i++)
    {
        p1 = QPoint((i+1)*stepHorizontal, img.height() - (float)m_cpuUsage.at(i)*stepVertical);
        painter.drawLine(previous, p1);
        previous = p1;
    }
    
    painter.end();

    QGraphicsPixmapItem* item = new QGraphicsPixmapItem(QPixmap::fromImage(img));

    m_scene->clear();
    m_scene->setSceneRect(0, 0, sizeofchart.width(), sizeofchart.height());
    m_scene->addItem(item);
    ui->graphicsView->setScene(m_scene);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    draw();
}

void MainWindow::slotTimer()
{
    // CPU
    float cpuInfo = m_systemInfo.getCpuValue();

    m_cpuUsage.append(cpuInfo);
    
    if(m_cpuUsage.size() > 60)
        m_cpuUsage.removeFirst();
    
    draw();

    // STORAGE
    qint64 bytesTotal, bytesAvailable, bytesFree;
    
    if( m_systemInfo.getSdaInfo(bytesTotal, bytesAvailable, bytesFree) )
    {
        float percent = 100.f * (float)bytesFree / (float)bytesTotal;
        ui->pbSda->setValue(percent);
    }
    // MEMORY
    int total,used,free;
    if(m_systemInfo.getMemoryInfo(total,free,used))
    {
        int percent = 100.f * (int)used / (int)total;
        ui->pbRam->setValue(percent);
    }
    //LABELS
    ui->lb_TotalRamValue->setText(QString::number(total)+" MB");
    ui->lb_UsedRamValue->setText(QString::number(used)+" MB");
    ui->lb_TotalSdaValue->setText(QString::number(bytesTotal/1000/1000/1000)+" GB");
    ui->lb_UsedSdaVAlue->setText(QString::number(bytesAvailable/1000/1000/1000)+" GB");
    ui->label_cpu->setText(QString::number(cpuInfo));
}

