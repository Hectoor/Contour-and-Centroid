#include <iostream>
#include <opencv2/opencv.hpp>
#include <math.h>

using namespace std;
using namespace cv;

Mat contours_find( Mat src, Mat dst, std::vector<std::vector<Point>> *contours)
{
   
	//显示原图
    namedWindow("input", CV_WINDOW_NORMAL);
    imshow( "input", src ); 
    cv::waitKey(0);
	//在边缘添加矩形边
    cv::rectangle( src, cvPoint( 0, 0 ), cvPoint( src.cols, src.rows ), Scalar( 255, 255, 0 ), 30, 4, 0 );
    imshow("tianchong", src);
    imwrite("tianchong.jpg", src);
    waitKey(0);

	//定义原图大小且值为0的矩阵
    dst = Mat::zeros( src.size(), CV_8UC3 );
    //dst2 = Mat::zeros( src.size(), CV_8UC3 );
    //blur(src,src,Size(3,3));     //jun zhi lv bo 

	//将bgr图片转换成灰度图
    cvtColor(src,src,COLOR_BGR2GRAY);   // bgr->gray
    imshow("gray_pic ", src);
    //waitKey();
    
	//对图像进行边缘提取    
    Canny(src, src, 20, 40, 3, false);  
    imshow("canny ", src);
    //waitKey();
	
	//第一次找到轮廓,先使用全白画出轮廓图
    std::vector<Vec4i> hierarchy;
    findContours(src, *contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

    RNG rng(0);  
    for(int i = 0; i < contours->size(); i++)
    {
        drawContours(dst, *contours, i, Scalar( 255, 255, 255 ), 5, 10, hierarchy, 1, Point(0,0));
    }
    cout <<"contours size :" << contours->size() << endl;
    namedWindow("output", CV_WINDOW_NORMAL);
    imshow("output",dst);
    imwrite("Contoursoutput.jpg", dst);
    //对画出来的轮廓图转成灰度图
    cvtColor( dst, dst, COLOR_RGB2GRAY);
    //Canny(dst, dst, 20, 80, 3, false); //不需要进行第二次边缘检测,效果有无都差不多
    imshow("cannny2",dst);
	//第二次找到轮廓,用不同的颜色区分,比较好看，可以看到轮廓基本连通
    findContours(dst, *contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
     dst = Mat::zeros( dst.size(), CV_8UC3 );
    for(int i = 0; i < contours->size(); i++)
    {
        Scalar color = Scalar(rng.uniform(0,255), rng.uniform(0,255), rng.uniform(0,255));
        drawContours(dst, *contours, i, color, 5, 8, hierarchy, 1, Point(0,0));
    }
    cout <<"contours size :" << contours->size() << endl;
    namedWindow("output2", CV_WINDOW_NORMAL);
    imshow("output2",dst);
    imwrite("Contoursoutput.jpg", dst);
    return dst;
}

void Bundle_sort(Point3d pa[], int count_Pointsize)
{
    for( int count_area1 = 0; count_area1 < count_Pointsize - 1; count_area1++ )
    {
        bool flag_area = false; 
        for( int count_area2 = 0; count_area2 < count_Pointsize - 1 - count_area1; count_area2++ )
        {
            if( pa[count_area2].z > pa[count_area2 + 1].z)
            {
                Point3d temp = { 0, 0, 0 };
                temp = pa[count_area2];
                pa[count_area2] = pa[count_area2 + 1];
                pa[count_area2 + 1] = temp;
                flag_area = true;
            }
        }
        if( flag_area == false)
        {
            break;
        }
    }
    // test 
       for( int test_area = 0; test_area < count_Pointsize; test_area ++ )
    {
        cout << "area:"<< pa[test_area].z << endl;
    }
}

int compute_center_of_gravity( Mat dst2, std::vector<std::vector<Point>> contours, int contours_size, Point3d pa[] )
{
    int i = 0;
    Point pt[100];//假设有100个连通区域
    int count_Pointsize = 0;
    Moments moment;//矩
    for (int i = 0; i < contours_size; ++i) 
    {
        cout << " area:"<< contourArea( contours[i], false ) << endl;
        if( contourArea( contours[i] ) > 1000 )
        {
            cout << "i :" << i << endl;
            Mat temp(contours.at(i));
            Scalar color(0, 255, 255);
            moment = moments(temp, false);
            if (moment.m00 != 0)//除数不能为0
            {
                pt[i].x = cvRound(moment.m10 / moment.m00);//计算重心横坐标
                pt[i].y = cvRound(moment.m01 / moment.m00);//计算重心纵坐标
            }
            cout << "Point :" << pt[i].x << "  " << pt[i].y << endl;
            Point p = Point(pt[i].x, pt[i].y);//重心坐标
            circle(dst2, p, 1, color, 30, 8);//原图画出重心坐标
            namedWindow( "test", WINDOW_NORMAL);
            imshow("test", dst2);
            cv::waitKey();

            pa[count_Pointsize].x =  pt[i].x ;
            pa[count_Pointsize].y =  pt[i].y ;
            pa[count_Pointsize].z =  (int)contourArea( contours[i], false );
            count_Pointsize ++;
        }
    }
    return count_Pointsize;
}


void nms_point( Mat White, Point3d pa[], Point3d pb[], int count_Pointsize )
{
    // index_size 大小为中心点的大小,index_result
    int index_size = count_Pointsize;
    int index_result = 0;
    int threshold_set = 50;
    Point p[100];
    while( index_size > 0 )
    {
        //先把pa数组里最大的值放到pb,pb是存放最后的点,同时index_size数量下降1
        pb[index_result] = pa[index_size - 1];
        p[index_result] = Point(pb[index_result].x, pb[index_result].y);
        
        cout << " pb : " ;
		for( int test11 = 0; test11 <= index_result; test11 ++)
        {
            cout << pb[test11] << "  " ;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              
        }
        cout << endl;
		for( int count_point = 0; count_point < index_size - 1; count_point++)
		{
            
            int distance = sqrt(pow((pb[index_result].x - pa[count_point].x),2) + pow( (pb[index_result].y - pa[count_point].y), 2));
            cout << "distance : " << distance << endl;
            //如果小于阀值,说明距离很接近,则覆盖掉
			if( distance < threshold_set)
            {		
               // cout << " distance is too short" << endl;
            
				for( int count_point2 = count_point ; count_point2 < index_size - 1; count_point2++ )
				{
                 
					pa[count_point2] = pa[count_point2 + 1];
				}
				index_size --;
            }
		}
        index_size --;
        cout << "index_size :" << index_size << endl;
        index_result ++;
    }
    
    for ( int nms_point = 0; nms_point < index_result; nms_point ++)
    {
        circle(White, p[nms_point] , 1, Scalar(0, 255, 255), 30, 8);//原图画出重心坐标
        namedWindow( "final_result", WINDOW_NORMAL);
        imshow( "final_result", White );
    }
}

int main(int argc, char **argv) {
    Mat src, dst, White;
    Point3d pa[100], pb[100];
    std::vector<std::vector<Point>> contours;
    int count_Pointsize;
    src = imread( "11.jpg" );	//读取本地图片
    if( src.empty() )			//检测是否文件为空
    {
        cout<<"can not load this pic!"<<endl;
        exit(0);
       // return -1;
    }
    White = src;
    
    dst = contours_find( src, dst,  &contours );
    //计算连通区域的质心坐标
    count_Pointsize = compute_center_of_gravity( dst, contours, contours.size(), pa );
    //Bundle sort 
    Bundle_sort( pa, count_Pointsize);
    nms_point( White, pa, pb, count_Pointsize );
    cout << "count_Pointsize:" << count_Pointsize << endl;
    cout<<"轮廓数量："<<contours.size()<<endl;
    imwrite("Center.jpg", White);
    waitKey();
    return 0;
}
