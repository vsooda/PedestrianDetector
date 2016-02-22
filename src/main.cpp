/*******************************************************************************
* Pedestrian Detector v0.1    2013-08
*
* Fabio Reis
* [freis-at-isr.ist.utl.pt]
*
* Please email me if you find bugs, or have suggestions or questions!
* Licensed under the Simplified BSD License [see external/bsd.txt]
*******************************************************************************/

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <algorithm>

/*
 * Our detector
 */
#include "../include/pedDetector.hpp"

/*
 * OpenCV Includes
 */
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

/*
 * Output help summary
 */
void help(){
  

}

/*
 * Input Parsing
 */
int parseInput(int argc, char *argv[], string &filename){
  string fname;
  if(argc == 1){
    fname = string(argv[1]);
    filename = fname;
    return 1;
  }else if(argc > 2)
    return 0;
    
  string inToggle = string(argv[1]);
  transform(inToggle.begin(), inToggle.end(), inToggle.begin(), ::tolower);
  if(inToggle.compare("-f") != 0){
    cout << "Wrong input" << endl;
    return 0;
  }else{
    fname = string(argv[2]);
    filename = fname;
    return 1;
  }
}


//cv::Mat getHogDescriptMat(const cv::Mat& image) {
//    std::cout << image.size() << std::endl;
//    HOGDescriptor hog;
//    vector<float> descriptors;
//    cv::Mat src = image.clone();
//    cv::resize(src, src, cv::Size(64,128));
//    hog.compute(src, descriptors, cv::Size(1,1));
//    cv::Mat hogmat = cv::Mat(descriptors);
//    return hogmat;
//}
cv::Mat getHogDescriptMat(const cv::Mat& image) {
    std::cout << image.size() << std::endl;
    HOGDescriptor hog;
    vector<float> descriptors;
    cv::Mat src;
    cv::resize(image, src, cv::Size(64,128));
    std::cout << src.rowRange(0, 5).colRange(0, 5) << std::endl;
    hog.compute(src, descriptors, cv::Size(1,1));
    cv::Mat hogmat = cv::Mat(descriptors);
    //std::cout << hogmat.rowRange(0, 10) << std::endl;
    //std::cout << "hogmat size: " << hogmat.size() << std::endl;
    return hogmat;
}

double getCosSimirity(cv::Mat a, cv::Mat b) {
    std::cout << "getcosSimirity" << std::endl;
    double ab = a.dot(b);
    double aa = a.dot(a);
    double bb = b.dot(b);
    std::cout << ab << " " << aa << " " << bb << std::endl;
    double sim = ab / sqrt(aa*bb);
    return sim;
}

double getHogCosSimirity(cv::Mat image1, cv::Mat image2) {
    std::cout << "gethogCossimirity" << std::endl;
    cv::Mat ahogMat = getHogDescriptMat(image1).clone();//if not clone, ahogmat will be the same with bhogmat, don't know why
    cv::Mat bhogMat = getHogDescriptMat(image2).clone();
    std::cout << ahogMat.rowRange(0, 10) << std::endl;
    std::cout << ahogMat.rowRange(0, 10) << std::endl;
    return getCosSimirity(ahogMat, bhogMat);
}

void videoPerestrianDetectTest(const char* filename) {
    bool fromfile = true;
    VideoCapture capture;
    capture.open(filename);
    // Read options
    // Init camera
    if (!capture.isOpened())
    {
        cout << "capture device failed to open!" << endl;
        return ;
    }
    // Register mouse callback to draw the tracking box
    namedWindow("pedDetector", CV_WINDOW_AUTOSIZE);
    //setMouseCallback("CT", mouseHandler, NULL);

    Mat frame;
    Mat last_gray;
    Mat first;
    if (fromfile)
    {
        capture >> frame;
        cvtColor(frame, last_gray, CV_RGB2GRAY);
        frame.copyTo(first);
    }
    else
    {
        capture.set(CV_CAP_PROP_FRAME_WIDTH, 340);
        capture.set(CV_CAP_PROP_FRAME_HEIGHT, 240);
    }

    // Run-time
    Mat image;

    PedDetector pd;

    cv::Mat priImage;
    int index = 0; 
    while(capture.read(frame))
    {
        std::cout << index++ << std::endl;
        image = frame.clone(); 
        clock_t a,b;
        a = clock();
        vector<cv::Rect_<int> >* rects = pd.detect(image);
        b = clock();
        std::cout << (b-a)* 1000.0 / CLOCKS_PER_SEC << std::endl;
        vector<cv::Rect_<int> >::iterator it;
        for(it = rects->begin(); it != rects->end(); it++)
            rectangle(image, *it, Scalar_<int>(255,0,0));
        if (rects->size() > 0) {
            //HOGDescriptor *phog = new HOGDescriptor(cv::Size(64, 128), cv::Size(16, 16), cv::Size(2, 2), cv::Size(2, 2), 9);
            cv::Mat gray;
            cvtColor(frame, gray, COLOR_BGR2GRAY );
            
            cv::Rect rect = (*rects)[0];
            cv::Mat roi;
            roi = gray(rect).clone();
            //cv::Mat hogmat = getHogDescriptMat(roi);
            //std::cout << hogmat.size() << std::endl;

            if (priImage.data != NULL) {
                std::cout << roi.size() << " " << priImage.size() << std::endl;
                double sim = getHogCosSimirity(roi, priImage);
                std::cout << "simirity: " << sim << std::endl;
            }
            priImage = roi.clone();
        }
        imshow("pedDetector", image);
        if (cv::waitKey(100) == 'q') {
            break;
        }
    }
}


void simarity_test() {
    cv::Mat a = cv::imread("data/a.png", 0);
    cv::Mat b = cv::imread("data/b.png", 0);
    cv::Mat c = cv::imread("data/c.png", 0);
    double sim_ab = getHogCosSimirity(a, b);
    double sim_ac = getHogCosSimirity(a, c);
    double sim_bc = getHogCosSimirity(b, c);
    std::cout << sim_ab << " " << sim_ac  << " " << sim_bc<< std::endl;
}

int main(int argc, char *argv[]){
    //simarity_test();
    //return 0;
    videoPerestrianDetectTest("data/test.avi");
    return 0;

    string filename;
    if(!parseInput(argc - 1, argv, filename))
    return 0;

    std::cout << filename << std::endl;
    Mat image = imread(filename.c_str());

    /*
     * Running the detector
     */
    PedDetector pd;
    vector<cv::Rect_<int> >* rects = pd.detect(image);

    /*
     * OpenCV Output
     */

    namedWindow("Teste", CV_WINDOW_NORMAL);
    vector<cv::Rect_<int> >::iterator it;
    for(it = rects->begin(); it != rects->end(); it++)
        rectangle(image, *it, Scalar_<int>(255,0,0));
    imshow("Teste", image);
    waitKey(0);


    /*
     * File Output
     */
    string resultTXT = filename.substr(0,filename.length()-1-3) + ".txt";
    ofstream myfile;
    myfile.open(resultTXT.c_str());
    myfile << "Each line of this file is of the type:" << endl 
         << "ID : X, Y, Height, Width" << endl;

    //vector<cv::Rect_<int> >::iterator it;
    int detection = 0;
    for(it = rects->begin(); it != rects->end(); it++, detection++){
        myfile << detection << " : " << it->x << ", " << it->y << ", " << 
                                      it->height << ", " << it->width << endl;
    }
    myfile.close();

    /*
     * Free Memory
     */
    delete(rects);
    return 0;
}
