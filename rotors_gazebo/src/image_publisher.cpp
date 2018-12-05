#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>

#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>
#include <ctype.h>
#include <algorithm>
#include <iterator>
#include <ctime>
#include <string>
#include <stdio.h>
#include <stdexcept>

#include "opencv2/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/aruco.hpp"
#include "opencv2/aruco/dictionary.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/video/tracking.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "cvdrawingutils.h"

using namespace std;
using namespace cv;
using namespace aruco;

const float arucoSquareDimension = 0.40f;
const Size chessboardDimensions = Size(6,9);

//default capture width and height
const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;

// create the detector o set parameters 
//aruco::MarkerDetector MDetector;
// // video capture
// VideoCapture TheVideoCapturer;
// // Create the vector marker to set the parameters of The Markers
// vector<Marker> TheMarkers;
// // create image frame of video of type Mat
// Mat TheInputImage, TheInputImageCopy, TheInputImageThCopy;
// // create the camera parameters to set ArUrco
// CameraParameters TheCameraParameters;

// // use a map so that for each id, we use a different pose tracker
// std::map<uint32_t, MarkerPoseTracker> MTracker; 


void imageCallback(const sensor_msgs::ImageConstPtr& msg)
{
  try
  {

  	//-- criando matrizes de imagem
    Mat src = cv_bridge::toCvShare(msg, "bgr8")->image;



    putText(src, "X", 
    	Point(src.cols / 2, src.rows / 2), 1.2, 1.2, Scalar(0, 0, 255), 2);

    
    //-- Imgem size is 856 x 480
    //cout << " img-size: " << src.size() << endl;

    //-- mostra imagem aruco
    cv::imshow("Img-Aruco", src);

    cv::waitKey(30);

  }
  catch (cv_bridge::Exception& e)
  {
    ROS_ERROR("Could not convert from '%s' to 'bgr8'.", msg->encoding.c_str());
  }
}

int main(int argc, char **argv)
{
  

  ros::init(argc, argv, "image_listener");
  ros::NodeHandle nh;
  //cv::namedWindow("ImgGray");
  cv::namedWindow("Img-Aruco", WINDOW_AUTOSIZE);
  cv::startWindowThread();

  image_transport::ImageTransport it(nh);
  image_transport::Subscriber sub = it.subscribe("bebop2/camera_base/image_raw", 1, imageCallback);

  ros::spin();
  cv::destroyWindow("Img-Aruco");


}

