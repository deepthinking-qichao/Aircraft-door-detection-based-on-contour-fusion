# Aircraft-door-detection-based-on-contour-fusion
A Fast Algorithm of Aircraft Door Detection Based on Image Processing

### Introduction
This is a rectangular detection algorithm for aircraft doors. Based on the shape characteristics of the aircraft doors, the digital image processing knowledge such as contour detection and morphology processing is used to detect the aircraft door in the image.

Author: [Qichao Wu](https://github.com/deepthinking-qichao)
<br>Email: 467736413@qq.com or 13660479414@163.com

### 运行环境
VS2013+OpenCV1.0
<br>项目只包含一个cpp文件<[main.cpp]()>，输入输出参数如下：
```
string s_input = "./Original_Imagee";               //输入原始图片
string s_output = "./Processed_Image";              //输出飞机舱门检测结果
```
检测算法中间结果在程序的运行过程中会以窗口的形式进行显示

### 第一步
将输入的[原图](https://github.com/deepthinking-qichao/Aircraft-door-detection-based-on-contour-fusion/blob/master/Intermediate%20Results/1.png)灰度化，然后对灰度图提取[canny边缘](https://github.com/deepthinking-qichao/Aircraft-door-detection-based-on-contour-fusion/blob/master/Intermediate%20Results/2.png)，其中canny的阈值可通过滑动条进行选择.

### 第二步
对canny边缘图施加闭操作，闭操作可以排除小型黑色区域，初步连接断开的边缘，按照边缘断裂的具体情况，选择合适大小的核，进行膨胀运算，进一步连接断开边缘，得到形态学处理后的[结果图](https://github.com/deepthinking-qichao/Aircraft-door-detection-based-on-contour-fusion/blob/master/Intermediate%20Results/3.png).

### 第三步
对形态学处理后的canny边缘图进行轮廓检测，并用树状轮廓进行存储，遍历每一层轮廓（总共四层），对于每一个轮廓，判断其轮廓包含点数、占空比和轮廓面积是否满足一定的条件，若不符合条件，则从轮廓容器中剔除，从而进行矩形轮廓的初步筛选，并对筛选后的轮廓计算[最小外接矩形](https://github.com/deepthinking-qichao/Aircraft-door-detection-based-on-contour-fusion/blob/master/Intermediate%20Results/4.png).
***注意：计算长宽比时，若矩形角度是否大于45度，则长宽比调整为原来的倒数

### 第四步
对初步筛选后的轮廓的最小外接矩形往较长边延伸，进行内部填充（即矩形的内部变为白色，矩形外部区域为黑色），对相邻嵌套的矩形实现[融合](https://github.com/deepthinking-qichao/Aircraft-door-detection-based-on-contour-fusion/blob/master/Intermediate%20Results/5.png).
***注意：往较长边延伸是为了弥补第二步对边缘膨胀运算后内层轮廓较原来收缩，同时消除舱门前缆绳的干扰***

### 第五步
对融合后的矩形图进行腐蚀处理，分离粘连在一起的矩形区域，然后计算融合后的矩形图的[轮廓](https://github.com/deepthinking-qichao/Aircraft-door-detection-based-on-contour-fusion/blob/master/Intermediate%20Results/6.png).

### 第六步
对融合后的矩形图的轮廓，计算轮廓的[最小外接矩形](https://github.com/deepthinking-qichao/Aircraft-door-detection-based-on-contour-fusion/blob/master/Intermediate%20Results/7.png)，然后根据占空比、长宽比和面积等条件进行筛选，最后得到筛选后的融合后的矩形图的轮廓的[最小外接矩形](https://github.com/deepthinking-qichao/Aircraft-door-detection-based-on-contour-fusion/blob/master/Intermediate%20Results/8.png).

经过第三步，在原图上检测到的所有轮廓的最小外接矩形
<br>![image](https://github.com/deepthinking-qichao/Aircraft-door-detection-based-on-contour-fusion/blob/master/Intermediate%20Results/10.png)
最终检测到的飞机舱门
<br>![image](https://github.com/deepthinking-qichao/Aircraft-door-detection-based-on-contour-fusion/blob/master/Intermediate%20Results/9.png)

原始的飞机舱门图片在文件夹[/Original_Image](https://github.com/deepthinking-qichao/Aircraft-door-detection-based-on-contour-fusion/tree/master/Original%20Image)中，飞机舱门检测结果图在文件夹[/Processed_Image](https://github.com/deepthinking-qichao/Aircraft-door-detection-based-on-contour-fusion/tree/master/Processed%20Image)中，中间结果在文件夹[/Intermediate_Results](https://github.com/deepthinking-qichao/Aircraft-door-detection-based-on-contour-fusion/tree/master/Intermediate%20Results)中.

# ***Hope this help you***