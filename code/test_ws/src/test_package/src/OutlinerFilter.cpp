#include <iostream>
#include <pcl/point_types.h>
#include <pcl/kdtree/kdtree_flann.h>
#include <pcl/surface/mls.h>
#include <sensor_msgs/PointCloud2.h>
#include <pcl_conversions/pcl_conversions.h>
#include <pcl/point_cloud.h>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/PCLPointCloud2.h>
#include <pcl/filters/statistical_outlier_removal.h>
ros::Publisher pub;
double std_threshold = 1.0;
int mean_k = 30;

void cloud_filter(const sensor_msgs::PointCloud2ConstPtr& input)
{
  // container
  sensor_msgs::PointCloud2 output;
  pcl::PCLPointCloud2* cloud = new pcl::PCLPointCloud2;
  pcl::PCLPointCloud2Ptr cloudPtr(cloud);
  pcl::PCLPointCloud2 cloud_filtered;

  pcl_conversions::toPCL(*input, *cloud);

  //pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud (new pcl::PointCloud<pcl::PointXYZRGB>);
  //pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_filtered (new pcl::PointCloud<pcl::PointXYZRGB>);


  // Create the filtering object
  pcl::StatisticalOutlierRemoval<pcl::PCLPointCloud2> outliner;
  outliner.setInputCloud (cloudPtr);
  outliner.setMeanK (mean_k);
  outliner.setStddevMulThresh (std_threshold);
  outliner.filter (cloud_filtered);

  // To sensor msg
  sensor_msgs::PointCloud2 cloud_pt;
  pcl_conversions::moveFromPCL(cloud_filtered, cloud_pt);

  //pcl::PCDWriter writer;
  //writer.write<pcl::PointXYZRGB> ("target_inliers.pcd", *cloud_filtered, false);
  //cloud_pt.header.frame_id = "pcd_frame";
  
  pub.publish(cloud_pt);
}
int main (int argc, char** argv)
{
  // Initialize ROS
  ros::init (argc, argv, "Outline_Filter");
  ros::NodeHandle nh;
  // Create a ROS subscriber for the input point cloud
  ros::Subscriber sub = nh.subscribe<sensor_msgs::PointCloud2> ("downsample_filter", 1, cloud_filter);
  std::cerr<<"Start to listen to /downsample_filter\n";

  // Create a ROS publisher for the output point cloud
  pub = nh.advertise<sensor_msgs::PointCloud2> ("/outliner_filter", 1);
  // Spin
  ros::spin ();
}