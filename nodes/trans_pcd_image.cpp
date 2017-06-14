/*
Author: mjj
Date: 2017/05/22
Copyright: Manas Team
*/

#include <opencv2/opencv.hpp>
#include <pcl/point_types.h>
#include <pcl/io/pcd_io.h>
#include <iostream>
#include <dirent.h>

struct params{
    std::string dir_path_;
    std::string in_type_;
    std::string out_type_;
    int grid_width_;
    int grid_height_;
    double grid_size_;
    double max_width_;
    double max_height_;
};

params g_trans_param;

void read_filelists(const std::string& dir_path,std::vector<std::string>& out_filelsits,std::string type);
std::string get_filename(const std::string& in_file);
cv::Point2i trans_grid(cv::Point2f in_point);

int main()
{
    std::string dir_path = "/home/mjj/roboTool/src/manas_cv_label/data";
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

    std::vector<std::string> filelists;
    read_filelists(dir_path + "/pcd/",filelists,in_type);
    for (int j = 0; j < filelists.size(); ++j) {
        std::string cur_file_name = get_filename(filelists[j]);
        std::string pcd_file = dir_path + "/pcd/" + filelists[j];

        pcl::PointCloud<pcl::PointXYZI>::Ptr in_cloud_ptr (new pcl::PointCloud<pcl::PointXYZI>);
        if (pcl::io::loadPCDFile<pcl::PointXYZI> (pcd_file.c_str(), *in_cloud_ptr) == -1) {
            PCL_ERROR ("Couldn't read file test_pcd.pcd \n"); //文件不存在时，返回错误，终止程序。
            return (-1);
        }
        trans_img.setTo(0);
        for (int i = 0; i < in_cloud_ptr->size(); ++i) {
            pcl::PointXYZI tmp_cloud_point = in_cloud_ptr->points[i];
            if (fabs(tmp_cloud_point.x) > max_width || fabs(tmp_cloud_point.y) > max_height)continue;
            cv::Point2i tmp_cv_point = trans_grid(cv::Point2f(tmp_cloud_point.x,tmp_cloud_point.y));
            trans_img.at<uchar>(tmp_cv_point.y,tmp_cv_point.x) = 255;
        }
        std::string img_file = dir_path + "/img/" + cur_file_name + g_trans_param.out_type_;
        cv::imwrite(img_file.c_str(),trans_img);
    }
}

cv::Point2i trans_grid(cv::Point2f in_point){
    return cv::Point2i(int(in_point.x / g_trans_param.grid_size_) + g_trans_param.grid_width_ / 2, int(in_point.y / g_trans_param.grid_size_) + g_trans_param.grid_height_ / 2);
}

std::string get_filename(const std::string& in_file){
    std::string tmp_filename = in_file.substr(0,in_file.size() - g_trans_param.in_type_.size());
    return tmp_filename;
}

void read_filelists(const std::string& dir_path,std::vector<std::string>& out_filelsits,std::string type){
    struct dirent *ptr;
    DIR *dir;
    dir = opendir(dir_path.c_str());
    out_filelsits.clear();
    while ((ptr = readdir(dir)) != NULL){
        std::string tmp_file = ptr->d_name;
        if (tmp_file[0] == '.')continue;
        if (type.size() <= 0){
            out_filelsits.push_back(ptr->d_name);
        }else{
            if (tmp_file.size() < type.size())continue;
            std::string tmp_cut_type = tmp_file.substr(tmp_file.size() - type.size(),type.size());
            if (tmp_cut_type == type){
                out_filelsits.push_back(ptr->d_name);
            }
        }
    }
}


