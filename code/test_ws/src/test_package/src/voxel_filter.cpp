#include <ros/ros.h>
# include <iostream>
// PCL specific includes
#include <sensor_msgs/PointCloud2.h>
#include <pcl_conversions/pcl_conversions.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/filters/voxel_grid.h>
#include <pcl/filters/passthrough.h>
ros::Publisher pub;
double voxel_size =0.01;
void cloud_filter(const sensor_msgs::PointCloud2ConstPtr& input)
{
  // Container
  pcl::PCLPointCloud2* cloud = new pcl::PCLPointCloud2;
  pcl::PCLPointCloud2Ptr cloudPtr(cloud);
  pcl::PCLPointCloud2 cloud_filtered;

  pcl_conversions::toPCL(*input, *cloud);

  // Create the filtering object
  pcl::VoxelGrid<pcl::PCLPointCloud2> sor;
  sor.setInputCloud (cloudPtr);
  sor.setLeafSize(voxel_size, voxel_size, voxel_size);
  sor.filter (cloud_filtered);
  
  // Convert to ROS
  sensor_msgs::PointCloud2 cloud_vox;
  pcl_conversions::moveFromPCL(cloud_filtered, cloud_vox);
  pub.publish (cloud_vox);
}
int main (int argc, char** argv)
{
  // Initialize ROS
  std::cerr<<"Start to listen to /cloud_pcd\n";
  ros::init (argc, argv, "Final__Filter");
  ros::NodeHandle nh;
  // Create a ROS subscriber for the input point cloud
  ros::Subscriber sub = nh.subscribe<sensor_msgs::PointCloud2> ("outliner_filter", 1, cloud_filter);

  // Create a ROS publisher for the output point cloud
  pub = nh.advertise<sensor_msgs::PointCloud2> ("/final_filter", 1);

  // Spin
  ros::spin ();
}