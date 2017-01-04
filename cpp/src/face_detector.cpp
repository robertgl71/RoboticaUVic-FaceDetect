/*
 * face_detector.cpp
 *
 *  Created on: 12 dic. 2016
 *      Author: robert
 */
//OpenCV
#include "opencv2/opencv.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"

//std
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

using namespace cv;
using namespace std;

/** Function Headers */
void detectAndDisplay( Mat frame, Mat moustache, Mat hat );

/** Global variables */
String face_cascade_name = "haarcascade_frontalface_default.xml";
CascadeClassifier face_cascade;
String window_name = "Face detection";

void addImage(cv::Mat image, cv::Mat frame, cv::Rect rect, int x_offset, int y_offset)
{
    int x, y;
    for (y=0; y<image.cols; y++)
    {
        for (x=0; x<image.rows; x++)
        {
            cv::Vec4b pixel = image.at<cv::Vec4b>(x,y);
            if (pixel[3] > 32)
            {
                frame.at<cv::Vec3b>(x+rect.y+x_offset,y+rect.x+y_offset)[0] = image.at<cv::Vec4b>(x,y)[0];
                frame.at<cv::Vec3b>(x+rect.y+x_offset,y+rect.x+y_offset)[1] = image.at<cv::Vec4b>(x,y)[1];
                frame.at<cv::Vec3b>(x+rect.y+x_offset,y+rect.x+y_offset)[2] = image.at<cv::Vec4b>(x,y)[2];
            }
        }
    }
}


int main(int argc, char* argv[])
{
	VideoCapture camera;
	int cam_id;
	Mat hat,moustache,frame,frame2,result;

	//check user args
	switch(argc)
	{
		case 1: //no argument provided, so try /dev/video0
			cam_id = 0;
			break;
		case 2: //an argument is provided. Get it and set cam_id
			cam_id = atoi(argv[1]);
			break;
		default:
			std::cout << "Invalid number of arguments. Call program as: webcam_capture [video_device_id]. " << std::endl;
			std::cout << "EXIT program." << std::endl;
			break;
	}

	std::cout << "Opening video device " << cam_id << std::endl;

	 //open the video stream and make sure it's opened
	if( !camera.open(cam_id) )
	{
		std::cout << "Error opening the camera. May be invalid device id. EXIT program." << std::endl;
		return -1;
	}

	//-- 1.Read the pictures
	moustache = imread("moustache.png",-1);   // Read the file
	if(! moustache.data )	{ cout <<  "Could not open or find moustache.png" << std::endl ; return -1; }

	hat = imread("hat.png",IMREAD_UNCHANGED);   // Read the file
	if(! hat.data )	{ cout <<  "Could not open or find the image" << std::endl ; return -1; }

	//-- 2. Load the cascades
	if( !face_cascade.load( face_cascade_name ) ){ printf("--(!)Error loading face cascade\n"); return -1; };

	while (1)
	    {
			bool bSuccess = camera.read(frame);

			   if (!bSuccess) //if not success, break loop
			   {
				   cout << "Cannot read the frame from video file" << endl;
				   break;
			    }
			cvtColor(frame,frame2,COLOR_BGR2BGRA);
	        detectAndDisplay( frame2 , moustache, hat);

	        int c = waitKey(10);
	        if( (char)c == 27 ) { break; } // escape
	    }
	    return 0;
}

/** @function detectAndDisplay */
void detectAndDisplay( Mat frame, Mat moustache, Mat hat )
{
	vector<Rect> faces;
    Mat frame_gray,mous_resize,frame_clone,hat_resize;

    cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
    equalizeHist( frame_gray, frame_gray );

    //-- Detect faces
    face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CASCADE_SCALE_IMAGE, Size(70, 80) );

    frame_clone = frame.clone();

    for( size_t i = 0; i < faces.size(); i++ )
    {
    	resize(moustache,mous_resize,faces[i].size());
    	resize(hat,hat_resize,faces[i].size());
    	rectangle(frame_clone, faces[i], cv::Scalar(255,0,0), 2);
    	addImage(mous_resize, frame_clone, faces[i], faces[i].height/4, faces[i].width/2);
    	addImage(hat_resize, frame_clone, faces[i], (-faces[i].height/10)*7, faces[i].width/2);
    }
    imshow( window_name, frame_clone);
}







