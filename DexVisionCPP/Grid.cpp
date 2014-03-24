//
//  Grid.cpp
//  DexVisionCPP
//
//  Created by Alexander Eden on 3/11/14.
//  Copyright (c) 2014 Alexander Eden. All rights reserved.
//

#include "Grid.h"



Mat Grid::getMemberMat(char which) {
        /*  Mat regVis;
         if(which == 'r') {
         regVis = reg.clone();
         regVis.convertTo(regVis, CV_8UC1);
         // Second pass.
         ushort r = reg.rows, c = reg.cols;
         for (int i = 0; i < r; i++) {
         ushort *regVisPtr = regVis.ptr<ushort>(i);
         for (int j = 0; j < c; j++) {
         if(regVisPtr[i] > 0) {
         regVisPtr[i] = 255 - regVisPtr[i];
         }
         }
         
         }
         
         }*/
        
        switch(which) {
                case '1':
                        return orig;
                        break;
                case 'b':
                        return bin;
                        break;
                case 'r':
                        return reg;
                        break;
                case 'h':
                        return histGenerator.getHistogramImage(reg);
                        break;
                default:
                        return orig;
                        
        }
}



void Grid::regionalizeBin() {
        
        // Reset the region matrix.
        reg = bin.clone();
        reg.convertTo(reg, CV_16UC1);
        mf.describe(reg, "Regionalized (reg)");
        
        // Create the Disjoint Set Forest
        DisjointSets sets(EXTRACT_MAX+1);
        
        // Scan Bounds
        int rowBound = reg.rows - 1;
        int colBound = reg.cols - 1;
        
     
        // Set the label counter to EXTRCT_MAX to avoid any conflict with label mapping later.
        ushort labelCounter = EXTRACT_MAX;
        
        // First pass. Go by rows.
        for (int j = 1; j < rowBound; j++) {
                
                const ushort * currRow = reg.ptr<const ushort>(j);      // Pointer to the current row.
                const ushort * prevRow = reg.ptr<const ushort>(j - 1);  // Pointer to the row above the current row.
                ushort * output = reg.ptr<ushort>(j);                   // Output Pointer
                
                // Then by columns.
                for(int i = 1; i < colBound; i++) {
                        // Grab the neighbors.
                        ushort N = prevRow[i];
                        ushort NW = prevRow[i - 1];
                        ushort NE = prevRow[i + 1];
                        ushort W = currRow[i - 1];
                        
                        // Is the pixel not empty?
                        if(currRow[i] != 0) {
                                // Check the neighbors.
                                if(N == 0 && NE == 0 && W == 0 && NW == 0) {
                                        // No neighbors, new region!
                                        labelCounter++;
                                        cout << "new label\t" << labelCounter << endl;
                                        sets.AddElements(1);
                                        output[i] = saturate_cast<ushort>(labelCounter);
                                }
                                else {
                                        // Has at least one neighbor. Give it the value of the lowest one.
                                        ushort smallest = 65535;
                                        
                                        // Find the lowest label value of the neighboring pixels.
                                        smallest = (N > 0 && N < smallest ? N : smallest);
                                        smallest = (NE > 0 && NE < smallest ? NE : smallest);
                                        smallest = (NW > 0 && NW < smallest ? NW : smallest);
                                        smallest = (W > 0 && W < smallest ? W : smallest);
                                        
                                        // Record any new equivalences in the union LUT.
                                        if(N > 0)       sets.Union(sets.FindSet(N), sets.FindSet(smallest));
                                        if(NE > 0)      sets.Union(sets.FindSet(NE), sets.FindSet(smallest));
                                        if(W > 0)       sets.Union(sets.FindSet(W), sets.FindSet(smallest));
                                        if(NW > 0)      sets.Union(sets.FindSet(NW), sets.FindSet(smallest));
                                        
                                        output[i] = saturate_cast<ushort>(smallest);
                                }
                        }
                }
        }
        
        // Build a temporary label histogram.
        Mat tempLabelHist(1, labelCounter, CV_32SC1, Scalar(0));
        
        // Second pass.
        ushort r = reg.rows, c = reg.cols;
        for (int i = 0; i < r; i++) {
                ushort *regPointer = reg.ptr<ushort>(i);
                for (int j = 0; j < c; j++) {
                        regPointer[j] = sets.FindSet(regPointer[j]);    // Assign the correct label.
                        tempLabelHist.at<int>(static_cast<int>(regPointer[j]))++;     // Increment occurance count for that label within the histogram.
                }
                
        }
        
        // Save the label histogram.
        labelHist = tempLabelHist.clone();
        
        mf.describe(labelHist, "Label Histogram");
        mf.describe(tempLabelHist, "Temporary Label Histogram");
        double minLabel, maxLabel;
        Point minLoc, maxLoc;

        // Grab the extraction tags.
        tempLabelHist.at<int>(0) = 0;
        cout << "Finding extraction labels...\n";
        for (int i = 0; i < extractionCapacity; i++) {
                minMaxLoc(tempLabelHist, &minLabel, &maxLabel, &minLoc, &maxLoc);
                
                extractionLabels[i] = saturate_cast<ushort>(maxLoc.x);
                cout << "next highest area of " << maxLabel << " has an index label of " << maxLoc.x << endl;
                // 3.5  Set the max count to zero for this location.
                tempLabelHist.at<int>(maxLoc.x) = 0;
        }
        
        cout << "Temporary Hist\n" << tempLabelHist << endl;
        cout << "Permanent Hist\n" << labelHist << endl;
        
        for (vector<ushort>::iterator it = extractionLabels.begin(); it != extractionLabels.end(); ++it) {
                cout << *it << '\t';
        }
        cout << endl;
        


        
        // 1    Set the zeroth element of the histogram to 0.
       /* tempLabelHist.at<int>(0) = 0;
        double minLabel, maxLabel;
        Point minLoc, maxLoc;

        // 2    Create a matrix to hold the labels to be remapped for extraction.
        Mat labelsMap(1, regionCount, CV_16UC1);

        cout << "Temp Label hist, pre collection\n" << tempLabelHist << endl;
        // 3    Collect EXTRACT_MAX (10) labels with the larget areas.
        for (int i = 0; i < regionCount; i++) {
                minMaxLoc(tempLabelHist, &minLabel, &maxLabel, &minLoc, &maxLoc);
                labelsMap.at<ushort>(i) = maxLoc.x;
                
                // 3.5  Set the max count to zero for this location.
                tempLabelHist.at<int>(maxLoc.x) = i + 1;
        }
        cout << "Temp Label hist, post collection\n" << tempLabelHist << endl;
        
        // 4    Reassign the labels in the reg matrix to their reduced values.
        for (int j = 0; j < r; j++) {
                ushort *regPointer = reg.ptr<ushort>(j);
                for (int i = 0; i < c; i++) {
                        if(regPointer[i] != 0){
                                // cout << "reassinging pixel with label " << regPointer[i] << " to " << saturate_cast<ushort>(tempLabelHist.at<int>(static_cast<int>(regPointer[i]))) << endl;
                                regPointer[i] = saturate_cast<ushort>(tempLabelHist.at<int>(static_cast<int>(regPointer[i])));    // Assign the correct label.

                        }
                }
                
        }
        

        //  cout << reg << endl;
        
        cout << "Temporary Label Hist\n" << tempLabelHist << endl;
        cout << "Labels Map\n" << labelsMap << endl;
        mf.describe(labelsMap, "Labels Map");
        
        
        //normalize(reg, reg, 0, 255, NORM_MINMAX);
        //reg.convertTo(reg, CV_8UC1);
        // cout << histGenerator.getHistogram(reg) << endl;
        labelHist = histGenerator.getHistogram(reg, labelCounter);//cv::calcHist(reg, 1, channels, cv::Mat(), hist, 1, 256, ranges);
                                                                  //cout << labelHist << endl;
        mf.describe(labelHist, "Label Histogram 1");
        //   imshow("Hist" , histGenerator.getHistogramImage(reg));
        
        
        
        // Set the label counter.
        
        cout << regionCount << " regions counted" << endl;
        
        
        
        */
        //mf.describe(labelHist, "Label Histogram");
        
}


