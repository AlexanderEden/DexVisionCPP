//
//  ColorDetectController.h
//  DexVisionCPP
//
//  Created by Alexander Eden on 3/9/14.
//  Copyright (c) 2014 Alexander Eden. All rights reserved.
//

#ifndef __DexVisionCPP__ColorDetectController__
#define __DexVisionCPP__ColorDetectController__

#include <iostream>
#include "ColorDetector.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>





class ColorDetectController {
        
private:
        
        // The algorithm class.
        ColorDetector *cdetect;
        
        // Constructor
        ColorDetectController() {
                // Initliaze the color detector.
                cdetect = new ColorDetector();
        }
        
        // Create a reference to the single instance of the class itself.
        static ColorDetectController *singleton;
        
        cv::Mat image;  // The image to be processed.
        cv::Mat result; // The image result.
        
public:
        // Get access to the Singleton instance.
        static ColorDetectController *getInstance() {
                // Create the instance at first call.
                if(singleton == 0) {
                        singleton = new ColorDetectController;
                }
                return singleton;
        }
        
        // Singleton destroyer.
        static void destroy() {
                if(singleton != 0) {
                        delete singleton;
                        singleton = 0;
                }
        }
        
        // Destructor
        ~ColorDetectController() {
                delete cdetect;
        }
        
        // Trigger
        void process() {
                result = cdetect -> process(image);
        }
        
        // Receiver
        const cv::Mat getLastResult() const {
                return result;
        }
        
        /*
        Getters & Setters
         */
        
        // Set the color distance threshold.
        void setColorDistanceThreshold(int distance) {
                cdetect->setColorDistanceThreshold(distance);
        }
        // Get the color distance threshold.
        int getColorDistanceThreshold() const {
                return cdetect->getColorDistanceThreshold();
        }
        
        // Set the target color for detection.
        void setTargetColor(unsigned char red, unsigned char green, unsigned char blue) {
                cdetect->setTargetColor(red, green, blue);
        }
        // Get the target color.
        void getTargetCOlor(unsigned char &red, unsigned char &green, unsigned char &blue) const {
                cv::Vec3b color = cdetect -> getTargetColor();
                
                red = color[2];
                green = color[1];
                blue = color[0];
        }
        
        // Set the input image. It'll read it from a file.
        bool setInputImage(std::string filename) {
                image = cv::imread(filename);
                
                return (!image.data ? false : true);
        }
        // Get the current input image.
        const cv::Mat getInputImage() const {
                return image;
        }
        
        
        
};








#endif /* defined(__DexVisionCPP__ColorDetectController__) */
