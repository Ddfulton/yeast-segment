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
int thresh = 4; // Starting threshold
int max_thresh = 100;
RNG rng(12345);

// Function declarations
void thresh_callback(int, void* );
vector<int> classifyEllipses(vector<RotatedRect> ellipses, vector<int> neighbor_counts);
bool hasNeighbor(RotatedRect ellipse, vector<RotatedRect> ellipses);


int main( int argc, const char** argv )
{
//    String filename = "/Users/derekfulton/Documents/yeast/cv_yeast/cv_yeast/transes/T10.tif";
    String filename = argv[1];
    src_gray = imread(filename, CV_LOAD_IMAGE_GRAYSCALE);

    blur( src_gray, src_gray, Size(1,1) ); // Gaussian blur
    
    String source_window = "Source";
    
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
            
            if(aspect_ratio < 0.4 || area < 2000 || prospectiveEllipse.size.height > 130){
                // Doesn't make the cut
            }
            else {
                minRect[i] = minAreaRect( Mat(contours[i]) );
                minEllipse[i] = fitEllipse( Mat(contours[i]) );
            }
        }
    }
    
    Mat overlay = imread(argv[1], CV_LOAD_IMAGE_COLOR);
    
    
    vector<int> initial_counts(minEllipse.size());
    
    // Make intial counts 0 because we start with zero neighbors
    for (int i = 0; i < minEllipse.size(); i++) {
        initial_counts[i] = 0;
    }
    
    vector<int> neighbor_counts = classifyEllipses(minEllipse, initial_counts);
    Scalar color;
    
    for( int i = 0; i < minEllipse.size(); i++) {
        
        
        
        
        //            // Contour
        //            drawContours(canny_drawing, contours, i, color, 1, 8, vector<Vec4i>(), 0, Point());
        //
        // Ellipse
        if (neighbor_counts[i] == 0) {
            color = Scalar(1, 0, 0); // blue
        }
        else {
            color = Scalar(0, 255, 0); // green
            ellipse(overlay, minEllipse[i], color, 2, 8);
        }
        
        //        std::string label = std::to_string(i);
        //
        //        putText(overlay, label, cvPoint(minEllipse[i].center.x, minEllipse[i].center.y),
        //                FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(200,200,250), 1, CV_AA);
        
        //            ellipse(overlay, minEllipse[i], color, 2, 8);
        
        
        //            // Rotated rectangle
        //            Point2f rect_points[4];
        //            minRect[i].points(rect_points);
        //        for (int j = 0; j < 4; j++) {
        //                line(overlay, rect_points[j], rect_points[(j + 1) % 4], color, 1, 8);
        //        }
    }
    
    // Show everything
    namedWindow( "Canny Contours", CV_WINDOW_FREERATIO );
    imshow( "Canny Contours", overlay );
    //    namedWindow( "Canny", CV_WINDOW_FREERATIO );
    //    imshow( "Canny", canny_output);

    waitKey(0);
    
    return(0);
}

/** @function thresh_callback */
void thresh_callback(int, void* )
{
    // Declare

    
}

vector<int> classifyEllipses(vector<RotatedRect> ellipses, vector<int> neighbor_counts) {
    vector<Scalar> colors (ellipses.size());
    // (BLUE, GREEN, RED)
//    Scalar red = Scalar(0, 0, 255);
//    Scalar green = Scalar(0, 255, 0);
//    Scalar blue = Scalar(255, 0, 0);
    for (int i = 0; i < ellipses.size(); i++) {
        RotatedRect E1 = ellipses[i];
        if (hasNeighbor(E1, ellipses)) { // If it meets the neighbor count criteria
            neighbor_counts[i] = neighbor_counts[i] + 1;
            continue;
        }
        else {
            continue;
        }
    }
    
    return neighbor_counts;
    
}

bool hasNeighbor(RotatedRect ellipse, vector<RotatedRect> ellipses) {
    bool result = false;
    float height = ellipse.size.height;
    float width = ellipse.size.width;
    Point2f center = ellipse.center;
    float angle = ellipse.angle;
    
    for (int i = 0; i < ellipses.size(); i++) {
        float curr_height = ellipses[i].size.height;
        float curr_width = ellipses[i].size.width;
        Point2f curr_center = ellipses[i].center;
        float curr_angle = ellipses[i].angle;
        float dist = sqrt( (ellipse.center.x - ellipses[i].center.x)*(ellipse.center.x - ellipses[i].center.x) + (ellipse.center.y - ellipses[i].center.y)*(ellipse.center.y - ellipses[i].center.y) );
        float unweighted_difference_index = (1/4)*abs(height - curr_height) + (1/4)*abs(width - curr_width) + 10*dist + 10*abs(angle - curr_angle);
        
        
        
        if (ellipse.center == ellipses[i].center) { // If it's the same ellipse
            continue;
        }
        else {
            if ( unweighted_difference_index < 500) { // If it's 10 away and E1/E2 are relatively similar
                result = true;
                continue;
            }
            else {
                continue;
            }
        }
        
        
    }
    return result;
}













/*
 THIS IS THE ITALIAN GUY'S ALGORITHM
 */

//#include "opencv2/imgproc.hpp"
//#include "opencv2/highgui.hpp"
//#include <stdlib.h>
//#include <stdio.h>
//#include <iostream>
//#include <algorithm>
//#include <vector>
//#include <stack>
//
//using namespace cv;
//using namespace std;
//
//int kernel_size = 3;
//int lowThreshold = 11;
//
//
//
//
///** @function main */
//int main( int argc, char** argv )
//{
//    
//    Mat src, src_gray;
//    Mat grad;
//    int scale = 1;
//    int delta = 0;
//    int ddepth = CV_16S;
//    
//    int c;
//    
//    /// Load an image
//    src = imread( "/Users/derekfulton/Documents/yeast/cv_yeast/cv_yeast/T5.jpg" );
//    
//    if( !src.data )
//    { return -1; }
//    
//    GaussianBlur( src, src, Size(3,3), 0, 0, BORDER_DEFAULT );
//    
//    /// Convert it to gray
//    cvtColor( src, src_gray, COLOR_RGB2GRAY );
//    
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
//    
//    Mat labels;
//    connectedComponents(edges, labels, 8, CV_32S);
//    Mat labels_binary;
//    labels_binary = labels > 1;
//    std::cout << labels_binary << std::endl;
//
//    
//    namedWindow("DYDX", CV_WINDOW_AUTOSIZE);
//    imshow("DYDX", dydx);
//    
//    namedWindow("Canny", CV_WINDOW_AUTOSIZE);
//    imshow("Canny", edges);
//    
//
//    namedWindow("Labels", CV_WINDOW_AUTOSIZE);
//    imshow("Labels", labels);
//    
//    
//    waitKey(0);
//    
//    return 0;
//}
