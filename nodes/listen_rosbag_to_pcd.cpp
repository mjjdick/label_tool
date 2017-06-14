//
// Created by mjj on 17-6-14.
//
#include <ros/ros.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/conversions.h>
#include <pcl_conversions/pcl_conversions.h>
#include <ros/package.h>
#include <pcl/common/transforms.h>
#include <opencv2/opencv.hpp>

template<typename T>
std::string num2str(T num)
{
    std::stringstream ss;
    std::string st;
    ss << num;
    ss >> st;
    return st;
}

struct listen_rosbag_params{
    std::string dir_path_;
    std::string in_type_;
    std::string out_type_;
    int frame_id_;
    int grid_width_;
    int grid_height_;
    double grid_size_;
    double max_width_;
    double max_height_;
};

listen_rosbag_params g_trans_param;

void transSensorMsgToPCloud(const sensor_msgs::PointCloud2& msg,pcl::PointCloud<pcl::PointXYZI>::Ptr out_cloud_ptr);
cv::Point2i trans_grid(cv::Point2f in_point);

void fullscanCallback(const sensor_msgs::PointCloud2 msg){
    pcl::PointCloud<pcl::PointXYZI>::Ptr in_cloud_ptr(new pcl::PointCloud<pcl::PointXYZI>);
    transSensorMsgToPCloud(msg,in_cloud_ptr);

    cv::Mat trans_img;
    trans_img = cv::Mat::zeros(g_trans_param.grid_height_,g_trans_param.grid_width_,CV_8UC1);

    for (int i = 0; i < in_cloud_ptr->size(); ++i) {
        pcl::PointXYZI tmp_cloud_point = in_cloud_ptr->points[i];
        if (fabs(tmp_cloud_point.x) > g_trans_param.max_width_ || fabs(tmp_cloud_point.y) > g_trans_param.max_height_)continue;
        cv::Point2i tmp_cv_point = trans_grid(cv::Point2f(tmp_cloud_point.x,tmp_cloud_point.y));
        trans_img.at<uchar>(tmp_cv_point.y,tmp_cv_point.x) = 255;
    }
    std::string img_file = g_trans_param.dir_path_ + "/img/" + num2str(g_trans_param.frame_id_) + g_trans_param.out_type_;
    std::string pcd_file = g_trans_param.dir_path_ + "/pcd/" + num2str(g_trans_param.frame_id_) + g_trans_param.in_type_;
    cv::imwrite(img_file.c_str(),trans_img);
    pcl::io::savePCDFileBinary(pcd_file.c_str(), *in_cloud_ptr);
    g_trans_param.frame_id_++;
}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "manas_listen_node");
    ros::NodeHandle node;
    ros::NodeHandle private_nh("~");

    ros::Subscriber sub_fullscan = node.subscribe("/velodyne_points",10,fullscanCallback);

    std::string dir_path = "/home/mjj/roboTool/data";
    std::string in_type = ".pcd";
    std::string out_type = ".png";
    double max_width = 50.;
    double max_height = 50.;
    double grid_size = 0.1;
    int grid_width = (max_width * 2) / grid_size - 1;
    int grid_heigt = (max_height * 2) / grid_size - 1;

    cv::Mat trans_img;
    trans_img = cv::Mat::zeros(grid_heigt,grid_width,CV_8UC1);
    g_trans_param.dir_path_ = dir_path;
    g_trans_param.in_type_ = in_type;
    g_trans_param.out_type_ = out_type;
    g_trans_param.grid_width_ = grid_width;
    g_trans_param.grid_height_ = grid_heigt;
    g_trans_param.grid_size_ = grid_size;
    g_trans_param.max_height_ = max_height;
    g_trans_param.max_width_ = max_width;
    g_trans_param.frame_id_ = 0;

    ros::spin();

    return 0;
}

cv::Point2i trans_grid(cv::Point2f in_point){
    return cv::Point2i(int(in_point.x / g_trans_param.grid_size_) + g_trans_param.grid_width_ / 2, int(in_point.y / g_trans_param.grid_size_) + g_trans_param.grid_height_ / 2);
}

void transSensorMsgToPCloud(const sensor_msgs::PointCloud2& msg,pcl::PointCloud<pcl::PointXYZI>::Ptr out_cloud_ptr){
    pcl::PCLPointCloud2 pcl_pc2;
    pcl_conversions::toPCL(msg,pcl_pc2);
    pcl::fromPCLPointCloud2(pcl_pc2,*out_cloud_ptr);
}
