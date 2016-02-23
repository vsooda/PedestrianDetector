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
#include "../include/CompressiveTracker.h"

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


    cv::Mat oldMat;
    int index = 0; 
    bool detected = false;
    cv::Rect rect;
    cv::Mat gray;
    while(capture.read(frame) && !detected)
    {
        std::cout << index++ << std::endl;
        image = frame.clone(); 
        clock_t a,b;
        a = clock();
        if (!detected) {
            detected = true;
            vector<cv::Rect_<int> >* rects = pd.detect(image);
            b = clock();
            std::cout << (b-a)* 1000.0 / CLOCKS_PER_SEC << std::endl;
            vector<cv::Rect_<int> >::iterator it;
            for(it = rects->begin(); it != rects->end(); it++)
                rectangle(image, *it, Scalar_<int>(255,0,0));
            if (rects->size() > 0) {
                HOGDescriptor hog;
                cvtColor(frame, gray, COLOR_BGR2GRAY );
                
                vector<float> descriptors;
                rect = (*rects)[0];
                cv::Mat roi;
                cv::resize(gray(rect).clone(), roi, cv::Size(64,128));
                hog.compute(roi, descriptors, cv::Size(1,1));
                cv::Mat hogmat = cv::Mat(descriptors);

                if (oldMat.data != NULL) {
                    double ab = oldMat.dot(hogmat);
                    double aa = oldMat.dot(oldMat);
                    double bb = hogmat.dot(hogmat);
                    double sim = ab / sqrt(aa*bb);
                    std::cout << "simirity: " << sim << std::endl;

                }
                oldMat = hogmat.clone();
            }
        }

        imshow("pedDetector", image);
        if (cv::waitKey(5) == 'q') {
            break;
        }
    }

	CompressiveTracker ct;
    ct.init(gray, rect);
    while (capture.read(frame)) {
        cvtColor(frame, gray, COLOR_BGR2GRAY );
		ct.processFrame(gray, rect);
		rectangle(frame, rect, Scalar(0,0,255));
        cv::imshow("pedDetector", frame);
        if (cv::waitKey(33) == 'q') {
            break;
        }
    }
}

int main(int argc, char *argv[]){
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
