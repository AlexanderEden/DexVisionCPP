//
//  main.cpp
//  DexVisionCPP
//
//  Created by Alexander Eden on 3/4/14.
//  Copyright (c) 2014 Alexander Eden. All rights reserved.
//

#include <iostream>
#include <cstring>
#include <algorithm>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "ColorDetector.h"
#include "Histogram1D.h"
#include "ContentFinder.h"
#include "morphoFeatures.h"
#include "Grid.h"
#include "DisjointSets.h"
#include "MatFunctions.h"
#define debug true

using namespace std;
using namespace cv;

const String FILEPATH = "/Users/Alexander/dev/tech/Projects/sdrt2-tech/DexVisionCPP/DexVisionCPP/";



// Function Signatures
void colorReduceByPointers(const Mat &image, Mat &output, uchar factor);
void colorReduceByIterators(Mat &image, int div);
void sharpen(const Mat &image, Mat &result);
void sharpenByKernel(const Mat &image, Mat &output);

int main(int argc, const char * argv[]) {
        String imgLoc = FILEPATH + "tools.png";
        Mat_<Vec3b> original = imread(imgLoc);
        if(original.empty()) {
                return 1;
        }
      
        
        Grid origGrid(original);
        
        namedWindow("Grid Original");
        imshow("Grid Original", origGrid.getMemberMat('1'));
        waitKey();
        /*
        namedWindow("Grid Binarized");
        imshow("Grid Binarized", origGrid.getMemberMat('b'));
        waitKey();
        
        namedWindow("Grid Regionalized");
        imshow("Grid Regionalized", origGrid.getMemberMat('r'));
        moveWindow("GridRegionalized", 0, 0);
        waitKey();
        */
        
        
        
        return 0;
}


void sharpenByKernel(const Mat &image, Mat &output) {
        // Construct a 3x3 kernel, all elements set to zero.
        Mat kernel(3, 3, CV_32F, Scalar(0));
        
        // Assign the kernel values.
        kernel.at<float>(1, 1) = 5.0;
        kernel.at<float>(0, 1) = -1.0;
        kernel.at<float>(2, 1) = -1.0;
        kernel.at<float>(1, 0) = -1.0;
        kernel.at<float>(1, 2) = -1.0;
        
        // Filter the Image
        //filter2D(image, output, image.depth(), kernel);
}

void sharpen(const Mat &image, Mat &result) {
        // Allocate the output matrix if necessary.
        result.create(image.size(), image.type());
        
        int factor = 1;
        
        // Loop through all but the first and last rows.
        for (int j = 1; j < image.rows - 1; j++) {
                const uchar * previous = image.ptr<const uchar>(j - 1);
                const uchar * current = image.ptr<const uchar>(j);
                const uchar * next = image.ptr<const uchar>(j + 1);
                
                uchar * output = result.ptr<uchar>(j);
                
                for (int i = 1; i < image.cols * 3; i++) {
                        * output++ =
                        saturate_cast
                        <uchar>(factor * (5 *
                                          current[i] -
                                          current[i - 1] -
                                          current[i + 1] -
                                          previous[i] -
                                          next[i]
                                          ));
                }
        }
        
        // Set the unprocessed pixels to zero.
        Scalar zero(0, 0, 0);
        result.row(0).setTo(zero);
        result.row(result.rows - 1).setTo(zero);
        result.col(0).setTo(zero);
        result.col(result.cols - 1).setTo(zero);
}


void colorReduceByIterators(Mat &image, int div) {
        // Obtain the iterator for the initial position.
        MatIterator_<Vec3b> iterator = image.begin<Vec3b>();
        
        // Obtain the end position.
        MatIterator_<Vec3b> end = image.end<Vec3b>();
        
        // Loop over all of the pixels.
        for ( ; iterator != end; ++iterator) {
                // Process each pixel by channel.
                (*iterator)[0] = (*iterator)[0] / div*div + div/2;
                (*iterator)[1] = (*iterator)[1] / div*div + div/2;
                (*iterator)[2] = (*iterator)[2] / div*div + div/2;
        }
        
}


void colorReduceByPointers(const Mat &original, Mat &output, uchar n) {
        // Recreate an image data buffer that matches the original.
        output.create(original.rows, original.cols, original.type());
        
        // Math stuff for the color reduction operation.
        int factor = pow(2, n);
        uchar mask = 0xFF << n;
        
        // Loop controls.
        int r = original.rows;
        int c = original.cols * original.channels();
        
        // If the image is continuous, make the matrix 1-dimensional
        // to eliminate the need for a double loop.
        if(original.isContinuous()) {
                c = c * r;
                r = 1;
        }
        
        for (int j = 0; j < r; j++) {
                // Get the address of row j in the original image.
                const uchar * origData = original.ptr<uchar>(j);
                
                // Get the address of row j in the output image.
                uchar * outData = output.ptr<uchar>(j);
                
                
                for (int i = 0; i < c; i++) {
                        // Reduce the color of the pixel from the original pic, set output pixel.
                        outData[i] =  (origData[i] & mask) + factor/2;
                }
        }
}


/*
 
 // ColorDetector Implementation
 ColorDetector cdetect;
 
 // Histogram Implementation
 Histogram1D h;
 
 
 
 // Content Finder Implementation
 ColorHistogram hcolor;
 Mat imageROI = original(Rect(0, 0, 500, 100));
 Mat hist = hcolor.getHistogram(imageROI);
 ContentFinder finder;
 finder.setHistogram(hist);
 finder.setThreshold(0.5f);
 // Mat finderResult = finder.find(original, 0, 255, 3);
 
 
 namedWindow("Finder Result");
 imshow("Finder Result", imageROI);
 waitKey();*/




/*
 VideoCapture capture(0);
 capture.set(CV_CAP_PROP_FRAME_WIDTH,1920);
 capture.set(CV_CAP_PROP_FRAME_HEIGHT,1080);
 if(!capture.isOpened()){
 cout << "Failed to connect to the camera." << endl;
 }
 
 capture >> frame;
 cout <<  frame.at<Point2d>(0, 0);
 // imshow("Original", frame);
 
 while(1) {
 capture >> frame;
 imshow("Original",frame);
 }
 
 Mat_<Vec3b> logo = imread(logoLoc);
 
 
 double duration = static_cast<double>(getTickCount());
 colorReduceByPointers(original, reduced, 5);
 //colorReduceByIterators(reduced, 16);
 duration = static_cast<double>(getTickCount()) - duration;
 duration /= getTickFrequency();
 
 if(original.empty()) {
 cout << "Image Not Found" << endl;
 return 0;
 }
 else {
 if(debug) {
 cout << "Image Found " << endl;
 cout << "Height\t" << original.rows << " pixels" << endl;
 cout << "Width\t" << original.cols << " pixels" << endl;
 cout << "Element Type \t\t" << original.type() << endl;
 cout << "Total Elements\t\t" << original.total() << " pixels" << endl;
 cout << "Effective Width\t" << original.step << " bytes" << endl;
 cout << "Element Size\t\t" << original.elemSize() << " bytes" << endl;
 cout << "Channels per Elem\t" << original.channels() << endl;
 cout << "Image Depth\t\t" << original.depth() << endl;
 cout << "Continuous?\t\t" << (original.isContinuous() ? "Yes" : "No") << endl;
 }
 
 }
 
 
 */

