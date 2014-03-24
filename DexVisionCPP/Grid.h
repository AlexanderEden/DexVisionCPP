//
//  Grid.h
//  DexVisionCPP
//
//  Created by Alexander Eden on 3/11/14.
//  Copyright (c) 2014 Alexander Eden. All rights reserved.
//

#ifndef __DexVisionCPP__Grid__
#define __DexVisionCPP__Grid__

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <algorithm>
#include <iomanip>

#include "ColorDetector.h"
#include "DisjointSets.h"
#include "Histogram1D.h"
#include "MatFunctions.h"

#define EXTRACT_MAX 10
using namespace cv;
using namespace std;


class Grid {
        
private:
        Mat orig, bin, reg;
        ColorDetector cdetect;
        int H, W, L, C; // C = total channels
        int type;
        Histogram1D histGenerator;
        Mat labelHist;
        MatFunctions mf;
        int extractionCapacity;
        vector<ushort> extractionLabels;
        RNG rng;
        
        
        
public:
        
        // Constructor
        Grid(Mat _grid)  {
                orig = _grid;
                
                rng = RNG(12345);
                
                H = orig.rows;
                W = orig.cols;
                C = orig.channels();
                type = orig.type();
                bin = Mat::zeros(H, W, CV_8UC1);
                reg = Mat::zeros(H, W, CV_16UC1);
                labelHist = Mat::zeros(H, W, CV_32SC1);
                extractionLabels = Mat(1, EXTRACT_MAX, CV_16UC1);
                extractionCapacity = EXTRACT_MAX;
                
                
                // Color Detector
                cdetect.setTargetHLS(0, 150, 0);
                cdetect.setRangesHLS(Range(0, 255), Range(0, 50), Range(0, 50));
                cdetect.setTargetedProps(false, true, true);
                
                // Initial manipulations.
                binarizeOrig();
                regionalizeBin();
                
                // Extract all pixels with label 1.
                mf.describe(reg, "Regionalized");
                cout << labelHist << endl;
                
                Mat extraction(H, W, CV_16UC1, Scalar(0));
                Mat roi;
                
                for (int i = 0; i < extractionCapacity; i++) {
                        extraction = (reg == static_cast<ushort>(extractionLabels[i]));
                        
                        // Declare the contours & hierarchy vectors.
                        vector<vector<Point>> contours;
                        vector<Vec4i> hierarchy;
                        
                        // Find the contours.
                        findContours(extraction, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
                        
                        // Initialize the contours poly & bounding rectangle vectors.
                        vector<vector<Point>> contours_poly(contours.size());
                        vector<Rect> boundRect(contours.size());
                        
                        // Tell me about the results.
                        cout << "contour size\t" <<  contours.size() << endl;
                        cout << "hierarchy size\t" << hierarchy.size() << endl;
                        cout << "contours vector index " << i << ", contains\t" << contours[0].size() << "\tpoints" <<  endl;
                        
                        // Draw a point at each point within the vertex.
                        for (int p = 0; p < contours[0].size(); p++) {
                                circle(extraction, contours[0][p], 1, Scalar(255), 2);
                        }
                        
                        
                        approxPolyDP(Mat(contours[0]), contours_poly[0], 3, true);
                        boundRect[0] = boundingRect(Mat(contours_poly[0]));
                        cout << "vector holding bounding boxes contains " << boundRect.size() << " rectangles" << endl;
                        roi = reg(boundRect[0]);
                        Moments moms = moments(roi, true);
                        cout << "Spatial Moments of the ROI\n" << setw(14) << moms.m00 << '\t' << moms.m10 << '\t' << moms.m01 << '\t' << moms.m20 << '\t' << moms.m11 << '\t' << moms.m02 << '\t' << moms.m30 << '\t' << moms.m21 << '\t' << moms.m12 << '\t' << moms.m03 << endl;
                        cout << "Central Moments of the ROI\n" << setw(14) << moms.mu20 << '\t' << moms.mu11 << '\t' << moms.mu02 << '\t' << moms.mu30 << '\t' << moms.mu21 << '\t' << moms.mu12 << '\t' << moms.mu03 << endl;
                        cout << "Center of Mass\n" << moms.m10/moms.m00 << ", " << moms.m01/moms.m00 << endl;
                        circle(roi, Point2f(moms.m10/moms.m00, moms.m01/moms.m00), 1, Scalar(200), 2);
                        //rectangle( extraction, boundRect[i].tl(), boundRect[i].br(), Scalar(rng.uniform(0, 255)), 2, 8, 0 );
                        // cout << boundRect[i] << endl;
                        
                        imshow("Extraction", roi);
                        waitKey();
                        
                }
                
                // cout << labelHist << endl;
                // cout << histGenerator.getHistogram(reg) << endl;
        }
        void binarizeOrig() {
                // Run the color detector.
                bin = cdetect.processByRange(orig);
                
                // Zero-out the borders.
                Scalar zero(0);
                bin.row(0).setTo(zero);
                bin.row(H - 1).setTo(zero);
                bin.col(0).setTo(zero);
                bin.col(W - 1).setTo(zero);
        }
        Mat getMemberMat(char which);
        void regionalizeBin();
        
};


#endif /* defined(__DexVisionCPP__Grid__) */
