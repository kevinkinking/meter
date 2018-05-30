#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/dnn/shape_utils.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include "files.h"
#include "utools.h"

using namespace cv;
using namespace cv::dnn;

using namespace std;

const float confidenceThreshold_iou = 0.60f;
const float confidenceThreshold_num = 0.80f;

string class_names_iou[2] = {"background",
                          "iou"};
string class_names_number[12] = {"background",
                          "1","2","3","4","5","6","7","8","9","0","iou"};

dnn::Net net_iou;
dnn::Net net_num;

bool ini_model(string txt_file, string bin_file, dnn::Net &net)
{
    net = readNetFromCaffe(txt_file, bin_file);
    cout<<"------use opencl-------"<<endl;
    net.setPreferableTarget(DNN_TARGET_OPENCL);

    if (net.empty())
    {
        //这句话没有打印就代表模型初始化成功
        cerr << "Can't load network by using the input files: " << endl;
        exit(-1);//报错退出
    }
    return true;
}

void detect_img_iou(cv::Mat frame, vector<cv::Rect> &rects, vector<int> &ids, dnn::Net net)
{
    if (frame.empty())
    {
        return;
    }

    if (frame.channels() == 4)
        cvtColor(frame, frame, COLOR_BGRA2BGR);

    Mat inputBlob = blobFromImage(frame, 0.007843f,Size(300, 300), Scalar(127.5f, 127.5f, 127.5f), false, false);

    net.setInput(inputBlob, "data");

    Mat detection = net.forward("detection_out");


    Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());


    for(int i = 0; i < detectionMat.rows; i++)
    {
        float confidence = detectionMat.at<float>(i, 2);
        if(confidence > confidenceThreshold_iou)
        {
            float objectClass = detectionMat.at<float>(i, 1);
            int xLeftBottom = static_cast<int>(detectionMat.at<float>(i, 3) * frame.cols);
            int yLeftBottom = static_cast<int>(detectionMat.at<float>(i, 4) * frame.rows);
            int xRightTop = static_cast<int>(detectionMat.at<float>(i, 5) * frame.cols);
            int yRightTop = static_cast<int>(detectionMat.at<float>(i, 6) * frame.rows);
            int width = xRightTop - xLeftBottom;
            int height = yRightTop - yLeftBottom;

            xRightTop+=width/20;
            xLeftBottom-=width/20;
            if(xRightTop > frame.cols)
            {
                xRightTop = frame.cols;
            }
            if(xLeftBottom < 0)
            {
                xLeftBottom = 0;
            }

            yLeftBottom-=height/20;
            yRightTop+=height/20;
            if(yRightTop > frame.rows)
            {
                yRightTop = frame.rows;
            }
            if(yLeftBottom < 0)
            {
                yLeftBottom = 0;
            }
            width = xRightTop - xLeftBottom;
            height = yRightTop - yLeftBottom;

            if(width > height)
            {
                int add = width - height;
                yLeftBottom-=(add/2);
                yRightTop+=(add/2);
                if(yLeftBottom < 0)
                {
                    yRightTop-=yLeftBottom;
                    yLeftBottom = 0;
                }
                if(yRightTop > frame.rows)
                {
                    yLeftBottom-=(yRightTop - frame.rows);
                    yRightTop = frame.rows;
                }

            }else if(height > width)
            {
                int add = height - width;
                xLeftBottom-=(add/2);
                xRightTop+=(add/2);
                if(xLeftBottom < 0)
                {
                    xRightTop -=xLeftBottom;
                    xLeftBottom = 0;
                }
                if(xRightTop > frame.cols)
                {
                    xLeftBottom-=(xRightTop - frame.cols);
                    xRightTop = frame.cols;
                }

            }
            rects.push_back(cv::Rect(xLeftBottom,yLeftBottom,xRightTop-xLeftBottom,yRightTop-yLeftBottom));

            ids.push_back(int(objectClass));
        }
    }
}

void detect_img_number(cv::Mat frame,vector<cv::Rect> &rects,vector<int> &ids, dnn::Net net)
{
    if (frame.empty())
    {
        return;
    }

    if (frame.channels() == 4)
        cvtColor(frame, frame, COLOR_BGRA2BGR);

    Mat inputBlob = blobFromImage(frame, 1.0f,Size(300, 300), Scalar(127.5f, 127.5f, 127.5f), false, false);

    net.setInput(inputBlob, "data");

    Mat detection = net.forward("detection_out");


    Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());


    for(int i = 0; i < detectionMat.rows; i++)
    {
        float confidence = detectionMat.at<float>(i, 2);
        if(confidence > confidenceThreshold_num)
        {
            float objectClass = detectionMat.at<float>(i, 1);
            int xLeftBottom = static_cast<int>(detectionMat.at<float>(i, 3) * frame.cols);
            int yLeftBottom = static_cast<int>(detectionMat.at<float>(i, 4) * frame.rows);
            int xRightTop = static_cast<int>(detectionMat.at<float>(i, 5) * frame.cols);
            int yRightTop = static_cast<int>(detectionMat.at<float>(i, 6) * frame.rows);
            rects.push_back(cv::Rect(xLeftBottom,yLeftBottom,xRightTop-xLeftBottom,yRightTop-yLeftBottom));

            ids.push_back(int(objectClass));
        }
    }
}

int main(int argc, char** argv)
{
    vector<string> img_names;
    string dir_path = "../img";

    readFileList(dir_path, img_names);

    ini_model("../model/iou.prototxt", "../model/iou.caffemodel", net_iou);
    ini_model("../model/num.prototxt", "../model/num.caffemodel", net_num);

    vector<string>::iterator img_name_iter;
    for(img_name_iter = img_names.begin();img_name_iter!=img_names.end();img_name_iter++)
    {
        string img_path =dir_path + "/" + *img_name_iter;
        cout<<img_path<<endl;
        cv::Mat img = imread(img_path);

        vector<cv::Rect> rects_iou;
        vector<int> ids_iou;

        double time_b = getTickCount();
        detect_img_iou(img, rects_iou, ids_iou, net_iou);
        double time_e = getTickCount();
        double freq = getTickFrequency() / 1000;

        double time = (time_e - time_b) / freq;
        cout << "Inference time, ms: " << time << endl;

        for(int i=0;i<rects_iou.size();i++)
        {
            Mat iou_img = img(rects_iou[i]);
            // imwrite(save_path,iou_img);

            vector<cv::Rect> rects_num;
            vector<int> ids_num;
            double time_b = getTickCount();
            detect_img_number(iou_img, rects_num, ids_num, net_num);
            double time_e = getTickCount();
            double freq = getTickFrequency() / 1000;

            double time = (time_e - time_b) / freq;
            cout << "Inference time1, ms: " << time << endl;
            vector<int> sorted_indexs;
            rects_sort(rects_num,sorted_indexs);
            for(int j = 0;j<sorted_indexs.size();j++)
            {
                int index = sorted_indexs[j];
                rectangle(iou_img,rects_num[index],Scalar(0,255,0));
                if(ids_num[index] == 11)
                {
                    continue;
                }
                cout<<class_names_number[ids_num[index]];
                putText(img,class_names_number[ids_num[index]],Point(20 * j,50),2,1,Scalar(0,0,255));
            }
            cout<<endl;
        }
        imshow("ssd",img);
        int key = waitKey(0);
        if(key==27)
        {
            break;
        }
    }

    return 0;
}
