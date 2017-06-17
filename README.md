# 点云label工具说明

1 需要建立文件夹data,data下建立文件夹img和pcd，img文件夹下建立label.txt文件

结构如下：
```
data
    img
        label.txt
    pcd
```

2 在节点中，修改文件夹路径


- nodes/main.cpp中修改一处
```
std::string dir_path = "/home/mjj/roboTool/src/cv_label/data/img";//到img
```
- nodes/listen_rosbag_to_pcd.cpp中修改一处
```
std::string dir_path = "/home/mjj/roboTool/src/cv_label/data";//到dat
```

3 运行listen_rosbag_to_pcd节点，将订阅的velodyne_points转化为pcd，并将pcd转化为png
```
    rosrun cv_label listen_rosbag_to_pcd
    //开启listen_rosbag_to_pcd节点后，再运行bag文件
```

4 运行cv_main节点，进行打标

- 点云鼠标左键，移动鼠标画框来对目标进行打标
- 松开鼠标左键后，需要对目标类别进行打标。按1表示car，按2表示ped
- 重复第1步和第2步，对一帧图像中的多个目标进行打标
- 按“n”进行下一帧的打标
- 按“s”保存当前打标好的数据
- 按“p”到上一帧
- 按“d”删除当前帧打标结果
