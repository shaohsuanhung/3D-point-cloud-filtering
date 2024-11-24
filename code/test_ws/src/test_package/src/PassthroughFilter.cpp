#include <iostream>
#include <pcl/point_types.h>
#include <pcl/filters/passthrough.h>
#include <sensor_msgs/PointCloud2.h>
#include <pcl_conversions/pcl_conversions.h>
#include <pcl/point_cloud.h>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/PCLPointCloud2.h>
// Remove the nonimpoartant part: ceiling and ground
ros::Publisher pub;
double lower_bound = -1.1;
double high_bound = 0.95;
void cloud_filter(const sensor_msgs::PointCloud2ConstPtr& input)
{
  // Container
  pcl::PCLPointCloud2* cloud = new pcl::PCLPointCloud2;
  pcl::PCLPointCloud2Ptr cloudPtr(cloud);
  pcl::PCLPointCloud2 cloud_filtered;

  pcl_conversions::toPCL(*input, *cloud);

  // Create the filtering object
  pcl::PassThrough<pcl::PCLPointCloud2> pass;
  pass.setInputCloud (cloudPtr);
  pass.setFilterFieldName("z");
  pass.setFilterLimits(lower_bound ,high_bound);
  pass.filter (cloud_filtered);
  
  // Convert to ROS msg
  sensor_msgs::PointCloud2 cloud_pt;
  pcl_conversions::moveFromPCL(cloud_filtered, cloud_pt);
  
  pub.publish (cloud_pt);
}
int main (int argc, char** argv)
{
  // Initialize ROS
  ros::init (argc, argv, "Passthrough_Filter");
  ros::NodeHandle nh;
  // Create a ROS subscriber for the input point cloud
  ros::Subscriber sub = nh.subscribe<sensor_msgs::PointCloud2> ("cloud_pcd", 1, cloud_filter);
  std::cerr<<"Start to listen to /cloud_pcd\n";

  // Create a ROS publisher for the output point cloud
  pub = nh.advertise<sensor_msgs::PointCloud2> ("/passthroughfilter", 1);
  // Spin
  ros::spin ();
}