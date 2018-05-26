/************************************************************
Filename: Rectangle_Detection
Author: Wu Qichao
Version; 1_6_4
Date: 2017-05-03
Description:1)以更大的核对canny边缘进行膨胀操作（kernel_coefficient3），然后在最后的矩形融合图填充轮廓的时候，把矩形往较大边伸长（expand_edge_coefficient）
Funtion List:
History:
1、Date:2017-04-07
Author:Wu Qichao
Version:1_0_0
Modification:参考网上的程序http://blog.csdn.net/mr_vision/article/details/4242904，基本上没做修改
2、Date:2017-04-11
Author:Wu Qichao
Version:1_1_0
Modification:从原来的只检测第2层轮廓(从第0层开始)，修改为对每一层轮廓都检测；
从原来的用长宽比变为利用占空比来进行矩形检测
3、Date:2017-04-12
Author:Wu Qichao
Version:1_2_0
Modification:1)修改了duty_cycle阈值
2)将对当前轮廓是否有v->next做出了位置调整，移到了循环体尾部
3)在ChipInfo结构中增加了最小外界矩形min_rectangle_area成员对象，并修改了duty_cycle的计算方法（原来的计算方法有误）
4、Date: 2017-04-12
Author: Wu Qichao
Version; 1_2_1
Modification:在1_2_0的基础上添加了注释
5、Date: 2017-04-23
Author: Wu Qichao
Version; 1_4_0
Modification:1)用canny算法提取轮廓
2)加入了滑动条，方便调整canny算法的阈值
3)在第0层轮廓中修改了画矩形的方法，采用cvRectangle函数进行画图，发现原来第0层轮廓和第1层轮廓覆盖，导致第0层轮廓没有显示
4)增加了利用开闭操作等形态学处理对边缘进行连接
6、Date: 2017-04-24
Author: Wu Qichao
Version; 1_5_0
Modification:1)增加预处理，也即是根据舱门边缘是黑色或灰色的特征，结合颜色信息进行处理，具体方法是用cvCopy掩膜处理
2)修改1效果不好，弃用，退回到1_4_0
6、Date: 2017-04-25
Author: Wu Qichao
Version; 1_6_0
Modification:1)修改了程序的一个小错误，也就是闭操作没有将核的大小参数NULL替换回自动以的element变量
2)采用矩形融合的方法将断开的轮廓融合在一起,具体做法是填充各个小矩形，然后练成一个大的连通域，以达到连通的目的。然后找出该连通域的最大外接矩形，就能做到矩形融合。
3)发现第3层轮廓的一些错误，并进行修改
4)矩形的width和height不统一，经过测试知道是外接矩形中angle的原因
6、Date: 2017-04-28
Author: Wu Qichao
Version; 1_6_1
Modification:1）搞不清楚为什么cvDrawContours不能较好地填充轮廓内部区域，因此放弃采用该方法（该方法有一定的意义，可继续进行尝试），从1_7_0退回到1_6_0；
2）在之前对canny提取边缘图进行闭操作的基础上，添加了膨胀操作，便于加粗边缘
Others:个人设想，因为在canny提取边缘图像的基础上添加了膨胀操作，对边缘图像进行轮廓检测会检测出一对内外轮廓。而由于外轮廓一般与其他轮廓粘连在一起，所以最终检测到的矩形轮廓都是内轮廓，
从而导致了一个问题，就是一个整体的区域，进行矩形内部填充之后，会分成好多个小区域。此时，可考虑填充轮廓的时候，把矩形的周围大一圈的区域也填充，具体数值为矩形外部kernel_coefficient3个长度大小的区域。
6、Date: 2017-05-02
Author: Wu Qichao
Version; 1_6_2
Modification:1）对2017/5/2日的数据进行测试，文件名称为snap1
2）为了读取测试方便，将单张图片读取方式优化为批量读取方式
3）针对融合时矩形连成一遍，可以考虑把面积小的矩形滤掉，防止因为这些矩形使不相关的区域连接在一起,通过修改contours_tatal_threshold可实现该目的
4）针对某些矩形融合图中，相邻的矩形在某些点处相连，导致成为一个更大的连通域（该连通域不再是矩形形状，而是两个矩形结合在一起的不规则形状）
而导致的检测失败，可通过修改kernel_coefficient2参数，使腐蚀的程度更强烈，切断两个矩形相连的部分。
7、Date: 2017-05-03
Author: Wu Qichao
Version; 1_6_3
Modification:1)针对只检测到舱门的一部份而没有检测出整体做出算法的改进，可弃掉canny边缘图的膨胀操作，然后
填充轮廓的时候，把矩形往较大边伸长（具体数值为矩形外部kernel_coefficient3个长度大小），使飞机舱门割裂的矩形重新连成一片。
2)针对光照不均匀导致的canny边缘提取有缺口的问题，可考虑通过图片预处理进行光照校正；
3)对于图片本身有缺陷的问题，如拍摄时舱门下部区域被一条黑绳子遮挡的情况，需要拍摄时注意镜头前无干扰物；
Others:1)20图检测不到舱门最下边的区域，问题是因为原图有缺陷，导致检测到的canny边缘中最低侧有缺口，进而检测不到最低侧的轮廓；
2)14、21、23、32、71图检测不到，问题是检测到的矩形侧边部分重合，导致互相包含，进而融合后得到一个不规则的多边形；
3)24、66图检测不到舱门上部区域，问题是canny边缘提取到的轮廓有缺口；
4)35图检测不到舱门次下边的区域，问题是canny提取到的该区域边缘有缺口；
5)54、58、59图检测不到舱门次下边的区域，问题是拍摄时有一条黑线挡住了；
6)61、63、64、67、68、69、73图检测不到舱门下部区域，因为光照问题导致canny提取不到下部区域边缘
************************************************************/

