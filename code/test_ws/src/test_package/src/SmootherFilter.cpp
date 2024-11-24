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
ros::Publisher pub;
double search_radious=0.1;

pcl::PointCloud<pcl::PointXYZRGB>::Ptr fromROSMsgRGB(const sensor_msgs::PointCloud2::ConstPtr& msg)
{
  pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZRGB>);
  pcl::PCLPointCloud2 pcl_pc2;
  pcl_conversions::toPCL(*msg, pcl_pc2);
  pcl::fromPCLPointCloud2(pcl_pc2,*cloud);
  return cloud;
};
sensor_msgs::PointCloud2::Ptr toROSMsg(pcl::PointCloud<pcl::PointXYZRGBNormal>& cloud)
{
  sensor_msgs::PointCloud2::Ptr msg(new sensor_msgs::PointCloud2);
  pcl::toROSMsg(cloud, *msg);
  return msg;
};

void cloud_filter(const sensor_msgs::PointCloud2ConstPtr& input)
{
  //std::cerr<<"Start filter...\n";
  // Container
  pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud = fromROSMsgRGB(input);

  pcl::search::KdTree<pcl::PointXYZRGB>::Ptr tree (new pcl::search::KdTree<pcl::PointXYZRGB>);

  // Output has the PointNormal type in order to store the normals calculated by MLS
  pcl::PointCloud<pcl::PointXYZRGBNormal> mls_points;

  // Init object (second point type is for the normals, even if unused)
  pcl::MovingLeastSquares<pcl::PointXYZRGB, pcl::PointXYZRGBNormal> mls;
 
  mls.setComputeNormals (true);

  // Set parameters
  mls.setInputCloud (cloud);
  mls.setPolynomialOrder (2);
  mls.setSearchMethod (tree);
  mls.setSearchRadius (search_radious);
  // Reconstruct
  mls.process (mls_points);
  
  // Convert to ROS msg
  sensor_msgs::PointCloud2::Ptr cloud_smth = toROSMsg(mls_points);

  pub.publish (cloud_smth);
}
int main (int argc, char** argv)
{
  // Initialize ROS
  ros::init (argc, argv, "Smoother_Filter");
  ros::NodeHandle nh;
  // Create a ROS subscriber for the input point cloud
  ros::Subscriber sub = nh.subscribe<sensor_msgs::PointCloud2> ("outliner_filter", 1, cloud_filter);
  std::cerr<<"Start to listen to /outliner_filter\n";

  // Create a ROS publisher for the output point cloud
  pub = nh.advertise<sensor_msgs::PointCloud2> ("/smoother_filter", 1);
  // Spin
  ros::spin ();
}