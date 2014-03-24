//
//  ColorDetector.h
//  DexVisionCPP
//
//  Created by Alexander Eden on 3/8/14.
//  Copyright (c) 2014 Alexander Eden. All rights reserved.
//

#ifndef __DexVisionCPP__ColorDetector__
#define __DexVisionCPP__ColorDetector__

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <stdio.h>

#define HUE 0
#define LIGHTNESS 1
#define SATURATION 2

#define DEBUG_CDETECT true

using namespace cv;
using namespace std;

class ColorDetector {
private:
        // Minimum acceptable color different distance.
        int minDist;
        
        // Detect color according to target property.
        bool TARGET_HUE, TARGET_SAT, TARGET_LIGHT;
        Range HUE_RANGE, LIGHT_RANGE, SAT_RANGE;
        // Target color.
        cv::Vec3b target;
        
        // Image containing the resulting binary map.
        cv::Mat result;
        cv::Mat converted;
        
        float getDistance(const Vec3b &color) const {
                float sumDist = 0;
                // If a target value is -1, ignore it.
                if(TARGET_HUE) {
                        sumDist += pow(abs(color[HUE] - target[HUE]), 2);
                }
                if(TARGET_SAT) {
                        sumDist += pow(abs(color[SATURATION] - target[SATURATION]), 2);
                }
                if(TARGET_LIGHT) {
                        sumDist += pow(abs(color[LIGHTNESS] - target[LIGHTNESS]), 2);
                }
                return sqrt(sumDist);
        }
        
        bool checkRanges(const Vec3b &color) const {
                // cout << color << endl;
                // If a target value is -1, ignore it.
                if(TARGET_HUE) {
                        if(color[HUE] > HUE_RANGE.end || color[HUE] < HUE_RANGE.start)
                                return false;
                }
                if(TARGET_SAT) {
                        if(color[SATURATION] > SAT_RANGE.end || color[SATURATION] < SAT_RANGE.start)
                                return false;
                }
                if(TARGET_LIGHT) {
                        if(color[LIGHTNESS] > LIGHT_RANGE.end || color[LIGHTNESS] < LIGHT_RANGE.start)
                                return false;
                }
                return true;
        }
        
        
public:
        // Process method signature.
        cv::Mat processByDistance(const  cv::Mat &image);
        cv::Mat processByRange(const  cv::Mat &image);
        
        // Constructor.
        ColorDetector() : minDist(50), HUE_RANGE(0, 255), LIGHT_RANGE(0, 255), SAT_RANGE(0, 255)  {
                TARGET_HUE = TARGET_SAT = TARGET_LIGHT = false;
                // Default parameter initialization.
                target[0] = target[1] = target[2] = 0;
        }
        
        
        /*
         Getters & Setters
         */
        
        // Sets the minimum threhsold distance. Must be greater than 0.
        void setColorDistanceThreshold(int distance) {
                distance < 0 ? distance = 0 : minDist = distance;
        }
        
        // Gets the minimum threshold distance.
        int getColorDistanceThreshold() const {
                return minDist;
        }
        
        // Sets the target color to be detected.
        void setTargetHLS(int hue, int light, int sat) {
                target[HUE] = hue;
                target[SATURATION] = sat;
                target[LIGHTNESS] = light;
                printf("Target HLS Set:\thue %d\tlightness %d\tsaturation %d\n", target[HUE], target[LIGHTNESS], target[SATURATION]);
                //debug();
        }
        void setRangesHLS(const Range &hR, const Range &lR, const Range &sR) {
                HUE_RANGE = hR;
                LIGHT_RANGE = lR;
                SAT_RANGE = sR;
                cout << "Target Ranges Set:\tHue " << HUE_RANGE.start << ", " << HUE_RANGE.end << "\tLightness " << LIGHT_RANGE.start << ", " << LIGHT_RANGE.end << "\tSaturation " << SAT_RANGE.start << ", " << SAT_RANGE.end << endl;
        }
        void setTargetedProps(bool th, bool tl, bool ts) {
                TARGET_HUE = th;
                TARGET_SAT = ts;
                TARGET_LIGHT = tl;
                cout << "Color Detection Target Props Set:\thue " << TARGET_HUE << "\tlightness " << TARGET_LIGHT << "\tsaturation " << TARGET_SAT << endl;
                
        }
        
        // Gets the target color to be detected.
        Vec3b getTargetColor() const {
                return target;
        }
        
        
        void debug(String msg) {
                if(DEBUG_CDETECT) cout << "Debug cdetect:\t" << msg << endl;
        }
        
};







#endif /* defined(__DexVisionCPP__ColorDetector__) */
