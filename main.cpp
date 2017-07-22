#include "opencv2/core/utility.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <stdio.h>
#include <iostream>
#include <string>
using namespace cv;
using namespace std;
// http://docs.opencv.org/2.4/doc/tutorials/imgproc/shapedescriptors/bounding_rotated_ellipses/bounding_rotated_ellipses.html
// https://cci.lbl.gov/cctbx_sources/spotfinder/core_toolbox/hough.cpp


Mat src_gray;
int thresh = 50; // Starting threshold
int max_thresh = 100;
RNG rng(12345);

// Function declarations
void thresh_callback(int, void* );
vector<int> countLowHangingCousins(vector<RotatedRect> ellipse_set, vector<int> empty_vector);
void processImage(string filename);
void thresh_callback(int, void* );


int main( int argc, const char** argv )
{
    processImage("/Users/derekfulton/Documents/yeast/cv_yeast/cv_yeast/yeast_testing/big_tiffs_from_josh/big_tiff_1_BP_FijiEdge.tif");




    waitKey(0);
    
    return(0);
}

void processImage(string filename) {
    
    src_gray = imread(filename, CV_LOAD_IMAGE_GRAYSCALE);
    
    blur( src_gray, src_gray, Size(1,1) ); // Gaussian blur
    
    string source_window = "Source";
    
    //    namedWindow( source_window, CV_WINDOW_AUTOSIZE );
    //
    //    imshow( source_window, src_gray);
    
    Mat canny_output;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    
    Canny( src_gray, canny_output, thresh, thresh*8, 3 ); // Edges
    
    findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0)); // Contours
    
    vector<RotatedRect> minRect ( contours.size() ); // Vector of RotatedRects called minRect
    vector<RotatedRect> minEllipse ( contours.size() ); // Vector of RotatedRects called minEllipse
    
    
    // Make one ellipse per contour, not vetted yet
    for( int i = 0; i < contours.size(); i++ ) {
        if ( contours[i].size() < 5 ) {
            // Not big enough to fit the ellipse
        }
        else {
            RotatedRect prospectiveEllipse = fitEllipse(Mat(contours[i]));
            // Features of each fitted ellipse
            float major = prospectiveEllipse.size.width;
            float minor = prospectiveEllipse.size.height;
            float area = prospectiveEllipse.size.area();
            float aspect_ratio = major / minor;
            // Validate
            
            float min_area = 200;
            float max_area = 30000.00;
            
            if(aspect_ratio < 0.4 || area < 400 || prospectiveEllipse.size.height > 130){
                // Doesn't make the cut
            }
            else {
                minRect[i] = minAreaRect( Mat(contours[i]) );
                minEllipse[i] = fitEllipse( Mat(contours[i]) );
            }
        }
    }
    
    Mat overlay = imread(filename, CV_LOAD_IMAGE_COLOR);
    
    
    vector<int> initial_counts(minEllipse.size());
    
    // Make intial counts 0 because we start with zero neighbors
    for (int i = 0; i < minEllipse.size(); i++) {
        initial_counts[i] = 0;
    }
    
    vector<int> neighbor_counts = countLowHangingCousins(minEllipse, initial_counts);
    
    for (int i = 0; i < neighbor_counts.size(); i++) {
    }
    
    Scalar color;
    
    for( int i = 0; i < minEllipse.size(); i++) {
        if (neighbor_counts[i] == 0) {
            color = Scalar(255, 0, 0); // blue
            ellipse(overlay, minEllipse[i], color, 2, 8);
            
        }
        else {
            color = Scalar(0, 255, 0); // green
            ellipse(overlay, minEllipse[i], color, 2, 8);
        }
        
        //        std::string label = std::to_string(i);
        //
        //        putText(overlay, label, cvPoint(minEllipse[i].center.x, minEllipse[i].center.y),
        //                FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(200,200,250), 1, CV_AA);
        
        
        
        
        
        //            // Rotated rectangle
        //            Point2f rect_points[4];
        //            minRect[i].points(rect_points);
        //        for (int j = 0; j < 4; j++) {
        //                line(overlay, rect_points[j], rect_points[(j + 1) % 4], color, 1, 8);
        //        }
    }
    
    // Show everything
    string title = filename.substr(filename.length() - 10, filename.length());
    namedWindow( title, CV_WINDOW_FREERATIO );
    imshow( title, overlay );
    
//    namedWindow( "Canny", CV_WINDOW_FREERATIO );
//    imshow( "Canny", canny_output);
}

vector<int> countLowHangingCousins(vector<RotatedRect> ellipse_set, vector<int> neighbor_counts) {
    for (int i = 0; i < ellipse_set.size(); i++) {
        int current_count = 0;
        RotatedRect E = ellipse_set[i];
        
        for (int j = 0; j < ellipse_set.size(); j++) {
            
            float dist = sqrt( pow(E.center.x - ellipse_set[j].center.x, 2) + pow((E.center.y - ellipse_set[j].center.y), 2));
            
            if (dist == 0) {
                // Same ellipse
            }
            else {
                if (dist < 10) {
                    current_count = current_count + 1;
                }
                else {
                    continue;
                }
            }
        
        }
        
        neighbor_counts[i] = current_count;
    }
    return neighbor_counts;
}

/** @function thresh_callback */
void thresh_callback(int, void* ){};

// ******************* ORIGINAL SCRIPT ENDS HERE ***************** //
















/* This code will help calculate the Sobel derivative for
 * each edge pixel. It will come in handy later after we
 * have removed the low hanging fruit and need to identify
 * smaller buds in the yeast.
 */

//    // Scoop edges
//    Mat edges;
//    Canny( src_gray, edges, lowThreshold, lowThreshold*3, kernel_size );
//    
//    /// Generate grad_x and grad_y
//    Mat grad_x, grad_y;
//    Mat abs_grad_x, abs_grad_y;
//    
//    /// Gradient X
//    //Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
//    Sobel( src_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT );
//    convertScaleAbs( grad_x, abs_grad_x );
//    
//    /// Gradient Y
//    //Scharr( src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
//    Sobel( src_gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT );
//    convertScaleAbs( grad_y, abs_grad_y );
//    
//    /// Total Gradient (approximate)
//    addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad );
//    
//    Mat slopes;
//    slopes = grad_y.mul(1/grad_x);
//    
//    Mat dydx;
//    dydx = slopes < 1;

