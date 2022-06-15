#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#include "capture.h"

using namespace std;
using namespace cv;
using namespace cv::face;

class Algorithms
{
 public:
    explicit Algorithms(bool objMode, bool filMode);
    ~Algorithms();
 public:
   Mat facialLandmark(Mat frame, CascadeClassifier faceDetector, Ptr<Facemark> facemark);
   Mat haarFaceAndEyes(Mat frame, CascadeClassifier face_cascade, CascadeClassifier eyes_cascade);
   Mat haarFace(Mat frame, CascadeClassifier cascade);
   Mat addFace(Mat frame, string name, CascadeClassifier face_cascade);
   void eigenFaceTrainer();
   Mat FaceRecognition(Mat frame, CascadeClassifier face_cascade, Ptr<FaceRecognizer> model);
 private:
   static void drawPolyline(Mat &im, const vector<Point2f> &landmarks, const int start, const int end, bool isClosed = false);
   static void drawLandmarks(Mat &im, vector<Point2f> &landmarks);
   static void dbread(vector<Mat>& images, vector<int>& labels);
 private:
    bool objectMode;
    bool filterMode;
};

#endif // ALGORITHMS_H
