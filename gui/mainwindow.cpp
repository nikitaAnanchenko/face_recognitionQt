#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCameraInfo>
#include <QLabel>
#include <QDir>
#include <QMessageBox>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , capturer(nullptr)
{
    ui->setupUi(this);
    //set graphics scene for videoflow
    scene = new QGraphicsScene();
    ui->graphicsView->setScene(scene);
    //set avalible videocameras
    showCameraInfo();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showCameraInfo()
{
    count_cameras = QCameraInfo::availableCameras().count();
    if(count_cameras == 0){
        ui->cBoxCamera->addItem("No cameras avalible");
    }
    else{
        QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
        foreach (const QCameraInfo &cameraInfo, cameras){
            ui->cBoxCamera->addItem(cameraInfo.deviceName());
        }
    }
}

int MainWindow::check_name(string name){
    string line;
    string sep = ":";
    fstream fin("/home/nikita/diplom/object_recognize/resources/dataset/people.txt");
    while(fin >> line){
        string person = line.substr(line.find(sep) + 1);
        if(person == name){
            fin.close();
            return stoi(line.substr(0,1));
        }
    }
    fin.close();
    return 0;
}

int MainWindow::addToDatabase(string name){
    int index;
    string line, last_line;
    fstream fin("/home/nikita/diplom/object_recognize/resources/dataset/people.txt");
    while(fin >> line)
        last_line = line;
    fin.close();
    index = stoi(last_line.substr(0,1));
    stringstream ss;
    ss << ++index << ":" << name << endl;
    string fullname;
    fullname = ss.str();

    fstream fout("/home/nikita/diplom/object_recognize/resources/dataset/people.txt", ios::app);
    fout << fullname;
    cout << fullname << endl;
    fout.close();
    return index;
}

void MainWindow::on_ApplyButton_clicked()
{
    if(algorithmID == 4){
        QString person_name = ui->nameEdit->text();
        name = person_name.toStdString();
        int index = check_name(name);
        if(index != 0){
            QMessageBox::information(this, "Face recognizer", "Вы уже есть в базе данных!");
        }
        else {
            index = addToDatabase(name);
            name = to_string(index);
        }
    }
    if(capturer != nullptr) {
        // if a thread is already running, stop it
        capturer->setRunning(false);
        disconnect(capturer, &Capture::frameCaptured, this, &MainWindow::updateFrame);
        connect(capturer, &Capture::finished, capturer, &Capture::deleteLater);
        sleep(1);
    }
    capturer = new Capture(cameraID, algorithmID, name, objectMode, filterMode,  data_lock);
    connect(capturer, &Capture::frameCaptured, this, &MainWindow::updateFrame);
    capturer->start();
}

void MainWindow::updateFrame(cv::Mat *mat)
{
    data_lock->lock();
    currentFrame = *mat;
    data_lock->unlock();

    QImage frame(
        currentFrame.data,
        currentFrame.cols,
        currentFrame.rows,
        currentFrame.step,
        QImage::Format_RGB888);
    QPixmap image = QPixmap::fromImage(frame);

    scene->clear();
    ui->graphicsView->resetMatrix();
    scene->addPixmap(image);
    scene->update();
    ui->graphicsView->setSceneRect(image.rect());
    ui->fpsValue->setNum(global::fps);
    if(global::addToDatabase){
        QMessageBox::information(this, "Face recognizer", "Вы добавлены в базу данных!");
        global::addToDatabase = false;
    }
}

void MainWindow::on_OneObjButton_clicked()
{
    if(ui->OneObjButton->isChecked()){
        objectMode = false;
        ui->objModeValue->setText("Один");
    }
}

void MainWindow::on_ManyObjButton_clicked()
{
    if(ui->ManyObjButton->isChecked()){
        objectMode = true;
        ui->objModeValue->setText("Много");
    }
}

void MainWindow::on_YesFilterButton_clicked()
{
    if(ui->YesFilterButton->isChecked()){
        filterMode = true;
        ui->filterValue->setText("Да");
    }
}

void MainWindow::on_NoFilterButton_clicked()
{
    if(ui->NoFilterButton->isChecked()){
        filterMode = false;
        ui->filterValue->setText("Нет");
    }
}


void MainWindow::on_cBoxAlgorithm_activated(int index)
{
    if(index == 0) {
        algorithmID = 0;
        ui->algorithmValue->setText("VideoCapture");
    }
    else if(index == 1) {
        algorithmID = 1;
        ui->algorithmValue->setText("Facial Landmark Detection");
    }
    else if(index == 2){
        algorithmID = 2;
        ui->algorithmValue->setText("Haar cascade: Face detection");
    }
    else if(index == 3){
        algorithmID = 3;
        ui->algorithmValue->setText("Haar cascade: Face and eyes detection");
    }
    else if(index == 4){
        algorithmID = 4;
        ui->algorithmValue->setText("Add Face to database");
    }
    else if(index == 5){
        algorithmID = 5;
        ui->algorithmValue->setText("Face Recognizer");
    }
}

void MainWindow::on_cBoxCamera_activated(const QString &arg1)
{
    if(count_cameras == 0){
        ui->cameraValue->setText("No cameras avalible");
    }
    else {
        QRegularExpression rx("video(\\d+)");
        QRegularExpressionMatchIterator i = rx.globalMatch(arg1);
        QRegularExpressionMatch match = i.next();
        QString word = match.captured(1);
        cameraID = word.toInt();
        ui->cameraValue->setText(arg1);
    }
}

