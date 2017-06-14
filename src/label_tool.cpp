//
// Created by mjj on 17-6-14.
//

#include "label_tool.h"

void initParams(const std::string& dir_path,const std::string& type){
    g_param.dir_path_ = dir_path;
    g_param.label_file_ = dir_path + "/labe.txt";
    g_param.type_ = type;
    g_param.fram_id_ = 0;
    g_param.label_infos_.clear();
    g_param.g_flag_ = 0;

    //--------read label infos
    read_label_text(dir_path);

    //--------read data lists
    std::vector<std::string> file_lists;
    read_filelists(dir_path,file_lists,type);

    if (file_lists.empty()){
        std::cout<<"there is no img in this dir"<<std::endl;
    }

    //-------label image
    label_img(file_lists);
}

void label_img(const std::vector<std::string>& in_filelists){
    int keyval = 0;
    cv::namedWindow("label_tool");
    cv::setMouseCallback("label_tool", mouse_action, 0);

    draw_img(in_filelists,g_param.fram_id_);
    while (keyval != 'q')
    {
        keyval = cv::waitKey(10);
        //Next
        if (keyval == 'n')
        {
            g_param.fram_id_++;
            if (g_param.fram_id_ >= in_filelists.size())break;
            draw_img(in_filelists,g_param.fram_id_);
        }
        //Label
        if (g_param.g_flag_ == 1 && keyval == '1'){
            LabelInfo cur_label;
            cur_label.leftPt = g_param.g_pt1_;
            cur_label.rightPt = g_param.g_pt2_;
            cur_label.label = "car";
            g_param.label_infos_[g_param.cur_file_name_].push_back(cur_label);
            draw_img(in_filelists,g_param.fram_id_);
            g_param.g_flag_ = 0;
        }
        if (g_param.g_flag_ == 1 && keyval == '2'){
            LabelInfo cur_label;
            cur_label.leftPt = g_param.g_pt1_;
            cur_label.rightPt = g_param.g_pt2_;
            cur_label.label = "ped";
            g_param.label_infos_[g_param.cur_file_name_].push_back(cur_label);
            draw_img(in_filelists,g_param.fram_id_);
            g_param.g_flag_ = 0;
        }
        //Pre
        if (keyval == 'p'){
            if (g_param.fram_id_ > 0){
                g_param.fram_id_--;
                draw_img(in_filelists,g_param.fram_id_);
            }
        }

        //Delete
        if (keyval == 'd')
        {
            int a = g_param.label_infos_.erase(g_param.cur_file_name_);
            draw_img(in_filelists,g_param.fram_id_);
        }

        if (keyval == 's'){
            save_label_text();
        }
    }
    save_label_text();
}

void save_label_text(){
    std::string label_file = g_param.dir_path_ + "/label.txt";

    std::ofstream outfile;
    outfile.open(label_file.c_str());
    assert(outfile.is_open());

    std::map<int,std::vector<LabelInfo> >::iterator l_itr = g_param.label_infos_.begin();
    for (;l_itr != g_param.label_infos_.end(); ++l_itr) {
        int tmp_frame_id = (*l_itr).first;
        std::vector<LabelInfo> cur_label_infos = (*l_itr).second;
        outfile << num2str(tmp_frame_id)<<" ";
        for (int j = 0; j < cur_label_infos.size(); ++j) {
            outfile<<cur_label_infos[j].label<<" ";
            outfile<<cur_label_infos[j].leftPt.x<<" ";
            outfile<<cur_label_infos[j].leftPt.y<<" ";
            outfile<<cur_label_infos[j].rightPt.x<<" ";
            outfile<<cur_label_infos[j].rightPt.y<<" ";
        }
        outfile<<std::endl;
    }
    outfile.close();
}

void draw_img(const std::vector<std::string>& in_filelists, const int& frame_id){
    std::string tmp_img_file = g_param.dir_path_ + "/" + in_filelists[frame_id];
    g_param.g_img_ = cv::imread(tmp_img_file);
    g_param.g_img_.copyTo(g_param.g_disp_);

    std::string filename = get_filename(in_filelists[g_param.fram_id_]);
    std::map<int ,std::vector<LabelInfo> >::iterator l_it;
    g_param.cur_file_name_ = atoi(filename.c_str());
    l_it=g_param.label_infos_.find(g_param.cur_file_name_);
    std::vector<LabelInfo> tmp_label_info = (*l_it).second;
    for (int i = 0; i < tmp_label_info.size(); ++i) {
        drawRect(g_param.g_disp_, tmp_label_info[i].leftPt, tmp_label_info[i].rightPt, cv::Scalar(0, 0, 255), 1);
        draw_label(g_param.g_disp_,tmp_label_info[i].label,tmp_label_info[i].leftPt.x,tmp_label_info[i].leftPt.y,cv::Scalar(0,0,255));
    }

    cv::imshow("label_tool", g_param.g_disp_);
}

