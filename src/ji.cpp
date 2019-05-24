/*
**
    仅用于演示
    ji.h的实现,包括license、模型加密、感兴趣区域的实现
**
*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <opencv2/opencv.hpp>

#include "ji.h"
#include "ji_license.h"
#include "pubKey.hpp"  // 由一键sdk生成
#include "cJSON.h"
#include "BoostInterface.h"

#include "encrypt_wrapper.hpp"

#include "model_str.hpp" 
#include <glog/logging.h>
#include "yolo.h"

using namespace std;


#define OPEN_PRINT 0
#if OPEN_PRINT 
#define PRINT(format,...) printf("File: "__FILE__", Line: %d: "format"\n", __LINE__, ##__VA_ARGS__)
#else  
#define PRINT(format,...)
#endif


/* 
** 说明
ji_calc_frame
ji_calc_buffer
ji_calc_file
ji_calc_video_file
    -------------->
    CustomPredictor::calc
        -------------->
        CustomPredictor.runByFrame
**
*/

/* 自定义检测器示例 */
/* 
** 说明
CustomPredictor: 
自定义检测器，有算法工程师补充实现；
需要补充实现的函数有create(),destroy(),checkAlarm(),runByFrame,共四个；
根据需要，算法开发者可自行增加其它函数。
**
*/
class CustomPredictor
{
public:
    /* 初始化 */    
    void create(int _pbtype)  
    {
        /* 随机种子，用于模拟算法输出，实际开发时可屏蔽该行代码 ??? */
        srand(time(NULL));


        /* 请参考    检测器类型值定义 */
        pbtype = _pbtype;
        predictor = NULL;

        /* 解析算法配置属性 /sdk/model/algo_config.json ??? */
        m_draw_roi_area = 0;
        m_draw_result   = 0;
        m_show_result   = 0;
        m_gpu_id        = 0;
        m_threshold     = 0.1;
        if (read_config("/usr/local/ev_sdk/model/algo_config.json") != true)
        {
            LOG(WARNING) << "parse config file failed!";
        }


        /* 
        ** 模块加解密 加密模块只对模型配置文件进行加密，不对模型文件加密，model_str.hpp是加密后的网络配置文件，
           要解析该配置文件，需要修改相应框架加载模型的api源码
        for example:
        对于darknet框架：
        ./3rd/bin/encrypt_model yolov3.cfg 01234567890123456789012345678988
        对于caffe框架：
        ./3rd/bin/encrypt_model deploy.prototxt 01234567890123456789012345678988
        out file: model_str.hpp, copy this file to "./src" directory and add to ev_sdk project.
        **
        */

        // darknet 加密示例
        // predictor = new yolo("/usr/local/ev_sdk/model/coco.data", 0.3);
        
        // void *decryptor = CreateEncryptor(model_str.c_str(), model_str.size(), key.c_str());
        // int fileLen = 0;
        // void *cfg_buff = FetchBuffer(decryptor, fileLen);

        // yolo *predictor_temp = (yolo*)predictor;
        // predictor_temp->loadNetwork((char *)cfg_buff, "/usr/local/ev_sdk/model/yolov3.weights"); // 此接口修改了darknet框架加载模型源码

        // DestroyEncrtptor(decryptor);


        /* allocate predictor ??? */
        predictor = new yolo("/usr/local/ev_sdk/model/yolov3.cfg",
                            "/usr/local/ev_sdk/model/yolov3.weights",
                            "/usr/local/ev_sdk/model/coco.data",
                            m_threshold, m_gpu_id);
        
    }

    /* 反初化 */
    void destroy() 
    {
        /* destroy predictor */
        if (predictor)
        {
            delete (yolo*)predictor;
        }
    }

