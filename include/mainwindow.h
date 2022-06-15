#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMutex>
#include <QGraphicsScene>
#include "opencv2/opencv.hpp"

#include "capture.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    void showCameraInfo();
    static int addToDatabase(string name);
    static int check_name(string name);
private slots:
    void on_ApplyButton_clicked();
    void on_cBoxCamera_activated(const QString &arg1);
    void on_cBoxAlgorithm_activated(int index);
    void on_OneObjButton_clicked();
    void on_ManyObjButton_clicked();
    void on_YesFilterButton_clicked();
    void on_NoFilterButton_clicked();
    void updateFrame(cv::Mat*);

private:
    Ui::MainWindow *ui;
    cv::Mat currentFrame;

    QGraphicsScene *scene;
    int cameraID;
    int count_cameras;
    int algorithmID;
    bool objectMode;
    bool filterMode;
    string name;
    // for capture thread
    QMutex *data_lock;
    Capture *capturer;
};

#endif // MAINWINDOW_H