std::string get_filename(const std::string& in_file){
    std::string tmp_filename = in_file.substr(0,in_file.size() - g_param.type_.size());
    return tmp_filename;
}

void draw_label(cv::Mat& disp,std::string str,int x,int y,cv::Scalar color){
    cv::putText(disp, str.c_str(),cv::Point(x,y),cv::FONT_HERSHEY_SIMPLEX,0.5,color,1);
}

void mouse_action(int event, int x, int y, int flags, void* param){
    static cv::Point pt1 = cv::Point(-1, -1);
    static cv::Point pt2 = cv::Point(-1, -1);

    cv::Scalar line_clr = cv::Scalar(255, 0, 0);
    cv::Scalar font_clr = cv::Scalar(0, 255, 0);
    cv::Scalar rect_clr = cv::Scalar(0, 0, 255);

    g_param.g_disp_.copyTo(g_param.g_disp_mouse_move_);

    if (g_param.g_flag_ == 0 && event == CV_EVENT_MOUSEMOVE && !(flags & CV_EVENT_FLAG_LBUTTON))
    {
        drawCrossLine(g_param.g_disp_mouse_move_, x, y, line_clr);
        drawCoor(g_param.g_disp_mouse_move_, x, y, font_clr);

        cv::imshow("label_tool", g_param.g_disp_mouse_move_);
    }
    else if (event == CV_EVENT_LBUTTONDOWN)
    {
        pt1 = cv::Point(x, y);
        drawCrossLine(g_param.g_disp_mouse_move_, x, y, line_clr);
        drawCoor(g_param.g_disp_mouse_move_, x, y, font_clr);
    }
    else if (event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON))
    {
        pt2 = cv::Point(x - 1, y - 1);
        drawCrossLine(g_param.g_disp_mouse_move_, x, y, line_clr);
        drawCoor(g_param.g_disp_mouse_move_, x, y, font_clr);
        drawRect(g_param.g_disp_mouse_move_, pt1, pt2, rect_clr, 1);
        cv::imshow("label_tool", g_param.g_disp_mouse_move_);
    }
    else if (event == CV_EVENT_LBUTTONUP)
    {
        g_param.g_flag_ = 1;
        g_param.g_pt1_ = pt1;
        g_param.g_pt2_ = pt2;
    }
}

void drawRect(cv::Mat& disp, cv::Point pt1, cv::Point pt2, cv::Scalar color, int thick){
    cv::rectangle(disp, pt1, pt2, color, thick);
}

void drawCoor(cv::Mat& disp, int x, int y, cv::Scalar color){
    std::string str;
    x -= 90;
    y -= 10;
    str = "(" + num2str(x) + "," + num2str(y) + ")";
    cv::putText(disp, str, cv::Point(x, y), cv::FONT_HERSHEY_SIMPLEX, 0.5, color, 1);
}

void drawCrossLine(cv::Mat& disp, int x, int y, cv::Scalar color){
    int width, height;
    width = disp.cols;
    height = disp.rows;

    x--;
    y--;

    cv::line(disp, cv::Point(0, y), cv::Point(width, y), color, 1);
    cv::line(disp, cv::Point(x, 0), cv::Point(x, height), color, 1);
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

void read_label_text(const std::string& dir_path){
    std::string label_file = dir_path + "/label.txt";

    std::ifstream ifile;  //需要包含头文件<fstream>
    ifile.open(label_file.c_str());
    assert(ifile.is_open());

    std::string line;
    while (getline(ifile,line)){
        std::istringstream infile(line);
        std::string tmp_str;
        infile >> tmp_str;
        int tmp_framid = atoi(tmp_str.c_str());
        LabelInfo tmp_label_info;
        while (!infile.eof()){
            infile >> tmp_str;
            tmp_label_info.label = tmp_str;
            infile >> tmp_str;
            tmp_label_info.leftPt.x = atof(tmp_str.c_str());
            infile >> tmp_str;
            tmp_label_info.leftPt.y = atof(tmp_str.c_str());
            infile >> tmp_str;
            tmp_label_info.rightPt.x = atof(tmp_str.c_str());
            infile >> tmp_str;
            tmp_label_info.rightPt.y = atof(tmp_str.c_str());
            g_param.label_infos_[tmp_framid].push_back(tmp_label_info);
        }
    }
    ifile.close();
}