    /* 分析一帧--静态成员函数 */
    static int calc(void *predictor, const cv::Mat &inMat, const char *args, 
                    cv::Mat &outMat, JI_EVENT *event)
    {
        int iRet = ji_check_expire();
        if (iRet != EV_SUCCESS)
        {
            return (iRet == EV_OVERMAXQPS)?JISDK_RET_OVERMAXQPS:JISDK_RET_UNAUTHORIZED;
        }

        CustomPredictor *pcp = (CustomPredictor*)predictor;
        if (!pcp->runByFrame(inMat,args?args:"",pcp->mat,pcp->json))
        {
            return JISDK_RET_FAILED;
        }

        outMat = pcp->mat;
        if (!pcp->json.empty())
        {
            if (event)
            {
                /* event->code: 非报警类算法，直接返回JISDK_CODE_ALARM */
                event->code = pcp->checkAlarm(pcp->json)?JISDK_CODE_ALARM:JISDK_CODE_NORMAL;
                event->json = pcp->json.c_str();        
            }
        }
        else 
        {
            if (event)
            {
                
                event->code = JISDK_CODE_FAILED;
                event->json = NULL;
            }
        }

        return JISDK_RET_SUCCEED;
    }

private:

    bool read_config(const string& srcFile)
    {
        std::ifstream ifs(srcFile.c_str(),std::ifstream::binary);
        if (!ifs.is_open())
        {
            LOG(ERROR) << "[ERROR] open config file failed:  " << srcFile;
            return false;
        }
        
        std::filebuf *fbuf = ifs.rdbuf();

        //get file size
        std::size_t size = fbuf->pubseekoff(0,ifs.end,ifs.in);
        fbuf->pubseekpos(0,ifs.in);

        //get file data
        char* buffer = new char[size+1];
        fbuf->sgetn(buffer,size);
        ifs.close();
        buffer[size] = '\0';

        cJSON *jsonRoot = NULL, *sub;
        bool bRet = false;
        do 
        {
            jsonRoot = cJSON_Parse(buffer);
            if (jsonRoot == NULL)
            {
                LOG(WARNING) << "parse config file failed:"<< buffer;
                break;
            }

            sub = cJSON_GetObjectItem(jsonRoot, "draw_roi_area");
            if (sub == NULL || sub->type != cJSON_Number)
            {
                LOG(WARNING) << "parse config file failed:"<< buffer;
                break;
            }
            m_draw_roi_area = sub->valueint;

            sub = cJSON_GetObjectItem(jsonRoot, "draw_result");
            if (sub == NULL || sub->type != cJSON_Number)
            {
                LOG(WARNING) << "parse config file failed:"<< buffer;
                break;
            }
            m_draw_result = sub->valueint;

            sub = cJSON_GetObjectItem(jsonRoot,"show_result");
            if (sub == NULL || (sub->type != cJSON_Number))
            {
                LOG(WARNING) << "parse config file failed:"<< buffer;
                break;
            }
            m_show_result = sub->valueint;

            sub = cJSON_GetObjectItem(jsonRoot, "gpu_id");
            if (sub == NULL || sub->type != cJSON_Number)
            {
                LOG(WARNING) << "parse config file failed:"<< buffer;
                break;
            }
            m_gpu_id = sub->valueint;
            sub = cJSON_GetObjectItem(jsonRoot, "threshold");
            if (sub == NULL || sub->type != cJSON_Number)
            {
                LOG(WARNING) << "parse config file failed:"<< buffer;
                break;
            }
            m_threshold = sub->valuedouble;

            bRet = true;
        } while (0);

        if (jsonRoot) cJSON_Delete(jsonRoot);
        if (buffer) delete [] buffer;
        return bRet;
    }

    bool checkAlarm(const std::string& content) 
    {
        /* 判断是否报警 ??? */
        cJSON *jsonRoot = cJSON_Parse(content.c_str());
        if (jsonRoot == NULL)
        {
            return false; 
        }

        bool bRet = false;
        cJSON *pSubJson = cJSON_GetObjectItem(jsonRoot, "alertFlag");
        if(pSubJson && (pSubJson->type == cJSON_Number))
        {
            bRet = (1 == pSubJson->valueint);
        }

        cJSON_Delete(jsonRoot);
        return bRet;

    }
    