/* Create a LUT that compresses the labels and pixel values.
 Mat labelCompressLUT(1, labelCounter, CV_32SC1, Scalar(0));
 ushort labelIndex = 0;
 
 // Loop through the label histogram.
 for (int i = 0; i < labelCounter; i++) {
 // We can reuse the histogram pointer.
 if (tempLabelHist.at<int>(i) != 0) {
 labelCompressLUT.at<int>(i) = labelIndex;
 labelCompressLUT.at<int>(labelIndex) = labelIndex;
 cout << i << " labels will be reassigned to " << labelIndex << endl;
 labelIndex++;
 
 }
 }
 
 
 // Apply the label compression LUT manually.
 labelHist = Mat(1, regionCount+1, CV_32SC1, Scalar(0));
 for (int i = 0; i < r; i++) {
 ushort *rowPointer = reg.ptr<ushort>(i);
 for (int j = 0; j < c; j++) {
 if(rowPointer[i] != static_cast<ushort>(0)) {
 rowPointer[i] = labelCompressLUT.at<int>(rowPointer[i]);    // Assign the compressed-version of the label.
 labelHist.at<int>(rowPointer[i])++;     // Increment occurance count for that label within the histogram.
 }
 else {
 labelHist.at<int>(0)++;
 }
 }
 }
 */




