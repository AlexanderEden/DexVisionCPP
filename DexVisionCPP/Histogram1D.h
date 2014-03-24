//
//  Histogram1D.h
//  DexVisionCPP
//
//  Created by Alexander Eden on 3/9/14.
//  Copyright (c) 2014 Alexander Eden. All rights reserved.
//

#ifndef __DexVisionCPP__Histogram1D__
#define __DexVisionCPP__Histogram1D__

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

class Histogram1D {
private:
        int histSize[1];        // Number of bins.
        float hranges[2];       // Min and Max pixel value.
        const float * ranges[1];
        int channels[1];        // We'll only be using 1 channel.
        
public:
        // Constructor
        Histogram1D() {
                // Prepare the arguments for the 1D histogram.
                histSize[0] = 256;
                hranges[0] = 0.0;
                hranges[1] = 256.0;
                ranges[0] = hranges;
                channels[0] = 0;        // By default, we looks at channel 0.
        }
        
        // Compute the 1D Histogram
        cv::Mat getHistogram(const cv::Mat &image) {
                cv::Mat hist;
                histSize[0] = 256;
                // Compute it.
                cv::calcHist(
                             &image,
                             1,         // Histogram from 1 image only.
                             channels,  // The channel used.
                             cv::Mat(), // No mask is used.
                             hist,      // The resulting histogram.
                             1,         // 1 dimension.
                             histSize,  // Number of histogram bins.
                             ranges     // Pixel value range.
                             );
                
                // Done, return it.
                return hist;
        }
        // Compute the 1D Histogram
        cv::Mat getHistogram(const cv::Mat &image, int binCount) {
                cv::Mat hist;
                
                histSize[0] = binCount;
                
                // Compute it.
                cv::calcHist(
                             &image,
                             1,         // Histogram from 1 image only.
                             channels,  // The channel used.
                             cv::Mat(), // No mask is used.
                             hist,      // The resulting histogram.
                             1,         // 1 dimension.
                             histSize,  // Number of histogram bins.
                             ranges     // Pixel value range.
                             );
                
                // Done, return it.
                return hist.t();
        }
        
        cv::Mat getHistogramImage(const cv::Mat &image) {
                // First compute the histogram.
                cv::Mat hist = getHistogram(image);
                
                // Get the Min and Max bin values.
                double maxVal = 0;
                double minVal = 0;
                cv::minMaxLoc(hist, &minVal, &maxVal, 0, 0);
                
                // Image on which to display the histogram.
                int d = histSize[0];
                int w = histSize[0];
                cv::Mat histImg(d, w + 20, CV_8U, Scalar(255));
                
                // Set the highest point at 90% of nbins.
                int hpt = static_cast<int>(d);
                
                // Draw a vertical line for each bin.
                for (int h = 0; h < d; h++) {
                        float binVal = hist.at<float>(h);
                        int intensity = static_cast<int>(binVal * hpt/maxVal);
                        
                        // Draw a line between the 2 points.
                        line(
                                  histImg,
                                  Point(h + 10, histSize[0]),
                                  Point(h + 10, histSize[0] - intensity),
                                  Scalar::all(0)
                                  );
                        
                }
                return histImg;
        }
        
        
        Mat applyLookUp(const Mat &image, const Mat &lookup) {
                // Mat lookup is a is a 1x256 uchar matrix.
                // Create the output image.
                Mat result;
                
                // Apply the lookup table.
                LUT(image, lookup, result);
                
                return result;
        }
        
        Mat invert(const Mat &image) {
                // Build the inversion lookup table.
                int dim = 256;
                cout << dim << endl;
                Mat lut(1,      // Dimension
                        &dim,   // 256 entries
                        CV_8U); // uchar type
                for (int i = 0; i < dim; i++) {
                        lut.at<uchar>(i) = 255 - i;
                }
                
                Mat result = applyLookUp(image, lut);
                return result;
        }
        
        Mat stretch(const Mat &image, int minValue) {
                // Grab the histogram first.
                MatND hist = getHistogram(image);
                
                // Find the left extremity.
                int imin = 0;
                for(; imin < histSize[0]; imin++) {
                        cout << hist.at<float>(imin) << endl;
                        if(hist.at<float>(imin) > minValue) break;
                }
                
                // Find the right extremity.
                int imax = histSize[0] - 1;
                for(; imax >= 0; imax--) {
                        if(hist.at<float>(imax) > minValue) break;
                }
                
                // Create the lookup table.
                int dim = 256;
                Mat lookup(1, &dim, CV_8U);
                
                // Build the lookup table.
                for (int i = 0; i < dim; i++) {
                        // Stretch between imin and imax.
                        if(i < imin) lookup.at<uchar>(i) = 0;
                        else if(i > imax) lookup.at<uchar>(i) = 255;
                        
                        // Linear mapping.
                        else lookup.at<uchar>(i) = static_cast<uchar>(
                                                                      255.0 * (i - imin)/(imax - imin) + 0.5);
                }
                
                // Apply the lookup table.
                Mat result;
                result = applyLookUp(image, lookup);
                return result;
        }
        
        
        
        
        
        
        
        
        
        
};

#endif /* defined(__DexVisionCPP__Histogram1D__) */
