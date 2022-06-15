#ifndef CAPTURE_H
#define CAPTURE_H

#include <QString>
#include <QThread>
#include <QMutex>

#include "opencv2/opencv.hpp"
#include "opencv2/videoio.hpp"

using namespace std;

namespace global {
    extern double fps;
    extern bool addToDatabase;
}

class Capture : public QThread
{
    Q_OBJECT
public:
    explicit Capture(int camera, int algorithm, string name, bool objectMode, bool filterMode, QMutex *lock);
    ~Capture();
    void setRunning(bool run) {running = run; }

protected:
    void run() override;

signals:
    void frameCaptured(cv::Mat *data);

private:
    void detectObjects(cv::Mat &frame);

private:
    bool running;
    int cameraID;
    int algorithmID;
    bool objectMode;
    bool filterMode;
    string name;
    QMutex *data_lock;
    cv::Mat frame;

    int frame_width, frame_height;
};

#endif // CAPTURE_H