#include "cv.h"
#include "highgui.h"
#include <vector>
#include <windows.h>												//MessageBoxA函数头文件
#include <string>
#include <iostream>
#define duty_cycle_threshold 0.7									//检测轮廓时的占空比(轮廓面积和最小矩形面积之比)的阈值，0.7最佳
#define duty_cycle_threshold_fuse 0.5								//填充矩形的占空比阈值
#define contours_tatal_threshold 30									//轮廓中的元素（点）总数
#define ostu_coefficient 1.0										//ostu法求得的阈值的比例系数
#define kernel_coefficient 1										//形态学核的参数，用于闭操作
#define kernel_coefficient2 2										//形态学核的参数,用于后处理腐蚀
#define kernel_coefficient3 2										//形态学核的参数，用于对canny边缘图进行膨胀，之前的参数是1
#define expand_edge_coefficient 6									//用于扩大符合一定条件的矩形的边缘，目的使舱门的各部分连接在一起
#define aspect_ratio_min_expand 1									//飞机舱门的长宽比最小值
#define aspect_ratio_max_expand 2									//飞机舱门的长宽比最大值
#define aspect_ratio_min 1.2										//飞机舱门的长宽比最小值
#define aspect_ratio_max 3											//飞机舱门的长宽比最大值
#define contour_area_max 1000										//飞机舱门面积阈值
#define area_max 3000												//飞机舱门面积阈值
int nThresholdEdge = 100;											//canny算法阈值
int element_shape = CV_SHAPE_RECT;									//核的形状为矩形
int angle_correcting;												//矩形校正后的角度
int selective_picture = 66;											//用于单独测试一张图片
using namespace std;
int otsu(const IplImage* image, int x0, int y0, int dx, int dy);	//ostu法求阈值函数

/*创建全局变量*/
const char *pstrWindowsCannyTitle = "边缘检测图";			//创建滑动条所在窗口（即canny边缘检测框）的名字
IplImage* gray;												//原图的灰度图
IplImage* canny_img;											//canny算法结果图，二值化图

/*利用canny算法创建二值图像canny_img，并输出*/
void on_trackbar(int threshold)
{
	//canny边缘检测  
	cvCanny(gray, canny_img, threshold, threshold * 3, 3);
	cvShowImage(pstrWindowsCannyTitle, canny_img);
}

