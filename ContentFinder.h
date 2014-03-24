//
//  ContentFinder.h
//  DexVisionCPP
//
//  Created by Alexander Eden on 3/10/14.
//  Copyright (c) 2014 Alexander Eden. All rights reserved.
//

#ifndef __DexVisionCPP__ContentFinder__
#define __DexVisionCPP__ContentFinder__

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;


class ContentFinder {
private:
        float hranges[2];
        const float *ranges[3];
        int channels[3];
        float threshold;
        MatND histogram;
        
public:
        ContentFinder() : threshold(-1.0f) {
                ranges[0] = hranges;    // All channels have the same range.
                ranges[1] = hranges;
                ranges[2] = hranges;
        }
        
        // Set the threshold on histogram values [0, 1].
        void setThreshold(float t) {
                threshold = t;
        }
        
        // Get the treshold.
        float getThreshold() {
                return threshold;
        }
        
        // Set the reference histogram.
        void setHistogram(const MatND &h) {
                histogram = h;
                normalize(histogram, histogram, 1.0);
        }
        
        Mat find(const Mat &image, float minValue, float maxValue,
                 //int *channels,
                 int dim) {
                Mat result;
                
                hranges[0] = minValue;
                hranges[1] = maxValue;
                
                for(int i = 0; i < dim; i++)
                        this -> channels[i] = channels[i];
                
                calcBackProject(&image, 1,      // Input image.
                                channels,       // List of channels used.
                                histogram,      // The histogram we're using.
                                result,         // The resulting backprojection.
                                ranges,         // The range of values.
                                255.0           // The scaling factor.
                                );
                if(threshold > 0.0)
                        cv::threshold(result, result, 255*threshold, 255, cv::THRESH_BINARY);
                return result;
                
        }
        
        
        
};



#endif /* defined(__DexVisionCPP__ContentFinder__) */