/*
 //
 //  Grid.cpp
 //  DexVisionCPP
 //
 //  Created by Alexander Eden on 3/11/14.
 //  Copyright (c) 2014 Alexander Eden. All rights reserved.
 //
 
 #include "Grid.h"
 
 
 Mat Grid::getMemberMat(char which) {
 switch(which) {
 case '1':
 return orig;
 break;
 case 'b':
 return bin;
 break;
 case 'r':
 return reg;
 break;
 case 'h':
 return histGenerator.getHistogramImage(reg);
 break;
 default:
 return orig;
 
 }
 }
 
 
 
 void Grid::regionalizeBin() {
 
 // Reset the region matrix.
 reg = bin.clone();
 cout << CV_16UC1 << endl;
 
 // Initialize a LUT.
 int dim(256);
 Mat unionTable(1, &dim, CV_8U);
 for (int i = 0; i < 256; i++) {
 unionTable.at<uchar>(i) = i;
 
 }
 
 // Create the Disjoint Set Forest
 DisjointSets sets(1);
 
 
 // Scan Bounds
 int rowBound = reg.rows - 1;
 int colBound = reg.cols - 1;
 
 // First Pass Neighbors
 int labelCounter = 0;
 int intlabelCounter = 0;
 uchar N, NW, NE, W;
 
 // Go by rows.
 for (int j = 1; j < rowBound; j++) {
 
 // Pointer to the current row.
 const uchar * currRow = reg.ptr<const uchar>(j);
 
 // Pointer to the row above the current row.
 const uchar * prevRow = reg.ptr<const uchar>(j - 1);
 
 // Output Pointer
 uchar * output = reg.ptr<uchar>(j);
 
 
 // Then by columns.
 for(int i = 1; i < colBound; i++) {
 // Grab the neighbors.
 N = prevRow[i];
 NW = prevRow[i - 1];
 NE = prevRow[i + 1];
 W = currRow[i - 1];
 
 // Is the pixel not empty?
 if(currRow[i] == 255) {
 // Check the neighbors.
 if(N == 0 && NE == 0 && W == 0 && NW == 0) {
 // No neighbors, new region!
 labelCounter++;        // Increment region counter.
 intlabelCounter++;
 sets.AddElements(1);
 output[i] = saturate_cast<uchar>(labelCounter);
 //cout << intlabelCounter << endl;
 
 }
 else {
 // Has at least one neighbor. Give it the value of the lowest one.
 uchar smallest = 255;
 
 // Find the lowest label value of the neighboring pixels.
 smallest = (N > 0 && N < smallest ? N : smallest);
 smallest = (NE > 0 && NE < smallest ? NE : smallest);
 smallest = (NW > 0 && NW < smallest ? NW : smallest);
 smallest = (W > 0 && W < smallest ? W : smallest);
 
 // Record any new equivalences in the union LUT.
 if(N > 0) {
 sets.Union(sets.FindSet(N), sets.FindSet(smallest));
 //unionTable.at<uchar>(N) = min(unionTable.at<uchar>(N), smallest);
 
 }
 if(NE > 0) {
 sets.Union(sets.FindSet(NE), sets.FindSet(smallest));
 //unionTable.at<uchar>(NE) = min(unionTable.at<uchar>(NE), smallest);
 
 }
 if(W > 0) {
 sets.Union(sets.FindSet(W), sets.FindSet(smallest));
 // unionTable.at<uchar>(W) = min(unionTable.at<uchar>(W), smallest);
 
 }
 if(NW > 0) {
 sets.Union(sets.FindSet(NW), sets.FindSet(smallest));
 //unionTable.at<uchar>(NW) = min(unionTable.at<uchar>(NW), smallest);
 }
 
 
 
 output[i] = saturate_cast<uchar>(smallest);
 
 }
 
 }
 
 }
 }
 
 // Second pass. Make it quick.
 // Obtain the iterator for the initial position.
 MatIterator_<Vec3b> iterator = reg.begin<Vec3b>();
 
 // Obtain the end position.
 MatIterator_<Vec3b> end = reg.end<Vec3b>();
 
 // Loop over all of the pixels.
 for ( ; iterator != end; ++iterator) {
 // Process each pixel by channel.
 (*iterator)[0] = sets.FindSet((*iterator)[0]);
 }
 
 
 // Build the LUT according to the values of the disjoint sets.
 for (int i = 0; i < sets.NumElements(); ++i) {
 unionTable.at<uchar>(i) = sets.FindSet(i);
 }
 
 // Apply the Disjointed-Set Lookup Table
 //LUT(reg, unionTable, reg);
 
 
 }
 
 
 */