struct ChipInfo
{
	CvBox2D box;													//OpenCV的2D盒子结构，包括矩形中心横纵坐标、长宽和偏移角度五个参数
	double area;													//轮廓包围面积
	float min_rectangle_area;										//最小外接矩形面积
	float duty_cycle;												//占空比（轮廓面积和最小外接矩形之比）	
	float aspect_ratio;												//长宽比
};
int main()
{
	/*创建canny算法的显示窗口及调节阈值的滑动条*/
	const char *pstrWindowsToolBar = "Threshold";				//创建滑动条的名字
	cvNamedWindow(pstrWindowsCannyTitle, CV_WINDOW_AUTOSIZE);	//创建滑动窗口									//创建滑动条初始位置
	cvCreateTrackbar(pstrWindowsToolBar, pstrWindowsCannyTitle, &nThresholdEdge, 255, on_trackbar);			//创建滑动条
	//on_trackbar(1);												//初始响应值为nThresholdEdge

	/*创建一系列的显示窗口*/
	cvNamedWindow("Src", 1);										//CV_DEFAULT=0，窗口的大小不会因图像的大小而改变；CV_WINDOW_AUTOSIZE=1，窗口则会根据图像的实际大小自动进行拉伸或伸缩，以容纳图像
	cvNamedWindow("Contours_Detect", 1);
	cvNamedWindow("rectangle_detect_fuse", 1);
	cvNamedWindow("Rectangle_Detect_Fuse_Rect", 1);
	cvNamedWindow("Contours_Detect_Fuse", 1);
	cvNamedWindow("Rectangle_Detect_Final_Result", 1);
	cvNamedWindow("Dst_Detection", 1);
	cvNamedWindow("rectangle_detect", 1);
	cvNamedWindow("dst", 1);

	/*输入源图像，并显示在Src窗口中*/
	string s_input = "F:\\图片\\飞机舱门图片\\snap1\\snap1\\snap00";
	string s1;
	string s_output = "F:\\图片\\飞机舱门图片\\1_6_4_v1\\snap00";

	/*生成输入图片和输出图片的绝对路径*/
	const char *picstr_input;												//输入图片的绝对路径
	const char *picstr_output;												//输出图片的绝对路径						
	for (int picnum = 8; picnum <= 73; picnum++)
	{
		/*用于调试的时候特定检查某一张图片的检测情况*/
		//if (picnum != selective_picture)
			//continue;

		if (picnum < 10)
		{
			s_input = s_input + '0';											//如果图片序列号小于10，则需要在序列号前面补充一个0对齐
			s_output = s_output + '0';								//如果图片序列号小于10，则需要在序列号前面补充一个0对齐
		}
		s1 = to_string(picnum);										//不需要包含任何头文件，应该是在utility中，但无需包含，直接使用，还定义任何其他内置类型转为string的重载函数
		s_input = s_input + s1 + ".jpg";										//生成图片的绝对路径
		s_output = s_output + s1 + ".jpg";										//生成图片的绝对路径
		picstr_input = s_input.c_str();								//将string对象转化为C风格字符串
		picstr_output = s_output.c_str();								//将string对象转化为C风格字符串
		IplImage* src = cvLoadImage(picstr_input, -1);					//#define CV_LOAD_IMAGE_UNCHANGED  -1 也就是保持图片的原状读入
		src->origin = IPL_ORIGIN_TL;								//将图片的原点坐标位置人工定位
		cvShowImage("Src", src);										//显示图像

		/*创建dst,rectangle_detect,gray*/
		IplImage* dst = cvCloneImage(src);											//dst为最后的矩形检测结果图
		IplImage* dst_detection = cvCloneImage(src);											//dst为最后的矩形检测结果图
		IplImage* rectangle_detect = cvCreateImage(cvGetSize(src), 8, 3);			//rectangle_detect为筛选出的拟合矩形图
		cvZero(rectangle_detect);
		IplImage* rectangle_detect_fuse_rect = cvCreateImage(cvGetSize(src), 8, 3);			//rectangle_detect为筛选出的拟合矩形图
		cvZero(rectangle_detect_fuse_rect);
		IplImage* rectangle_detect_final = cvCreateImage(cvGetSize(src), 8, 3);			//rectangle_detect为最终得到的飞机舱门拟合矩形图
		cvZero(rectangle_detect_final);
		IplImage* rectangle_detect_fuse = cvCreateImage(cvGetSize(src), 8, 1);			//rectangle_detect_fuse为填充后的拟合二值图
		cvZero(rectangle_detect_fuse);
		IplImage* contours_detect = cvCreateImage(cvGetSize(src), 8, 3);			//contours_detect为检测到的所有轮廓图
		IplImage* contours_detect_fuse = cvCreateImage(cvGetSize(src), 8, 3);			//contours_detect_fuse为填充后的矩形的外接矩形图
		gray = cvCreateImage(cvGetSize(src), 8, 1);						//depth=8 channels=1
		cvCvtColor(src, gray, CV_RGB2GRAY);											//将源图像src转换成灰度图gray
		canny_img = cvCreateImage(cvGetSize(src), 8, 1);						//depth=8 channels=1
		IplImage * temp = cvCreateImage(cvGetSize(src), 8, 1);					//创建开闭操作是的临时图像，用于本地in-place运算
		IplConvKernel* element = 0;												//创建形态学处理的核
		IplConvKernel* element2 = 0;											//创建形态学处理的核
		IplConvKernel* element3 = 0;											//创建形态学处理的核

		/*利用canny算法创建二值图像canny_img，并输出*/
		cvCanny(gray, canny_img, nThresholdEdge, nThresholdEdge * 3, 3);
		element = cvCreateStructuringElementEx(kernel_coefficient * 2 + 1, kernel_coefficient * 2 + 1, kernel_coefficient, kernel_coefficient, element_shape, 0);
		cvMorphologyEx(canny_img, canny_img, temp, element, CV_MOP_CLOSE, 1);					//default 3*3，进行闭操作运算，闭运算能够排除小型黑洞（黑色区域）
		element3 = cvCreateStructuringElementEx(kernel_coefficient3 * 2 + 1, kernel_coefficient3 * 2 + 1, kernel_coefficient3, kernel_coefficient3, element_shape, 0);
		cvDilate(canny_img, canny_img, element3, 1);
		cvShowImage(pstrWindowsCannyTitle, canny_img);

		///*创建二值图像gray_clone，并输出*/
		//iplimage* gray_clone = cvcloneimage(gray);																//创建灰度图像gray的副本gray_clone
		//int threshold = otsu(gray_clone, 0, 0, gray_clone->width, gray_clone->height) * ostu_coefficient;		//将ostu法的ostu_coefficient倍作为二值化图像的阈值
		//cvthreshold(gray_clone, gray_clone, threshold, 255, cv_thresh_binary);									//gray_clone为ostu法阈值分割后的二值图像
		//cvnamedwindow("gray_threshold_image", 1);
		//cvnamedwindow("contours_detect", 1);
		//cvshowimage("gray_threshold_image", gray_clone);
		//cvsaveimage("gray_threshold.jpg", gray_clone);

		///*腐蚀并膨胀图像*/
		//cvErode(gray_clone, gray_clone);
		//cvDilate(gray_clone, gray_clone);

		/*找到图像中的所有轮廓*/
		CvMemStorage* storage = cvCreateMemStorage(0);

		CvSeq* contours;
		cvFindContours(canny_img, storage, &contours, sizeof(CvContour), CV_RETR_TREE,
			CV_CHAIN_APPROX_NONE, cvPoint(0, 0));						//找二值图像中的轮廓，CV_RETR_TREE - 提取所有轮廓，并且重构嵌套轮廓的全部 hierarchy，CV_CHAIN_APPROX_NONE - 将所有点由链码形式翻译(转化）为点序列形式 

		/*在Contours_Detect窗口中画出检测到的所有轮廓*/
		if (contours)
			cvDrawContours(contours_detect, contours, cvScalarAll(255), cvScalarAll(255), 100);				//在contours_detect中画出所有的轮廓
		cvShowImage("Contours_Detect", contours_detect);
		cvSaveImage("Contours_Detect.jpg", contours_detect);

		/*在每一层的轮廓中筛选出符合条件的候选轮廓，并将候选轮廓以ChipInfo结构，压入到容器中*/
		std::vector <ChipInfo> candiate_rect_vec0;						//创建ChipInfo容器，对象为ChipInfo结构
		std::vector <ChipInfo> candiate_rect_vec1;						//创建ChipInfo容器，对象为ChipInfo结构
		std::vector <ChipInfo> candiate_rect_vec2;						//创建ChipInfo容器，对象为ChipInfo结构
		std::vector <ChipInfo> candiate_rect_vec3;						//创建ChipInfo容器，对象为ChipInfo结构
		std::vector <ChipInfo> candiate_rect_vec_fuse;					//创建ChipInfo容器，对象为ChipInfo结构
		ChipInfo chip;

		/*进入到最外层轮廓（第0层），判断其是否包含别的轮廓*/
		for (; contours; contours = contours->h_next)
		{
			/*判断该轮廓树节点的点数是否大于某个阈值*/
			if (contours->total < contours_tatal_threshold)
			{
				continue;
			}

			/*筛选出占空比大于某个阈值的轮廓*/
			CvBox2D box_outer = cvMinAreaRect2(contours);			//函数cvMinAreaRect2可以返回一个包围轮廓最小的长方形，且这个长方形允许是倾斜的
			double area = fabs(cvContourArea(contours));			//函数cvContourArea计算整个或部分轮廓的面积。在计算部分轮廓的情况时，由轮廓弧线和连接两端点的弦围成的区域总面积被计算
			float min_rectangle_area = box_outer.size.width * box_outer.size.height;	//计算轮廓最小外接矩形的面积
			float duty_cycle = area / min_rectangle_area;			//定义占空比，为轮廓面积除以最小外界矩形面积
			double angle_correcting_0 = fabs(box_outer.angle);
			double angle_correcting_90 = fabs(box_outer.angle - 90);							//校正矩形旋转角度，统一矩形的长宽对应的实际长度
			float aspect_ratio;
			bool angle_flag = angle_correcting_0 < angle_correcting_90;
			if (angle_flag)
				aspect_ratio = box_outer.size.width / box_outer.size.height;	//计算轮廓最小外接矩形的长宽比
			else
				aspect_ratio = box_outer.size.height / box_outer.size.width;	//计算轮廓最小外接矩形的长宽比
			if ((duty_cycle > duty_cycle_threshold) && (area > contour_area_max))
			{
				/*对一个ChipInfo结构进行赋值*/
				printf("test 第0层的angle为%f,  width为%f,  height为%f,  aspect_ratio为%f, angle_flag为%d\n", box_outer.angle, box_outer.size.width, box_outer.size.height, aspect_ratio, angle_flag);
				ChipInfo chip;
				if (angle_flag == 0)
					box_outer.size.width += expand_edge_coefficient;
				else
					box_outer.size.height += expand_edge_coefficient;
				chip.box = box_outer;					//chip对象的box成员为轮廓的最小外接矩形
				chip.area = area;						//chip对象的area成员为轮廓的面积
				chip.min_rectangle_area = min_rectangle_area;		//chip对象的min_rectangle_area成员为轮廓最小外接矩形面积;						//chip对象的area成员为轮廓的面积
				chip.duty_cycle = duty_cycle;			//chip对象的duty_cycle成员为轮廓与最小外接矩形的占空比
				candiate_rect_vec0.push_back(chip);		//将每个生成的ChipInfo对象压入ChipInfo容器之中
			}

			/*再判断其是否包含下一层的轮廓，若不包含，就进而检测同一层的其他轮廓*/
			if (!contours->v_next)
			{
				continue;
			}

			/*进入到下一层轮廓（第一层）*/
			CvSeq* contour_first = contours->v_next;
			for (; contour_first; contour_first = contour_first->h_next)
			{
				/*判断该轮廓树节点的点数是否大于某个阈值；再判断其是否包含别的轮廓*/
				if (contour_first->total < contours_tatal_threshold)
				{
					continue;
				}

				/*筛选出占空比大于某个阈值的轮廓*/
				CvBox2D box_outer = cvMinAreaRect2(contour_first);		//函数cvMinAreaRect2可以返回一个包围轮廓最小的长方形，且这个长方形允许是倾斜的
				double area = fabs(cvContourArea(contour_first));		//函数cvContourArea计算整个或部分轮廓的面积。在计算部分轮廓的情况时，由轮廓弧线和连接两端点的弦围成的区域总面积被计算
				float min_rectangle_area = box_outer.size.width * box_outer.size.height;	//计算轮廓最小外接矩形的面积
				float duty_cycle = area / min_rectangle_area;			//定义占空比，为轮廓面积除以最小外界矩形面积
				double angle_correcting_0 = fabs(box_outer.angle);
				double angle_correcting_90 = fabs(box_outer.angle - 90);							//校正矩形旋转角度，统一矩形的长宽对应的实际长度
				float aspect_ratio;
				bool angle_flag = angle_correcting_0 < angle_correcting_90;
				if (angle_flag)
					aspect_ratio = box_outer.size.width / box_outer.size.height;	//计算轮廓最小外接矩形的长宽比
				else
					aspect_ratio = box_outer.size.height / box_outer.size.width;	//计算轮廓最小外接矩形的长宽比
				if ((duty_cycle > duty_cycle_threshold) && (area > contour_area_max))
				{
					/*对一个ChipInfo结构进行赋值*/
					printf("test 第1层的angle为%f,  width为%f,  height为%f,  aspect_ratio为%f, angle_flag为%d\n", box_outer.angle, box_outer.size.width, box_outer.size.height, aspect_ratio, angle_flag);
					ChipInfo chip;
					if (angle_flag)
						box_outer.size.width += expand_edge_coefficient;
					else
						box_outer.size.height += expand_edge_coefficient;
					chip.box = box_outer;					//chip对象的box成员为轮廓的最小外接矩形
					chip.area = area;						//chip对象的area成员为轮廓的面积
					chip.min_rectangle_area = min_rectangle_area;		//chip对象的min_rectangle_area成员为轮廓最小外接矩形面积;						//chip对象的area成员为轮廓的面积
					chip.duty_cycle = duty_cycle;			//chip对象的duty_cycle成员为轮廓与最小外接矩形的占空比
					candiate_rect_vec1.push_back(chip);		//将每个生成的ChipInfo对象压入ChipInfo容器之中
				}

				/*再判断其是否包含下一层的轮廓，若不包含，就进而检测同一层的其他轮廓*/
				if (!(contour_first->v_next))
				{
					continue;
				}

				/*进入下一层轮廓（第二层）*/
				CvSeq* contour_second = contour_first->v_next;
				for (; contour_second; contour_second = contour_second->h_next)
				{
					/*判断该轮廓树节点的点数是否大于某个阈值；再判断其是否包含别的轮廓*/
					if (contour_second->total < contours_tatal_threshold)
					{
						continue;
					}

					/*筛选出占空比大于某个阈值的轮廓*/
					CvBox2D box_outer = cvMinAreaRect2(contour_second);		//函数cvMinAreaRect2可以返回一个包围轮廓最小的长方形，且这个长方形允许是倾斜的
					double area = fabs(cvContourArea(contour_second));		//函数cvContourArea计算整个或部分轮廓的面积。在计算部分轮廓的情况时，由轮廓弧线和连接两端点的弦围成的区域总面积被计算
					float min_rectangle_area = box_outer.size.width * box_outer.size.height;	//计算轮廓最小外接矩形的面积
					float duty_cycle = area / min_rectangle_area;			//定义占空比，为轮廓面积除以最小外界矩形面积
					double angle_correcting_0 = fabs(box_outer.angle);
					double angle_correcting_90 = fabs(box_outer.angle - 90);							//校正矩形旋转角度，统一矩形的长宽对应的实际长度
					float aspect_ratio;
					bool angle_flag = angle_correcting_0 < angle_correcting_90;
					if (angle_flag)
						aspect_ratio = box_outer.size.width / box_outer.size.height;	//计算轮廓最小外接矩形的长宽比
					else
						aspect_ratio = box_outer.size.height / box_outer.size.width;	//计算轮廓最小外接矩形的长宽比
					if (duty_cycle > duty_cycle_threshold)
					{
						/*对一个ChipInfo结构进行赋值*/
						ChipInfo chip;
						if (angle_flag == 0)
							box_outer.size.width += expand_edge_coefficient;
						else
							box_outer.size.height += expand_edge_coefficient;
						chip.box = box_outer;					//chip对象的box成员为轮廓的最小外接矩形
						chip.area = area;						//chip对象的area成员为轮廓的面积
						chip.min_rectangle_area = min_rectangle_area;		//chip对象的min_rectangle_area成员为轮廓最小外接矩形面积;						//chip对象的area成员为轮廓的面积
						chip.duty_cycle = duty_cycle;			//chip对象的duty_cycle成员为轮廓与最小外接矩形的占空比
						candiate_rect_vec2.push_back(chip);		//将每个生成的chip对象压入ChipInfo容器之中
					}

					/*再判断其是否包含下一层的轮廓，若不包含，就进而检测同一层的其他轮廓*/
					if (!(contour_second->v_next))
					{
						continue;
					}

					/*进入下一层轮廓（第三层）*/
					CvSeq* contour_third = contour_second->v_next;
					for (; contour_third; contour_third = contour_third->h_next)
					{
						/*判断该轮廓树节点的点数是否大于某个阈值；再判断其是否包含别的轮廓*/
						if (contour_third->total < contours_tatal_threshold)
						{
							continue;
						}

						/*筛选出占空比大于某个阈值的轮廓*/
						CvBox2D box_outer = cvMinAreaRect2(contour_third);		//函数cvMinAreaRect2可以返回一个包围轮廓最小的长方形，且这个长方形允许是倾斜的
						double area = fabs(cvContourArea(contour_third));		//函数cvContourArea计算整个或部分轮廓的面积。在计算部分轮廓的情况时，由轮廓弧线和连接两端点的弦围成的区域总面积被计算
						float min_rectangle_area = box_outer.size.width * box_outer.size.height;	//计算轮廓最小外接矩形的面积
						float duty_cycle = area / min_rectangle_area;			//定义占空比，为轮廓面积除以最小外界矩形面积
						double angle_correcting_0 = fabs(box_outer.angle);
						double angle_correcting_90 = fabs(box_outer.angle - 90);							//校正矩形旋转角度，统一矩形的长宽对应的实际长度
						float aspect_ratio;
						bool angle_flag = angle_correcting_0 < angle_correcting_90;
						if (angle_flag)
							aspect_ratio = box_outer.size.width / box_outer.size.height;	//计算轮廓最小外接矩形的长宽比
						else
							aspect_ratio = box_outer.size.height / box_outer.size.width;	//计算轮廓最小外接矩形的长宽比
						if (duty_cycle > duty_cycle_threshold)
						{
							/*对一个ChipInfo结构进行赋值*/
							ChipInfo chip;
							if (angle_flag == 0)
								box_outer.size.width += expand_edge_coefficient;
							else
								box_outer.size.height += expand_edge_coefficient;
							chip.box = box_outer;					//chip对象的box成员为轮廓的最小外接矩形
							chip.area = area;						//chip对象的area成员为轮廓的面积
							chip.min_rectangle_area = min_rectangle_area;		//chip对象的min_rectangle_area成员为轮廓最小外接矩形面积;						//chip对象的area成员为轮廓的面积
							chip.duty_cycle = duty_cycle;			//chip对象的duty_cycle成员为轮廓与最小外接矩形的占空比
							candiate_rect_vec3.push_back(chip);		//将每个生成的chip对象压入ChipInfo容器之中
						}
					}
				}
			}
		}

		/*得到了第0层一些候选的区域后，设定占空比/长宽比均最接近1的候选目标为目标*/
		float likely_max0 = -1;
		int index0 = -1;
		for (int t = candiate_rect_vec0.size() - 1; t >= 0; t--)
		{
			if (likely_max0 < candiate_rect_vec0[t].duty_cycle)
			{
				likely_max0 = candiate_rect_vec0[t].duty_cycle;
				index0 = t;														//容器下标从0开始
			}
		}

		/*如果第0层存在符合筛选条件的候选轮廓，挑选出占空比最大的轮廓进行画图，用红色表示*/
		if (index0 >= 0)
		{
			CvPoint2D32f pt[4];
			for (int t = candiate_rect_vec0.size() - 1; t >= 0; t--)
			{
				cvBoxPoints(candiate_rect_vec0[t].box, pt);						//找到矩形框顶点的坐标
				CvPoint pt0, pt1;
				pt0 = cvPoint(cvRound(pt[0].x), cvRound(pt[0].y));
				pt1 = cvPoint(cvRound(pt[2].x), cvRound(pt[2].y));
				cvRectangle(dst, pt0, pt1, cvScalar(0, 0, 255), 1, 4, 0);
				cvRectangle(rectangle_detect, pt0, pt1, cvScalar(0, 0, 255), 1, 4, 0);										//画筛选后符合条件的矩形
				cvRectangle(rectangle_detect_fuse, pt0, pt1, cvScalar(255), CV_FILLED, 4, 0);								//画填充后的矩形
				printf("0 level %d contours area is %lf.\t", t, candiate_rect_vec0[t].area);								//在屏幕上输出轮廓的面积
				printf("0 level %d min_rectangle_area is %f.\t", t, candiate_rect_vec0[t].min_rectangle_area);			    //在屏幕上输出轮廓最小外接矩形的面积
				printf("0 level %d duty_cycle is %f.\n", t, candiate_rect_vec0[t].duty_cycle);								//在屏幕上输出轮廓的占空比
			}

			cvLine(dst, cvPoint(cvRound(candiate_rect_vec0[index0].box.center.x - 5),
				cvRound(candiate_rect_vec0[index0].box.center.y)),
				cvPoint(cvRound(candiate_rect_vec0[index0].box.center.x + 5),
				cvRound(candiate_rect_vec0[index0].box.center.y)),
				CV_RGB(255, 0, 0), 1);
			cvLine(dst, cvPoint(cvRound(candiate_rect_vec0[index0].box.center.x),
				cvRound(candiate_rect_vec0[index0].box.center.y - 5)),
				cvPoint(cvRound(candiate_rect_vec0[index0].box.center.x),
				cvRound(candiate_rect_vec0[index0].box.center.y + 5)),
				CV_RGB(255, 0, 0), 1);
			char a[100];
			sprintf(a, "chip0 pos:(%d , %d),angle: %d", cvRound(candiate_rect_vec0[index0].box.center.x),
				cvRound(candiate_rect_vec0[index0].box.center.y),
				cvRound(candiate_rect_vec0[index0].box.angle));
			//MessageBoxA(NULL, a, "chip0 information", MB_OK);
		}
		else
		{
			char a[100];
			sprintf(a, "No Chip0");
			//MessageBoxA(NULL, a, "chip information", MB_OK);
		}

		/*得到了第1层一些候选的区域后，设定占空比/长宽比均最接近1的候选目标为目标，用绿色表示*/
		float likely_max1 = -1;
		int index1 = -1;
		for (int t = candiate_rect_vec1.size() - 1; t >= 0; t--)
		{
			if (likely_max1 < candiate_rect_vec1[t].duty_cycle)
			{
				likely_max1 = candiate_rect_vec1[t].duty_cycle;
				index1 = t;														//容器下标从0开始
			}
		}

		/*如果第1层存在符合筛选条件的候选轮廓，挑选出占空比最大的轮廓进行画图*/
		if (index1 >= 0)
		{
			CvPoint2D32f pt[4];
			for (int t = candiate_rect_vec1.size() - 1; t >= 0; t--)
			{
				cvBoxPoints(candiate_rect_vec1[t].box, pt);						//找到矩形框顶点的坐标
				CvPoint pt0, pt1;
				pt0 = cvPoint(cvRound(pt[0].x), cvRound(pt[0].y));
				pt1 = cvPoint(cvRound(pt[2].x), cvRound(pt[2].y));
				cvRectangle(dst, pt0, pt1, cvScalar(0, 255, 0), 1, 4, 0);
				cvRectangle(rectangle_detect, pt0, pt1, cvScalar(0, 255, 0), 1, 4, 0);										//画筛选后符合条件的矩形
				cvRectangle(rectangle_detect_fuse, pt0, pt1, cvScalar(255), CV_FILLED, 4, 0);								//画填充后的矩形
				printf("1 level %d contours area is %lf.\t", t, candiate_rect_vec1[t].area);								//在屏幕上输出轮廓的面积
				printf("1 level %d min_rectangle_area is %f.\t", t, candiate_rect_vec1[t].min_rectangle_area);			    //在屏幕上输出轮廓最小外接矩形的面积
				printf("1 level %d duty_cycle is %f.\n", t, candiate_rect_vec1[t].duty_cycle);								//在屏幕上输出轮廓的占空比
			}

			cvLine(dst, cvPoint(cvRound(candiate_rect_vec1[index1].box.center.x - 5),
				cvRound(candiate_rect_vec1[index1].box.center.y)),
				cvPoint(cvRound(candiate_rect_vec1[index1].box.center.x + 5),
				cvRound(candiate_rect_vec1[index1].box.center.y)),
				CV_RGB(255, 0, 0), 1);
			cvLine(dst, cvPoint(cvRound(candiate_rect_vec1[index1].box.center.x),
				cvRound(candiate_rect_vec1[index1].box.center.y - 5)),
				cvPoint(cvRound(candiate_rect_vec1[index1].box.center.x),
				cvRound(candiate_rect_vec1[index1].box.center.y + 5)),
				CV_RGB(255, 0, 0), 1);
			char a[100];
			sprintf(a, "chip1 pos:(%d , %d),angle: %d", cvRound(candiate_rect_vec1[index1].box.center.x),
				cvRound(candiate_rect_vec1[index1].box.center.y),
				cvRound(candiate_rect_vec1[index1].box.angle));
			//MessageBoxA(NULL, a, "chip1 information", MB_OK);
		}
		else
		{
			char a[100];
			sprintf(a, "No Chip1");
			//MessageBoxA(NULL, a, "chip information", MB_OK);
		}

		/*得到了第2层一些候选的区域后，设定占空比/长宽比均最接近1的候选目标为目标，用蓝色表示*/
		float likely_max2 = -1;
		int index2 = -1;
		for (int t = candiate_rect_vec2.size() - 1; t >= 0; t--)
		{
			if (likely_max2 < candiate_rect_vec2[t].duty_cycle)
			{
				likely_max2 = candiate_rect_vec2[t].duty_cycle;
				index2 = t;														//容器下标从0开始
			}
		}

		/*如果第2层存在符合筛选条件的候选轮廓，挑选出占空比最大的轮廓进行画图*/
		if (index2 >= 0)
		{
			CvPoint2D32f pt[4];
			for (int t = candiate_rect_vec2.size() - 1; t >= 0; t--)
			{
				cvBoxPoints(candiate_rect_vec2[t].box, pt);						//找到矩形框顶点的坐标
				CvPoint pt0, pt1;
				pt0 = cvPoint(cvRound(pt[0].x), cvRound(pt[0].y));
				pt1 = cvPoint(cvRound(pt[2].x), cvRound(pt[2].y));
				cvRectangle(dst, pt0, pt1, cvScalar(255, 0, 0), 1, 4, 0);
				cvRectangle(rectangle_detect, pt0, pt1, cvScalar(255, 0, 0), 1, 4, 0);										//画筛选后符合条件的矩形
				cvRectangle(rectangle_detect_fuse, pt0, pt1, cvScalar(255), CV_FILLED, 4, 0);								//画填充后的矩形
				printf("2 level %d contours area is %lf.\t", t, candiate_rect_vec2[t].area);								//在屏幕上输出轮廓的面积
				printf("2 level %d min_rectangle_area is %f.\n", t, candiate_rect_vec2[t].min_rectangle_area);			    //在屏幕上输出轮廓最小外接矩形的面积
				printf("2 level %d duty_cycle is %f.\n", t, candiate_rect_vec2[t].duty_cycle);								//在屏幕上输出轮廓的占空比
			}

			cvLine(dst, cvPoint(cvRound(candiate_rect_vec2[index2].box.center.x - 5),
				cvRound(candiate_rect_vec2[index2].box.center.y)),
				cvPoint(cvRound(candiate_rect_vec2[index2].box.center.x + 5),
				cvRound(candiate_rect_vec2[index2].box.center.y)),
				CV_RGB(255, 0, 0), 1);
			cvLine(dst, cvPoint(cvRound(candiate_rect_vec2[index2].box.center.x),
				cvRound(candiate_rect_vec2[index2].box.center.y - 5)),
				cvPoint(cvRound(candiate_rect_vec2[index2].box.center.x),
				cvRound(candiate_rect_vec2[index2].box.center.y + 5)),
				CV_RGB(255, 0, 0), 1);
			char a[100];
			sprintf(a, "chip2 pos:(%d , %d),angle: %d", cvRound(candiate_rect_vec2[index2].box.center.x),
				cvRound(candiate_rect_vec2[index2].box.center.y),
				cvRound(candiate_rect_vec2[index2].box.angle));
			//MessageBoxA(NULL, a, "chip2 information", MB_OK);
		}
		else
		{
			char a[100];
			sprintf(a, "No Chip2");
			//MessageBoxA(NULL, a, "chip information", MB_OK);
		}

		/*得到了第3层一些候选的区域后，设定占空比/长宽比均最接近1的候选目标为目标*/
		float likely_max3 = -1;
		int index3 = -1;
		for (int t = candiate_rect_vec3.size() - 1; t >= 0; t--)
		{
			if (likely_max3 < candiate_rect_vec3[t].duty_cycle)
			{
				likely_max3 = candiate_rect_vec3[t].duty_cycle;
				index3 = t;														//容器下标从0开始
			}
		}

		/*如果第3层存在符合筛选条件的候选轮廓，挑选出占空比最大的轮廓进行画图，用黄色表示*/
		if (index3 >= 0)
		{
			CvPoint2D32f pt[4];
			for (int t = candiate_rect_vec3.size() - 1; t >= 0; t--)
			{
				cvBoxPoints(candiate_rect_vec3[t].box, pt);						//找到矩形框顶点的坐标
				CvPoint pt0, pt1;
				pt0 = cvPoint(cvRound(pt[0].x), cvRound(pt[0].y));
				pt1 = cvPoint(cvRound(pt[2].x), cvRound(pt[2].y));
				cvRectangle(dst, pt0, pt1, cvScalar(255, 255, 0), 1, 4, 0);
				cvRectangle(rectangle_detect, pt0, pt1, cvScalar(255, 255, 0), 1, 4, 0);										//画筛选后符合条件的矩形
				cvRectangle(rectangle_detect_fuse, pt0, pt1, cvScalar(255), CV_FILLED, 4, 0);								//画填充后的矩形
				printf("3 level %d contours area is %lf.\t", t, candiate_rect_vec3[t].area);								//在屏幕上输出轮廓的面积
				printf("3 level %d min_rectangle_area is %f.\n", t, candiate_rect_vec3[t].min_rectangle_area);			    //在屏幕上输出轮廓最小外接矩形的面积
				printf("3 level %d duty_cycle is %f.\n", t, candiate_rect_vec3[t].duty_cycle);								//在屏幕上输出轮廓的占空比
			}

			cvLine(dst, cvPoint(cvRound(candiate_rect_vec3[index3].box.center.x - 5),
				cvRound(candiate_rect_vec3[index3].box.center.y)),
				cvPoint(cvRound(candiate_rect_vec3[index3].box.center.x + 5),
				cvRound(candiate_rect_vec3[index3].box.center.y)),
				CV_RGB(255, 0, 0), 1);
			cvLine(dst, cvPoint(cvRound(candiate_rect_vec3[index3].box.center.x),
				cvRound(candiate_rect_vec3[index3].box.center.y - 5)),
				cvPoint(cvRound(candiate_rect_vec3[index3].box.center.x),
				cvRound(candiate_rect_vec3[index3].box.center.y + 5)),
				CV_RGB(255, 0, 0), 1);
			char a[100];
			sprintf(a, "chip3 pos:(%d , %d),angle: %d", cvRound(candiate_rect_vec3[index3].box.center.x),
				cvRound(candiate_rect_vec3[index3].box.center.y),
				cvRound(candiate_rect_vec3[index3].box.angle));
			//MessageBoxA(NULL, a, "chip3 information", MB_OK);
		}
		else
		{
			char a[100];
			sprintf(a, "No Chip3");
			//MessageBoxA(NULL, a, "chip information", MB_OK);
		}

		/*输出填充后的矩形*/
		element2 = cvCreateStructuringElementEx(kernel_coefficient2 * 2 + 1, kernel_coefficient2 * 2 + 1, kernel_coefficient2, kernel_coefficient2, element_shape, 0);
		cvErode(rectangle_detect_fuse, rectangle_detect_fuse, element2, 1);					//default 3*3，进行闭操作运算，闭运算能够排除小型黑洞（黑色区域）
		cvShowImage("rectangle_detect_fuse", rectangle_detect_fuse);

		/*rectangle_detect矩形融合*/
		if ((index0 >= 0) || (index1 >= 0) || (index2 >= 0) || (index3 >= 0))
		{
			//cvThreshold(rectangle_detect, rectangle_detect, 128, 255, CV_THRESH_BINARY);				//将各种颜色的框转化为白色框
			CvMemStorage* storage_fuse = cvCreateMemStorage(0);
			CvSeq* contours_fuse;
			cvFindContours(rectangle_detect_fuse, storage_fuse, &contours_fuse, sizeof(CvContour), CV_RETR_TREE,
				CV_CHAIN_APPROX_NONE, cvPoint(0, 0));						//找二值图像中的轮廓，CV_RETR_TREE - 提取所有轮廓，并且重构嵌套轮廓的全部 hierarchy，CV_CHAIN_APPROX_NONE - 将所有点由链码形式翻译(转化）为点序列形式 

			/*在Contours_Detect_Fuse窗口中画出检测到的所有轮廓*/
			if (contours_fuse)
				cvDrawContours(contours_detect_fuse, contours_fuse, cvScalarAll(255), cvScalarAll(255), 100);				//在contours_detect中画出所有的轮廓
			cvShowImage("Contours_Detect_Fuse", contours_detect_fuse);

			for (; contours_fuse; contours_fuse = contours_fuse->h_next)
			{
				CvBox2D box_outer = cvMinAreaRect2(contours_fuse);			//函数cvMinAreaRect2可以返回一个包围轮廓最小的长方形，且这个长方形允许是倾斜的
				double area = fabs(cvContourArea(contours_fuse));			//函数cvContourArea计算整个或部分轮廓的面积。在计算部分轮廓的情况时，由轮廓弧线和连接两端点的弦围成的区域总面积被计算
				angle_correcting = box_outer.angle;							//校正矩形旋转角度，统一矩形的长宽对应的实际长度
				float aspect_ratio;
				if (angle_correcting < 45)
					aspect_ratio = box_outer.size.width / box_outer.size.height;	//计算轮廓最小外接矩形的长宽比
				else
					aspect_ratio = box_outer.size.height / box_outer.size.width;	//计算轮廓最小外接矩形的长宽比
				float min_rectangle_area = box_outer.size.width * box_outer.size.height;	//计算轮廓最小外接矩形的面积
				float duty_cycle = area / min_rectangle_area;			//定义占空比，为轮廓面积除以最小外界矩形面积
				printf("angle %f width %f height %f.\n", box_outer.angle, box_outer.size.width, box_outer.size.height);
				printf("candidate rectangle fuse aspect_ratio is %f.\n", aspect_ratio);			    //在屏幕上输出并检查轮廓最小外接矩形的长宽比，以便于确定长宽比最小和最大阈值

				CvPoint2D32f pt[4];												//声明外接矩形的4个顶点
				cvBoxPoints(box_outer, pt);						//找到矩形框顶点的坐标
				CvPoint pt0, pt1;
				pt0 = cvPoint(cvRound(pt[0].x), cvRound(pt[0].y));
				pt1 = cvPoint(cvRound(pt[2].x), cvRound(pt[2].y));
				cvRectangle(rectangle_detect_fuse_rect, pt0, pt1, cvScalar(0, 255, 255), 1, 4, 0);										//画筛选后符合条件的矩形		
				cvShowImage("Rectangle_Detect_Fuse_Rect", rectangle_detect_fuse_rect);

				if ((duty_cycle >= duty_cycle_threshold_fuse) && (aspect_ratio >= aspect_ratio_min) && (aspect_ratio <= aspect_ratio_max) && (area >= area_max))				//筛选出占空比，长宽比和面积满足一定条件的外接矩形
				{
					/*对一个ChipInfo结构进行赋值*/
					chip.box = box_outer;					//chip对象的box成员为轮廓的最小外接矩形
					chip.area = area;						//chip对象的area成员为轮廓的面积
					chip.min_rectangle_area = min_rectangle_area;		//chip对象的min_rectangle_area成员为轮廓最小外接矩形面积;						//chip对象的area成员为轮廓的面积
					chip.duty_cycle = duty_cycle;			//chip对象的duty_cycle成员为轮廓与最小外接矩形的占空比
					chip.aspect_ratio = aspect_ratio;			//chip对象的aspect_ratio成员为最小外接矩形的长宽比
					candiate_rect_vec_fuse.push_back(chip);		//将每个生成的ChipInfo对象压入ChipInfo容器之中
				}
			}

			//CvPoint2D32f pt[4];
			for (int t = candiate_rect_vec_fuse.size() - 1; t >= 0; t--)
			{
				CvPoint2D32f pt[4];													//声明外接矩形的4个顶点
				cvBoxPoints(candiate_rect_vec_fuse[t].box, pt);						//找到矩形框顶点的坐标
				CvPoint pt0, pt1;
				pt0 = cvPoint(cvRound(pt[0].x), cvRound(pt[0].y));
				pt1 = cvPoint(cvRound(pt[2].x), cvRound(pt[2].y));
				cvRectangle(dst, pt0, pt1, cvScalar(0, 0, 255), 2, 4, 0);
				cvRectangle(dst_detection, pt0, pt1, cvScalar(0, 0, 255), 2, 4, 0);
				cvRectangle(rectangle_detect_final, pt0, pt1, cvScalar(0, 0, 255), 2, 4, 0);										//画筛选后符合条件的矩形
				printf("fuse %d contours area is %lf.\t", t, candiate_rect_vec_fuse[t].area);								//在屏幕上输出轮廓的面积
				printf("fuse %d min_rectangle_area is %f.\n", t, candiate_rect_vec_fuse[t].min_rectangle_area);			    //在屏幕上输出轮廓最小外接矩形的面积
				printf("fuse %d duty_cycle is %f.\n", t, candiate_rect_vec_fuse[t].duty_cycle);								//在屏幕上输出轮廓的占空比
				printf("fuse %d aspect_ratio is %f.\n", t, candiate_rect_vec_fuse[t].aspect_ratio);			    //在屏幕上输出轮廓最小外接矩形的长宽比
			}

			cvShowImage("Rectangle_Detect_Final_Result", rectangle_detect_final);
			cvShowImage("Dst_Detection", dst_detection);
			cvSaveImage(picstr_output, dst_detection);						//输出最终的结果
		}

		/*显示结果*/
		if ((index0 >= 0) || (index1 >= 0) || (index2 >= 0) || (index3 >= 0))
		{
			cvShowImage("rectangle_detect", rectangle_detect);
			cvSaveImage("rectangle_detect.jpg", rectangle_detect);
			cvShowImage("dst", dst);
			cvSaveImage("dst.jpg", dst);
		}

		s_input = "F:\\图片\\飞机舱门图片\\snap1\\snap1\\snap00";					//清零上一张图片的名字，循环读取下一张图片
		s_output = "F:\\图片\\飞机舱门图片\\1_6_4_v1\\snap00";				//清零上一张图片的名字，循环输出下一张图片
		s1 = "";
	}

	cvWaitKey(0);
	return 0;
}