    /* 分析一帧 */
    bool runByFrame(const cv::Mat &inMat, const std::string &args, 
                        cv::Mat &outMat, std::string& strJson) 
    {
        /* 规避修改源mat，算法操作请在destMat进行，其中destMat指向outMat即共享内存空间 */
        inMat.copyTo(outMat);
        cv::Mat destMat = outMat;

        /* 模拟args(roi)的处理 */
        int shift_width  = 0;  // roi 相对原图的偏移坐标
        int shift_height = 0;
        if (!args.empty())
        {   
            std::vector<cv::Point> v_point;
            BoostInterface obj;
            obj.parsePolygon(args.c_str(), outMat.size(), v_point);
            if (!v_point.empty())
            {
                cv::Rect roi = obj.polygon2Rect(v_point);
                destMat = outMat(roi);
                shift_width = roi.x;
                shift_height = roi.y;
            }

            // roi显示
            if (m_draw_roi_area)
            {
                int count = v_point.size();
                cv::Point rook_points[1][count];
                for(int j = 0; j < count; j++)
                {
                        rook_points[0][j] = v_point[j];
                }

                const cv::Point* ppt[] = { rook_points[0] };
                int npt[] = {count};
                cv::polylines(outMat, ppt, npt, 1, true, cv::Scalar(0, 255, 0), 3, 8, 0);
            }
        }

        /* 算法实现     ??? */
        VEC_TARGET det_vec;
        yolo *predictor_temp =(yolo*)predictor;
        predictor_temp->detect(destMat, det_vec);
        
        // 解析成json格式
        cJSON *root, *js_body, *js_list;
        root = cJSON_CreateObject();
        if (det_vec.size() > 0)
	    {   
            cJSON_AddItemToObject(root, "alertFlag", cJSON_CreateNumber(1));
            cJSON_AddItemToObject(root,"detectInfo", js_body = cJSON_CreateArray());
            int count = det_vec.size();
            for (int i = 0; i < count; ++i)
            {
                det_vec[i].rect.x += shift_width;
                det_vec[i].rect.y += shift_height;
                cJSON_AddItemToArray(js_body, js_list = cJSON_CreateObject());
                cJSON_AddNumberToObject(js_list,"x",      det_vec[i].rect.x);
                cJSON_AddNumberToObject(js_list,"y",      det_vec[i].rect.y);
                cJSON_AddNumberToObject(js_list,"height", det_vec[i].rect.height);
                cJSON_AddNumberToObject(js_list,"width",  det_vec[i].rect.width);
                
                // 检测结果显示
                if (m_draw_result)
                {
                    cv::rectangle(outMat,  det_vec[i].rect, cv::Scalar(0,0,255),1,1,0);
                    cv::putText(outMat,"object",cv::Point(det_vec[i].rect.x, det_vec[i].rect.y),cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0,255),2,8,0);
                }

            }
            
            cJSON_AddItemToObject(root, "message", cJSON_CreateString("object is being detected!"));
            cJSON_AddItemToObject(root, "numOfFlameRects", cJSON_CreateNumber(count));
        } 
        else
        {
            cJSON_AddItemToObject(root, "alertFlag", cJSON_CreateNumber(0));
            cJSON_AddItemToObject(root,"detectInfo", js_body = cJSON_CreateArray());
            cJSON_AddItemToObject(root, "message", cJSON_CreateString("nothing is being detected!"));
            cJSON_AddItemToObject(root, "numOfFlameRects", cJSON_CreateNumber(0));
        }

        char *out = cJSON_Print(root);
        if (out)
        {
            strJson = out;
            free(out);
            out = NULL;
        }
        else
        {
             strJson = "";
        }
        
        if (root)
            cJSON_Delete(root);
        // 显示结果
        if (m_show_result)
        {
            cv::imshow("result", outMat);
            cv::waitKey(1);
        }
    
        
        return true;
    }
    
private:
    int pbtype;
    void *predictor;

    cv::Mat mat;
    std::string json;
 
    // 算法私有参数
    int m_draw_roi_area;
    int m_draw_result;
    int m_show_result;
    int m_gpu_id;
    double m_threshold;       // 置信度
};

