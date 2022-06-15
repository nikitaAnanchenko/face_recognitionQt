#include "algorithms.h"
#include <QDebug>
#include <ctime>
#include <cstdlib>

#include <fstream>
#include <sstream>
#include <string>
Algorithms::Algorithms(bool objMode, bool filMode):
     objectMode(objMode),filterMode(filMode){}

Algorithms::~Algorithms() {}

static int filenumber = 0;
//static function
void Algorithms::drawPolyline(Mat &im, const vector<Point2f> &landmarks, const int start, const int end, bool isClosed) {
    vector <Point> points;
    for (int i = start; i <= end; i++) {
        points.push_back(cv::Point(landmarks[i].x, landmarks[i].y));
    }
    polylines(im, points, isClosed, Scalar(255, 200,0), 2, 16);

}


void Algorithms::drawLandmarks(Mat &im, vector<Point2f> &landmarks) {
    if (landmarks.size() == 68) {
      drawPolyline(im, landmarks, 0, 16, false);    // Jaw line
      drawPolyline(im, landmarks, 17, 21, false);   // Left eyebrow
      drawPolyline(im, landmarks, 22, 26, false);   // Right eyebrow
      drawPolyline(im, landmarks, 27, 30, false);   // Nose bridge
      drawPolyline(im, landmarks, 30, 35, true);    // Lower nose
      drawPolyline(im, landmarks, 36, 41, true);    // Left eye
      drawPolyline(im, landmarks, 42, 47, true);    // Right Eye
      drawPolyline(im, landmarks, 48, 59, true);    // Outer lip
      drawPolyline(im, landmarks, 60, 67, true);    // Inner lip
    }
    else
    {
        for(int i = 0; i < landmarks.size(); i++)
        {
            circle(im,landmarks[i],3, Scalar(255, 200,0), FILLED);
        }
    }
}


Mat Algorithms::facialLandmark(Mat frame, CascadeClassifier faceDetector, Ptr<Facemark> facemark){
    Mat gray;

    vector<Rect> faces;
    cvtColor(frame, gray, COLOR_BGR2GRAY);

    TickMeter tm;
    tm.start();

    faceDetector.detectMultiScale(gray, faces, 1.2,
                             (filterMode || objectMode) ? 4 : 0,
                             (objectMode ? CASCADE_FIND_BIGGEST_OBJECT : 0)
                                    | CASCADE_SCALE_IMAGE);
    vector< vector<Point2f> > landmarks;
    bool success = facemark->fit(gray, faces, landmarks);
    if(success) {
      for(int i = 0; i < landmarks.size(); i++) {
              drawLandmarks(frame, landmarks[i]);
              drawFacemarks(frame, landmarks[i], Scalar(0, 0, 255));
      }
    }
    tm.stop();
    double detectionTime = tm.getTimeMilli();
    global::fps = 1000 / detectionTime;
    return frame;
}

Mat Algorithms::haarFace(Mat frame, CascadeClassifier cascade){
    Mat gray;
    vector<Rect> faces;

    cvtColor(frame, gray, COLOR_BGR2GRAY);
    TickMeter tm;

    tm.start();
    cascade.detectMultiScale(gray, faces, 1.2,
                             (filterMode || objectMode) ? 4 : 0,
                             (objectMode ? CASCADE_FIND_BIGGEST_OBJECT : 0)
                                    | CASCADE_SCALE_IMAGE);
    for (size_t i = 0; i < faces.size(); ++i)
    {
        rectangle(gray, faces[i], Scalar(255));
    }
    tm.stop();
    double detectionTime = tm.getTimeMilli();
    global::fps = 1000 / detectionTime;
    frame = gray;
    cvtColor(frame, frame, COLOR_GRAY2RGB);
    return frame;
}

Mat Algorithms::haarFaceAndEyes(Mat frame, CascadeClassifier face_cascade, CascadeClassifier eyes_cascade){
    Mat gray;
    vector<Rect> faces;

    cvtColor(frame, gray, COLOR_BGR2GRAY);
    TickMeter tm;

    tm.start();
    face_cascade.detectMultiScale(gray, faces, 1.2,
                             (filterMode || objectMode) ? 4 : 0,
                             (objectMode ? CASCADE_FIND_BIGGEST_OBJECT : 0)
                                    | CASCADE_SCALE_IMAGE);

    for (size_t i = 0; i < faces.size(); i++){
        rectangle(gray, faces[i], Scalar(255));
        Mat faceROI = gray(faces[i]);
        //-- In each face, detect eyes
        std::vector<Rect> eyes;
        eyes_cascade.detectMultiScale(faceROI, eyes);
        for (size_t j = 0; j < eyes.size(); j++){
            Point eye_center(faces[i].x + eyes[j].x + eyes[j].width/2, faces[i].y + eyes[j].y + eyes[j].height/2);
            int radius = cvRound((eyes[j].width + eyes[j].height)*0.25);
            circle(gray, eye_center, radius, Scalar( 255, 0, 0 ), 4);
        }
    }
    tm.stop();
    double detectionTime = tm.getTimeMilli();
    global::fps = 1000 / detectionTime;
    cvtColor(gray, gray, COLOR_GRAY2RGB);
    frame = gray;
    return frame;
}

