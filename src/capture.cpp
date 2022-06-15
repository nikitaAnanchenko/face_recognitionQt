#include <QTime>
#include <QDebug>
#include <fstream>
#include <ctime>
#include <iostream>
#include <cstdlib>
#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"

#include "capture.h"
#include "algorithms.h"

namespace global {
    double fps = 0;
    bool addToDatabase = false;
}

Capture::Capture(int camera, int algorithm, string name, bool objectMode, bool filterMode, QMutex *lock):
    running(false), cameraID(camera), algorithmID(algorithm),
    name(name), objectMode(objectMode), filterMode(filterMode), data_lock(lock)
{
    frame_width = frame_height = 0;
}

Capture::~Capture() {
}
void init_cascade(
        CascadeClassifier *face_cascade, CascadeClassifier *eyes_cascade,
        CascadeClassifier *lbp_cascade
)
{
    face_cascade->load("/home/nikita/diplom/object_recognize/resources/haarcascade/haarcascade_frontalface_default.xml");
    eyes_cascade->load("/home/nikita/diplom/object_recognize/resources/haarcascade/haarcascade_eye_tree_eyeglasses.xml");
    lbp_cascade->load("/home/nikita/diplom/object_recognize/resources/lbf/lbpcascade_frontalface.xml");
}

void Capture::run() {
    running = true;
    cv::VideoCapture cap(cameraID);
    cv::Mat tmp_frame, result_frame;

    frame_width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    frame_height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);

    CascadeClassifier face_cascade, eyes_cascade, lbp_cascade;
    Ptr<Facemark> facemark;
    Ptr<FaceRecognizer> model;

    init_cascade(&face_cascade, &eyes_cascade, &lbp_cascade);
    if(algorithmID == 1){
        facemark = FacemarkLBF::create();
        facemark->loadModel("/home/nikita/diplom/object_recognize/resources/lbf/lbfmodel.yaml");
    }
    else if(algorithmID == 5){
        model = FisherFaceRecognizer::create();
        string eigenfaceName = "/home/nikita/diplom/object_recognize/resources/dataset/eigenface.yml";
        model->read(eigenfaceName);
    }

    Algorithms *alg = new Algorithms(objectMode, filterMode);

    int count = 0;

    while(running) {
        cap >> tmp_frame;
        if (tmp_frame.empty()) {
            break;
        }
        if(algorithmID == 0){
            result_frame = tmp_frame;
        }
        if(algorithmID == 1){
            result_frame = alg->facialLandmark(tmp_frame, lbp_cascade, facemark);
        }
        else if(algorithmID == 2){
            result_frame = alg->haarFace(tmp_frame, face_cascade);
        }
        else if(algorithmID == 3){
            result_frame = alg->haarFaceAndEyes(tmp_frame, face_cascade, eyes_cascade);
        }
        else if(algorithmID == 4){
            result_frame = alg->addFace(tmp_frame, name, face_cascade);
            count++;
            sleep(1);
        }
        else if(algorithmID == 5){
            result_frame = alg->FaceRecognition(tmp_frame, face_cascade, model);
        }

        //cvtColor(result_frame, result_frame, COLOR_GRAY2RGB);
        data_lock->lock();
        frame = result_frame;
        data_lock->unlock();
        if(count == 10){
            alg->eigenFaceTrainer();
            global::addToDatabase = true;
            emit frameCaptured(&frame);
            break;
        }
        emit frameCaptured(&frame);

    }
    cap.release();
    delete alg;
    running = false;
}

