//
//  ColorDetector.cpp
//  DexVisionCPP
//
//  Created by Alexander Eden on 3/8/14.
//  Copyright (c) 2014 Alexander Eden. All rights reserved.
//

#include "ColorDetector.h"


Mat ColorDetector :: processByDistance(const Mat &image) {

        // Reallocate the binary map if necessary.
        result.create(image.rows, image.cols, CV_8U);
        
        // Reallocate the intermediate image if necessary.
        converted.create(image.rows, image.cols, image.type());
        
        // Convert to the LAB color space.
        cv::cvtColor(image, converted, CV_BGR2HLS);
        
        // Acquire the iterators.
        cv::Mat_<Vec3b>::iterator it = converted.begin<Vec3b>();
        cv::Mat_<Vec3b>::iterator itend = converted.end<Vec3b>();
        cv::Mat_<uchar>::iterator itout = result.begin<uchar>();
        // Pixel access iterator loop.
        for (; it != itend; ++it, ++itout) {
                // Compute the pixel's distance from the target color.
                if (getDistance(*it) < minDist) {
                        *itout = 255;
                }
                else {
                        *itout = 0;
                }
        }
        
        return result;
}

Mat ColorDetector :: processByRange(const Mat &image) {
        
        // Reallocate the binary map if necessary.
        result.create(image.rows, image.cols, CV_8U);
        
        // Reallocate the intermediate image if necessary.
        converted.create(image.rows, image.cols, image.type());
        
        // Convert to the LAB color space.
        cv::cvtColor(image, converted, CV_BGR2HLS);
        
        // Acquire the iterators.
        cv::Mat_<Vec3b>::iterator it = converted.begin<Vec3b>();
        cv::Mat_<Vec3b>::iterator itend = converted.end<Vec3b>();
        cv::Mat_<uchar>::iterator itout = result.begin<uchar>();
        cout << getDistance(*it) << endl;
        // Pixel access iterator loop.
        for (; it != itend; ++it, ++itout) {
                // Compute the pixel's distance from the target color.
                if (checkRanges(*it)) {
                        *itout = 255;
                }
                else {
                        *itout = 0;
                }
        }
        
        return result;
}
