//
//  MatFunctions.h
//  DexVisionCPP
//
//  Created by Alexander Eden on 3/22/14.
//  Copyright (c) 2014 Alexander Eden. All rights reserved.
//

#ifndef __DexVisionCPP__MatFunctions__
#define __DexVisionCPP__MatFunctions__

#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <algorithm>

using namespace std;

class MatFunctions {
        
public:
        void describe(const cv::Mat &m, string name)  {
                
                cout << "--- Mat Descriptor ---\n";
                cout << "name:\t" << name << endl;
                cout << *m.refcount << "\treferences" << endl;
                cout << m.rows << (m.rows > 1 ? "\trows" : "\trow") << endl;
                cout << m.cols << (m.cols > 1 ? "\tcolumns" : "\tcolumn") << endl;
                cout << m.step << "\teffective width" << endl;
                cout << m.channels() << (m.channels() > 1 ? "\tchannels" : "\tchannel") << endl;
                cout << m.depth() << "\tdepth" << endl;
                cout << m.total() << "\ttotal elements" << endl;
                cout << countNonZero(m) << "\tnonzero elements" << endl;
                cout << "continuous?\t" << (m.isContinuous() ? "yes" : "no") << endl;
                cout << "elem type:\t" << typeName(m.type()) << endl;
                cout << "elem size:\t" << m.elemSize() << "\tbytes" << endl;
                cout << "elem size:\t" << m.elemSize1() << "\tbytes, channels ignored" << endl;
                if(m.channels() == 1) {
                        double min, max;
                        minMaxLoc(m, &min, &max);
                        cout << "elem min:\t" << min << endl;
                        cout << "elem max:\t" << max << endl;
                }
                cout << "----------------------\n\n";
        }
        
        string typeName(const int t) {
                switch(t) {
                        case 0:
                                return "CV_8U";
                        case 1:
                                return "CV_8S";
                        case 2:
                                return "CV_16U";
                        case 3:
                                return "CV_16S";
                        case 4:
                                return "CV_32S";
                        case 5:
                                return "CV_32F";
                        case 6:
                                return "CV_64F";
                        default:
                                return "unknown type";
                }
        }
};


#endif /* defined(__DexVisionCPP__MatFunctions__) */
