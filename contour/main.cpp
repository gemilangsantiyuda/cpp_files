#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <math.h>
#include <iostream>
#include <algorithm>
using namespace cv;
using namespace std;


bool cmp(vector<Point> l, vector<Point> r){
  Moments m1 = moments(Mat(l));    
  Moments m2 = moments(Mat(r));
  
  double area1=0,cx1=0,cy1=0;
  double area2=0,cx2=0,cy2=0;
  
  area1 = m1.m00;
  cx1 = m1.m10/area1;
  cy1 = m1.m01/area1;
  area2 = m2.m00;
  cx1 = m2.m10/area2;
  cx2 = m2.m01/area2;
  
  if (cx1<cx2){ 
    return true;
  } else if (cx1==cx2) {
    return cy1>cy2;
  }
  return false;  
}

vector<Point> combineContour(vector<vector<Point>> contours, double min_area_size, double min_distance){  
  vector<Point> contour_point;
  contour_point.clear();
  
  double area1=0,cx1=0,cy1=0;
  double area2=0,cx2=0,cy2=0;
  
  vector<vector<Point>> contours_min_area;
  vector<vector<Point>> contours_selected;
  contours_min_area.clear();
  contours_selected.clear();
  for(size_t i=0;i<contours.size();i++){
    if((contourArea(contours[i])>min_area_size)){
      contours_min_area.push_back(contours[i]);
    }
  }
    
  //sort(contours_min_area.begin(),contours_min_area.end(),cmp);
  //contours_selected.push_back(contours_min_area[0]);
  
  for(size_t i=1;i<contours_min_area.size();i++){
    //Moments m1 = moments(Mat(contours_min_area[i]));
    //Moments m2 = moments(Mat(contours_selected[contours_selected.size()-1]));
    //area1 = m1.m00;
    //cx1 = m1.m10/area1;
    //cy1 = m1.m01/area1;
    //area2 = m2.m00;
   // cx1 = m2.m10/area2;
    //cx2 = m2.m01/area2;
    //if(sqrt((cx1-cx2)*(cx1-cx2)+(cy1-cy2)*(cy1-cy2))<min_distance){
      contours_selected.push_back(contours_min_area[i]);
    //}
  }
  
  for(size_t i=0;i<contours_selected.size();i++){
    for(size_t j=0;j<contours_selected[i].size();j++){
      contour_point.push_back(contours_selected[i][j]);
    }
  }
  cout<<contour_point;
  return contour_point;
}

Mat boundaryDetect(Mat frame){
  Mat mat_HSV;
  Mat mat_thresh(frame.size(),CV_8UC1);
  Mat hasil(frame.size(),CV_8UC3);
  
  cvtColor(frame,mat_HSV,CV_BGR2HSV);
  inRange(mat_HSV,Scalar(43,108,0),Scalar(59,166,141),mat_thresh);
  
  vector<vector<Point>> contours;
  vector<Vec4i> hierarchy;
  
  findContours(mat_thresh,contours,hierarchy,CV_RETR_TREE,CV_CHAIN_APPROX_SIMPLE);
  
  vector<Point> titik_kontur = combineContour(contours,250,frame.cols/2);
  vector<Point> titik_hull;
    
  if(!titik_kontur.size()){
    return hasil; //kalau tidak ada kontur yang terdeteksi lakukan sesuatu
  } 
  convexHull(titik_kontur,titik_hull);
  
  Mat draw = Mat::zeros(frame.size(),CV_8UC1);
  Mat hvs = Mat::zeros(frame.size(),CV_8UC3);
  Mat transHVS(frame.size(),CV_8UC1);
  polylines(draw,titik_hull,true,Scalar::all(255),2);

  vector<vector<Point>> contours2;
  
  int batas_lapangan[frame.cols];
  
  findContours(draw,contours2,hierarchy,CV_RETR_LIST,CV_CHAIN_APPROX_SIMPLE);
  drawContours(hvs,contours2,0,Scalar::all(255),CV_FILLED);  
  cvtColor(hvs,transHVS,CV_BGR2GRAY);
  transpose(transHVS,transHVS);
  
  
  for(int i=0;i<transHVS.rows;i++){
    uchar *thPtr = transHVS.ptr<uchar>(i);
    batas_lapangan[i]=transHVS.cols-1;
    for(int j=0;j<transHVS.cols;j++){
      if(thPtr[j]){
        batas_lapangan[i]=j;
        break;
      }
    }
  }
  divide(255,hvs,hvs);
  multiply(frame,hvs,hasil);  
  return hasil;
}

int main(){
  VideoCapture cap("movie.mp4");
  if(!cap.isOpened()){
    cerr<<"DEBUG"<<endl;
    return -1;
  }

  while(1){
    Mat frame; cap>>frame;
     Mat fieldBoundary = boundaryDetect(frame);
    if(waitKey(10)==27)break;
    imshow("hsl",fieldBoundary);
    //imshow("original",frame);
  }
}
