//
// Created by mjj on 17-6-14.
//

#ifndef PROJECT_LABEL_TOOL_H
#define PROJECT_LABEL_TOOL_H

#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <dirent.h>

/**************************
1. Please move mouse to draw rectangle of car or ped.
2. PRESS 'y' to label next figure.
'n' to label current figure again.
'c' to label current figure car
'p' to label current fgure ped
3. Press 'q' to quit this program.
**************************/

template<typename T>
std::string num2str(T num)
{
    std::stringstream ss;
    std::string st;
    ss << num;
    ss >> st;
    return st;
}

struct LabelInfo{
    cv::Point2f leftPt;
    cv::Point2f rightPt;
    std::string label;
};

struct params{
    std::string dir_path_,label_file_,type_;
    int fram_id_,cur_file_name_;
    cv::Mat g_img_,g_disp_,g_disp_mouse_move_;
    int g_flag_;
    cv::Point g_pt1_, g_pt2_;
    std::map<int,std::vector<LabelInfo> > label_infos_;
};

params g_param;

void initParams(const std::string& dir_path,const std::string& type);
void read_label_text(const std::string& dir_path);
void read_filelists(const std::string& dir_path,std::vector<std::string>& out_filelsits,std::string type);
void label_img(const std::vector<std::string>& in_filelists);
void mouse_action(int event, int x, int y, int flags, void* param);
void drawCrossLine(cv::Mat& disp, int x, int y, cv::Scalar color);
void drawCoor(cv::Mat& disp, int x, int y, cv::Scalar color);
void drawRect(cv::Mat& disp, cv::Point pt1, cv::Point pt2, cv::Scalar color, int thick);
void draw_img(const std::vector<std::string>& in_filelists, const int& frame_id);
void draw_label(cv::Mat& disp,std::string str,int x,int y,cv::Scalar color);
std::string get_filename(const std::string& in_file);
void save_label_text();

#endif //PROJECT_LABEL_TOOL_H
