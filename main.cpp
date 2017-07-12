#include "opencv2/core/utility.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <stdio.h>
#include <iostream>
using namespace cv;
using namespace std;
// http://docs.opencv.org/2.4/doc/tutorials/imgproc/shapedescriptors/bounding_rotated_ellipses/bounding_rotated_ellipses.html
// https://cci.lbl.gov/cctbx_sources/spotfinder/core_toolbox/hough.cpp

Mat src_gray;
int thresh = 64; // Starting threshold
int max_thresh = 255;
RNG rng(12345);

// Funciton header
void thresh_callback(int, void* );

int main( int argc, const char** argv )
{
    String filename = "/Users/derekfulton/Documents/yeast/cv_yeast/cv_yeast/transes/T7.tif";
    // Print message
    std::cout << "Displaying: " << filename << endl;
    
    // Read the image in grayscale
    src_gray = imread(filename, CV_LOAD_IMAGE_GRAYSCALE);
    //    cvtColor( src_gray, src_gray, CV_BGR2GRAY );
    blur( src_gray, src_gray, Size(1,1) );
    
    
    // Make window for the source
    char* source_window = "Source";
    namedWindow( source_window, CV_WINDOW_AUTOSIZE );
    
    // Show it
    imshow( source_window, src_gray);
    
    // Make slider
    createTrackbar( " Threshold:", source_window, &thresh, max_thresh, thresh_callback);
    thresh_callback(0, 0);
    
    waitKey(0);
    return(0);
}

/** @function thresh_callback */
void thresh_callback(int, void* )
{
    // Declare variables thresh output, contours and hierarchy
    Mat threshold_output; Mat threshold_intermediate; Mat canny_output; Mat og_output;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    
    std::cout << "thresh is: " << thresh << endl;
    
//    // Detect edges using threshold (try canny later?)
//    threshold( src_gray, canny_output, thresh, 255, THRESH_BINARY);
    
        // Detect edges using canny BUT STILL CALL IT threshold_output
        Canny( src_gray, canny_output, thresh, thresh*2, 3 );
    
    
    // Find contours
    findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
    
    // FIND ELLIPSES
    
    // Find the rotated rectangles and ellipses for each contour (later try one or the other alone)
    vector<RotatedRect> minRect ( contours.size() );
    vector<RotatedRect> minEllipse ( contours.size() );
    
    // Make the ellipess
    for( int i = 0; i < contours.size(); i++ )
    {
        minRect[i] = minAreaRect( Mat(contours[i]) );
        std::cout << contours[i].size() << endl;
        if( contours[i].size() > 5)
        {
            minEllipse[i] = fitEllipse( Mat(contours[i]) );
        }
    }
    
    // Draw contours and rotated rectangles and ellipses
  
    vector<vector<Point> >hull(contours.size());
    vector<vector<int> > hullsI(contours.size()); // Indices to contour points
    vector<vector<Vec4i>> defects(contours.size());
    
    Mat canny_drawing = imread("/Users/derekfulton/Documents/yeast/cv_yeast/cv_yeast/transes/T7.tif", CV_LOAD_IMAGE_COLOR);
    for( int i = 0; i < contours.size(); i++)//contours.size(); i++ )
    {
        // ONLY DRAW IT IF IT MEETS THE RIGHT AREA
        Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
        
        
        // Make sure the ellipse is what we want
        float major = minEllipse[i].size.width;
        float minor = minEllipse[i].size.height;
        float area = minEllipse[i].size.area();
        float aspect_ratio = major / minor;
        
        convexHull(contours[i], hull[i], false);
        convexHull(contours[i], hullsI[i], false);
        if(hullsI[i].size() > 3 ) // You need more than 3 indices
        {
            convexityDefects(contours[i], hullsI[i], defects[i]);
        }
        
        float min_area = 600;
        float max_area = 15000.00;
        
        
        if( area < min_area || area > max_area || aspect_ratio < 0.5) {
            
        }
        else
        {
//            // Contour
//            drawContours(canny_drawing, contours, i, color, 1, 8, vector<Vec4i>(), 0, Point());
//            
            // Ellipse
            ellipse(canny_drawing, minEllipse[i], color, 2, 8);
//
            
//            // Rotated rectangle
//            Point2f rect_points[4];
//            minRect[i].points(rect_points);
//            for (int j = 0; j < 4; j++)
//                line(canny_drawing, rect_points[j], rect_points[(j + 1) % 4], color, 1, 8);
        }
    }
    
    // Show in a window
    namedWindow( "Canny Contours", CV_WINDOW_FREERATIO );
    imshow( "Canny Contours", canny_drawing );
    
    // Show threshold
    namedWindow( "Canny", CV_WINDOW_FREERATIO );
    imshow( "Canny", canny_output);
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