int ji_init(int argc, char **argv)
{
/* 
** 参数说明
for example:
argc：4
argv[0]: $license
argv[1]: $timestamp
argv[2]: $qps
argv[3]: $version
...
**
*/
    if (argc < 4 )
    {
        return JISDK_RET_INVALIDPARAMS;
    }

    if (argv[0] == NULL || 
        argv[3] == NULL)
    {
        return JISDK_RET_INVALIDPARAMS;
    }

    int qps = 0;
    if (argv[2]) qps = atoi(argv[2]);

    /* 
    ** 请替换算法的公钥
    for example:
    ./3rd/license/v5/ev_license -c pubKey.txt
    ** 
    */
    return (ji_check_license(pubKey,argv[0], argv[1], qps>0?&qps:NULL, atoi(argv[3])) == EV_SUCCESS)?
            JISDK_RET_SUCCEED:
            JISDK_RET_UNAUTHORIZED;
}

void* ji_create_predictor(int pdtype)
{
    if (ji_check_expire() != EV_SUCCESS)
    {
        return NULL;
    }

    CustomPredictor *pcp = new CustomPredictor();
    pcp->create(pdtype);
    return pcp;
}

void ji_destroy_predictor(void *predictor)
{
    if (predictor == NULL) return ;

    CustomPredictor *pcp = (CustomPredictor*)predictor;
    pcp->destroy();
    delete pcp;
}

int ji_calc_frame(void *predictor, const JI_CV_FRAME *inframe, const char *args,
                  JI_CV_FRAME *outframe, JI_EVENT *event)
{
    if (predictor == NULL || 
        inframe   == NULL )
    {
        return JISDK_RET_INVALIDPARAMS;
    }
    
    cv::Mat inMat(inframe->rows,inframe->cols,inframe->type,inframe->data,inframe->step);
    if (inMat.empty())
    {
        return JISDK_RET_FAILED; 
    }

    cv::Mat outMat;
    int iRet = CustomPredictor::calc(predictor, inMat, args, outMat, event);
    if (iRet == JISDK_RET_SUCCEED)
    {
        if ((event->code != JISDK_CODE_FAILED) &&
            (!outMat.empty()) &&
            (outframe))
        {
            outframe->rows = outMat.rows;
            outframe->cols = outMat.cols;
            outframe->type = outMat.type();
            outframe->data = outMat.data;
            outframe->step = outMat.step;
        }
    }

    return iRet;
}

int ji_calc_buffer(void *predictor, const void *buffer, int length,
                 const char *args, const char *outfile, JI_EVENT *event)
{
    if (predictor == NULL || 
        buffer    == NULL || 
        length    <= 0 )
    {
        return JISDK_RET_INVALIDPARAMS;
    }

    const unsigned char * b = (const unsigned char*)buffer;
    std::vector<unsigned char> vecBuffer(b,b+length);
    cv::Mat inMat = cv::imdecode(vecBuffer, cv::IMREAD_COLOR);
    if (inMat.empty())
    {
        return JISDK_RET_FAILED; 
    }

    cv::Mat outMat;
    int iRet = CustomPredictor::calc(predictor, inMat, args, outMat, event);
    if (iRet == JISDK_RET_SUCCEED)
    {
        if ((event->code != JISDK_CODE_FAILED) &&
            (!outMat.empty()) &&
            (outfile))
        {
            cv::imwrite(outfile,outMat);
        }
    }

    return iRet;
}
                  
int ji_calc_file(void *predictor, const char *infile, const char *args,
                 const char *outfile, JI_EVENT *event)
{
    if (predictor == NULL || 
        infile    == NULL )
    {
        return JISDK_RET_INVALIDPARAMS;
    }

    cv::Mat inMat = cv::imread(infile);
    if (inMat.empty())
    {
        return JISDK_RET_FAILED;
    }

    cv::Mat outMat;
    int iRet = CustomPredictor::calc(predictor, inMat, args, outMat, event);
    if (iRet == JISDK_RET_SUCCEED)
    {
        if ((event->code != JISDK_CODE_FAILED) &&
            (!outMat.empty()) &&
            (outfile))
        {
            cv::imwrite(outfile,outMat);
        }
    }
    
    return iRet;
}