int otsu(const IplImage* image, int x0, int y0, int dx, int dy)
{

	unsigned char *np;      // 图像指针
	int thresholdValue = 1; // 阈值
	int ihist[256];             // 图像直方图，256个点

	int i, j, k;          // various counters
	int n, n1, n2, gmin, gmax;
	double m1, m2, sum, csum, fmax, sb;

	// 对直方图置零...
	memset(ihist, 0, sizeof(ihist));

	gmin = 255; gmax = 0;
	// 生成直方图
	for (i = y0 + 1; i < y0 + dy - 1; i++)
	{
		np = (uchar*)(image->imageData + i*image->widthStep);
		// np = &image[i*cols+x0+1];
		for (j = x0 + 1; j < x0 + dx - 1; j++)
		{
			ihist[*np]++;
			if (*np > gmax) gmax = *np;
			if (*np < gmin) gmin = *np;
			np++; /* next pixel */
		}
	}

	// set up everything
	sum = csum = 0.0;
	n = 0;

	for (k = 0; k <= 255; k++) {
		sum += (double)k * (double)ihist[k];    /* x*f(x) 质量矩*/
		n += ihist[k];                                         /*  f(x)    质量    */
	}

	if (!n)
	{
		// if n has no value, there is problems...
		return -1;
	}

	// do the otsu global thresholding method
	fmax = -1.0;
	n1 = 0;
	for (k = 0; k < 255; k++) {
		n1 += ihist[k];
		if (!n1) { continue; }
		n2 = n - n1;
		if (n2 == 0) { break; }
		csum += (double)k *ihist[k];
		m1 = csum / n1;
		m2 = (sum - csum) / n2;
		sb = (double)n1 *(double)n2 *(m1 - m2) * (m1 - m2);
		/* bbg: note: can be optimized. */
		if (sb > fmax) {
			fmax = sb;
			thresholdValue = k;
		}
	}

	// at this point we have our thresholding value

	// debug code to display thresholding values
	//cvWaitKey(0);
	return(thresholdValue);
}