Mat Algorithms::addFace(Mat frame, string name, CascadeClassifier face_cascade){
    if(filenumber >= 10)
        filenumber = 0;
    vector<Rect> faces;
    Mat frame_gray;
    Mat crop;
    Mat result;
    Mat gray;
    string text;
    stringstream sstm;

    cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
    equalizeHist(frame_gray, frame_gray);
    face_cascade.detectMultiScale(frame_gray, faces, 1.2,
                             (filterMode || objectMode) ? 4 : 0,
                             (objectMode ? CASCADE_FIND_BIGGEST_OBJECT : 0)
                                    | CASCADE_SCALE_IMAGE, Size(30, 30));
    Rect roi_b, roi_c;

    for (size_t i = 0; i < faces.size(); i++)

    {
        roi_c.x = faces[i].x;
        roi_c.y = faces[i].y;
        roi_c.width = (faces[i].width);
        roi_c.height = (faces[i].height);

        roi_b.x = faces[0].x;
        roi_b.y = faces[0].y;
        roi_b.width = (faces[0].width);
        roi_b.height = (faces[0].height);

        crop = frame(roi_b);
        resize(crop, result, Size(128, 128), 0, 0, INTER_LINEAR);
        cvtColor(result, gray, COLOR_BGR2GRAY);

        stringstream ssfn;
        string filename;
        ssfn << "/home/nikita/diplom/object_recognize/resources/faces/" << name << filenumber << ".jpg";
        filename = ssfn.str();
        imwrite(filename, result);
        filenumber++;

        Point pt1(faces[i].x, faces[i].y);
        Point pt2((faces[i].x + faces[i].height), (faces[i].y + faces[i].width));
        rectangle(frame, pt1, pt2, Scalar(0, 255, 0), 2, 8, 0);
    }
    return frame;
}

void Algorithms::dbread(vector<Mat>& images, vector<int>& labels) {
    vector<cv::String> fn;
    string filename = "/home/nikita/diplom/object_recognize/resources/faces/";
    glob(filename, fn, false);

    size_t count = fn.size();

    for (size_t i = 0; i < count; i++)
    {
        string itsname="";
        char sep = '/';
        size_t j = fn[i].rfind(sep, fn[i].length());
        qDebug() << j;
        if (j != string::npos)
        {
            itsname=(fn[i].substr(j + 1, fn[i].length() - j-6));
        }
        images.push_back(imread(fn[i], 0));
        labels.push_back(atoi(itsname.c_str()));
    }
}

void Algorithms::eigenFaceTrainer() {
    vector<Mat> images;
    vector<int> labels;
    dbread(images, labels);
    qDebug() << "size of the images is " << images.size();
    qDebug() << "size of the labels is " << labels.size();
    qDebug() << "Training begins....";

    //create algorithm eigenface recognizer
    Ptr<EigenFaceRecognizer> model = EigenFaceRecognizer::create();

    //train data
    model->train(images, labels);
    string eigenfaceName = "//home/nikita/diplom/object_recognize/resources/dataset/eigenface.yml";
    model->save(eigenfaceName);

    qDebug() << "Training finished....";
}

static string check_name(int label){
    string line, person;
    string sep = ":";
    int index;
    fstream fout("/home/nikita/diplom/object_recognize/resources/dataset/people.txt");
    while(fout >> line){
        index = stoi(line.substr(0,1));
        if(index == label){
            person = line.substr(line.find(sep) + 1);
            fout.close();
            return person;
        }
    }
    person = "Unrecognized";
    fout.close();
    return person;
}

Mat Algorithms::FaceRecognition(Mat frame, CascadeClassifier face_cascade, Ptr<FaceRecognizer> model) {
    string sampleName = "/home/nikita/diplom/object_recognize/resources/dataset/0.jpg";
    Mat testSample = imread(sampleName, 0);

    int img_width = testSample.cols;
    int img_height = testSample.rows;

    string Pname= "";

    vector<Rect> faces;
    Mat graySacleFrame, original;

    original = frame.clone();
    cvtColor(original, graySacleFrame, COLOR_BGR2GRAY);
    TickMeter tm;

    tm.start();
    face_cascade.detectMultiScale(graySacleFrame, faces, 1.2,
                             (filterMode || objectMode) ? 4 : 0,
                             (objectMode ? CASCADE_FIND_BIGGEST_OBJECT : 0)
                                    | CASCADE_SCALE_IMAGE, Size(30, 30));
    for (int i = 0; i < faces.size(); i++) {
        Rect face_i = faces[i];
        Mat face = graySacleFrame(face_i);

        Mat face_resized;
        resize(face, face_resized, Size(img_width, img_height), 1.0, 1.0, INTER_CUBIC);

        //recognizing what faces detected
        int label = -1; double confidence = 0;
        model->predict(face_resized, label, confidence);
        qDebug() << " confidence " << confidence << " Label: " << label << endl;

        string res_name = check_name(label);

        //drawing green rectagle in recognize face
        rectangle(original, face_i, CV_RGB(0, 255, 0), 1);
        int pos_x = std::max(face_i.tl().x - 10, 0);
        int pos_y = std::max(face_i.tl().y - 10, 0);

        putText(original, res_name, Point(pos_x, pos_y), FONT_HERSHEY_COMPLEX_SMALL, 1.0, CV_RGB(0, 255, 0), 1.0);
    }
    tm.stop();
    double detectionTime = tm.getTimeMilli();
    global::fps = 1000 / detectionTime;
    return original;
}