int ji_calc_video_file(void *predictor, const char *infile, const char* args,
                       const char *outfile, const char *jsonfile)
{
/* 根据算法需求，输出格式有所不同，请完善。??? */

/*  
** json 格式(算法不同输出格式也有所不同)：
** for example:
{
    "totalFrames": 2,
    "alertFrames": 1,
    "detail": [
        {
            "timestamp": 1000000,
            "alertFlag": 1,
            "totalHeads": 2,
            "headInfo": [
                {
                    "x": 10,
                    "y": 15,
                    "width": 25,
                    "height": 20,
                    "haveHelmet": 1
                },
                {
                    "x": 300,
                    "y": 500,
                    "width": 15,
                    "height": 20,
                    "haveHelmet": 0
                }
            ]
        },
        {
            "timestamp": 1000001,
            "alertFlag": 0,
            "totalHeads": 1,
            "headInfo": [
                {
                    "x": 10,
                    "y": 15,
                    "width": 25,
                    "height": 20,
                    "haveHelmet": 1
                }
            ]
        }
    ]
}
**
*/
    if (predictor == NULL ||
        infile    == NULL )
    {
        return JISDK_RET_INVALIDPARAMS;
    }

    cv::VideoCapture vcapture(infile);
    if (!vcapture.isOpened())
    {
        return JISDK_RET_FAILED;
    }

    cv::VideoWriter vwriter;
    cv::Mat inMat, outMat;
    JI_EVENT event;
    int iRet = JISDK_RET_FAILED;
    int totalFrames, alertFrames, timestamp;
    totalFrames = alertFrames = timestamp = 0;

    cJSON *jsonRoot, *jsonDetail;
    jsonRoot = jsonDetail = NULL;
    
    while (vcapture.read(inMat))
    {
        timestamp = vcapture.get(CV_CAP_PROP_POS_MSEC);
        
        iRet = CustomPredictor::calc(predictor, inMat, args, outMat, &event);
        if (iRet == JISDK_RET_SUCCEED)
        {
            ++totalFrames;
            
            if (event.code != JISDK_CODE_FAILED)
            {
                if (event.code == JISDK_CODE_ALARM)
                {
                    ++alertFrames;
                }

                if (!outMat.empty() && outfile)
                {
                    if (!vwriter.isOpened())
                    {
                        vwriter.open(outfile,
                                    vcapture.get(cv::CAP_PROP_FOURCC),
                                    vcapture.get(cv::CAP_PROP_FPS),
                                    outMat.size());
                                    //cv::Size(outMat.cols,outMat.rows));
                        if (!vwriter.isOpened())
                        {
                            return JISDK_RET_FAILED;
                        }
                    }
                    vwriter.write(outMat);
                }

                if (event.json && jsonfile)
                {
                    if (jsonDetail == NULL)
                    {
                        jsonDetail = cJSON_CreateArray();
                    }
                    
                    cJSON *jsonFrame = cJSON_Parse(event.json);
                    if (jsonFrame)
                    {
                        cJSON_AddItemToObjectCS(jsonFrame,"timestamp", cJSON_CreateNumber(timestamp)); 
                        cJSON_AddItemToArray(jsonDetail,jsonFrame);
                    }
                }
            }
        }
        else 
        {
            break;
        }
    }

    if (iRet == JISDK_RET_SUCCEED)
    {
        if (jsonfile)
        {
            jsonRoot = cJSON_CreateObject();
            cJSON_AddItemToObjectCS(jsonRoot, "totalFrames", cJSON_CreateNumber(totalFrames));
            cJSON_AddItemToObjectCS(jsonRoot, "alertFrames", cJSON_CreateNumber(alertFrames));

            if (jsonDetail)
            {
                cJSON_AddItemToObjectCS(jsonRoot,"detail", jsonDetail);
            }

            char *buff = cJSON_Print(jsonRoot); 
            std::ofstream fs(jsonfile);
            if (fs.is_open())
            {
                fs << buff;
                fs.close();
            }
            free(buff);
        }
    }

    if (jsonRoot)
    {
        cJSON_Delete(jsonRoot);
    }
    else if (jsonDetail)
    {
        cJSON_Delete(jsonDetail);
    }

    return iRet;
}
