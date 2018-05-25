# Aircraft-door-detection-based-on-contour-fusion
A Fast Algorithm of Aircraft Door Detection Based on Image Processing

### Introduction
This is a rectangular detection algorithm for aircraft doors. Based on the shape characteristics of the aircraft doors, the digital image processing knowledge such as contour detection and morphology processing is used to detect the aircraft door in the image.

Author: [Qichao Wu](https://github.com/deepthinking-qichao)
<br>Email: 467736413@qq.com or 13660479414@163.com

### 第一步
将输入的[原图]()灰度化，然后对灰度图提取[canny边缘]()，其中canny的阈值可通过滑动条进行选择.

### 第二步
对canny边缘图施加闭操作，闭操作可以排除小型黑色区域，初步连接断开的边缘，按照边缘断裂的具体情况，选择合适大小的核，进行膨胀运算，进一步连接断开边缘，得到形态学处理后的[结果图]().

### 第三步
对形态学处理后的canny边缘图进行轮廓检测，并用树状轮廓进行存储，遍历每一层轮廓（总共四层），对于每一个轮廓，判断其轮廓包含点数、占空比和轮廓面积是否满足一定的条件，若不符合条件，则从轮廓容器中剔除，从而进行矩形轮廓的初步筛选，并对筛选后的轮廓计算[最小外接矩形]().
***注意：计算长宽比时，若矩形角度是否大于45度，则长宽比调整为原来的倒数

### 第四步
对初步筛选后的轮廓的最小外接矩形往较长边延伸（这符合舱门的长方形特性），进行内部填充（即矩形的内部变为白色，矩形外部区域为黑色），对相邻嵌套的矩形实现[融合]().

### 第五步
对融合后的矩形图进行腐蚀处理，分离粘连在一起的矩形区域，然后计算融合后的矩形图的[轮廓]().

### 第六步
对融合后的矩形图的轮廓，计算轮廓的[最小外接矩形]()，然后根据占空比、长宽比和面积等条件进行筛选，最后得到筛选后的融合后的矩形图的轮廓的[最小外接矩形]().

原始的飞机舱门图片在文件夹[/Original Image]()中，飞机舱门检测结果图在文件夹[/Processed Image]()中，中间结果在文件夹[/Intermediate Results]中.

# ***Hope this help you***