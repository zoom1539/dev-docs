/**
 * 示例：实现ji.h定义的图像接口，开发者需要根据自己的实际需求对接口进行实现
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fstream>

#include <opencv2/opencv.hpp>
#include <glog/logging.h>

#include "ji.h"

#include "ji_license.h"
#include "pubKey.hpp"

#include "encrypt_wrapper.hpp"
#include "model_str.hpp"

#include "BoostInterface.h"
#include "cJSON.h"

#include <ClassifierPredictor.hpp>

// 如果需要添加授权功能，请保留该宏定义，并在ji_init中实现授权校验
#define ENABLE_AUTHORIZATION
// 如果需要加密模型，请保留该宏定义，并在ji_create_predictor中实现模型解密
#define ENABLE_MODEL_ENCRYPTION


/**
 * 使用predictor对输入图像inFrame进行处理
 * @param[in] predictor 句柄
 * @param[in] inFrame 输入图像
 * @param[in] args 处理当前输入图像所需要的输入参数，例如在目标检测中，通常需要输入ROI，由开发者自行定义和解析
 * @param[out] outFrame 输入图像，由内部填充结果，外部代码需要负责释放其内存空间
 * @param[out] event 以JI_EVENT封装的处理结果
 * @return 如果处理成功，返回JISDK_RET_SUCCEED
 */
int processMat(void *predictor, const cv::Mat &inFrame, const char* args, cv::Mat &outFrame, JI_EVENT &event) {
    // 校验license是否过期等
    int iRet = ji_check_expire();
    if (iRet != EV_SUCCESS) {
        return (iRet == EV_OVERMAXQPS) ? JISDK_RET_OVERMAXQPS : JISDK_RET_UNAUTHORIZED;
    }

    // 处理输入图像
    ClassifierPredictor *classifierPtr = reinterpret_cast<ClassifierPredictor *>(predictor);
    if (inFrame.empty()) {
        return JISDK_RET_FAILED;
    }
    int top = atoi(args);   // 设置需要输出结果排名的前top项
    iRet = classifierPtr->processImage(inFrame, top);
    if (iRet != ClassifierPredictor::PROCESS_OK) {
        return JISDK_RET_FAILED;    // TODO, process according to iRet
    }

    // 这里简单地将原图复制到outFrame，请根据实际需求填充真实的outFrame
    inFrame.copyTo(outFrame);
    event.code = JISDK_RET_SUCCEED;
    event.json = classifierPtr->getProcessResult();

    return JISDK_RET_SUCCEED;
}

int ji_init(int argc, char **argv) {
/* 
** 参数说明
for example:
argc：6
argv[0]: $license
argv[1]: $url
argv[2]: $activation
argv[3]: $timestamp
argv[4]: $qps
argv[5]: $version
...
**
*/
    /*
     * License
     */
    if (argc < 6) {
        return JISDK_RET_INVALIDPARAMS;
    }

    if (argv[0] == NULL || argv[5] == NULL) {
        return JISDK_RET_INVALIDPARAMS;
    }

    int qps = 0;
    if (argv[4]) qps = atoi(argv[4]);

#ifdef ENABLE_AUTHORIZATION
    // 使用公钥校验授权信息
    int ret = ji_check_license(pubKey, argv[0], argv[1], argv[2], argv[3], qps > 0 ? &qps : NULL, atoi(argv[5]));
    return ret == EV_SUCCESS ? JISDK_RET_SUCCEED : JISDK_RET_UNAUTHORIZED;
#else
    // 其他初始化工作
#endif
}

void ji_reinit() {
    ji_check_license(NULL, NULL, NULL, NULL, NULL, NULL, 0);
}


void *ji_create_predictor(int pdtype) {
    if (ji_check_expire_only() != EV_SUCCESS) {
        return NULL;
    }

    ClassifierPredictor *predictor = new ClassifierPredictor;
    char *model_struct_str = nullptr;

#ifdef ENABLE_MODEL_ENCRYPTION
    // 使用加密后的模型配置文件
    void *h = CreateEncryptor(model_str.c_str(), model_str.size(), key.c_str());

    // 获取解密后的字符串
    int fileLen = 0;
    model_struct_str = (char *) FetchBuffer(h, fileLen);
    char *tmp = new char[fileLen + 1];
    strncpy(tmp, model_struct_str, fileLen);
    tmp[fileLen] = '\0';
    model_struct_str = tmp;
    LOG(INFO) << "Buffer len:" << fileLen;
    LOG(INFO) << "FetchBuffer:" << model_struct_str;

    // 获取解密后的文件句柄
    // file *file = (file *) FetchFile(h);

    DestroyEncrtptor(h);
#else
    // 不使用模型加密功能，直接从模型文件读取
    std::ifstream ifs = std::ifstream("/usr/local/ev_sdk/sample-classifier/model/tiny.cfg", std::ios::binary);
    ifs.seekg(0, std::ios_base::end);
    long len = ifs.tellg();
    ifs.seekg(0, std::ios_base::beg);
    model_struct_str = new char[len + 1];
    ifs.get(model_struct_str, len);
    model_struct_str[len] = '\0';
#endif

    int iRet = predictor->init("/usr/local/ev_sdk//sample-classifier/model/config/imagenet1k.data", model_struct_str,
            "/usr/local/ev_sdk//sample-classifier/model/tiny.weights");
    if (iRet != ClassifierPredictor::INIT_OK) {
        return nullptr;
    }

    return predictor;
}

void ji_destroy_predictor(void *predictor) {
    if (predictor == NULL) return;

    ClassifierPredictor *classifierPredictor = reinterpret_cast<ClassifierPredictor *>(predictor);
    classifierPredictor->unInit();
    delete classifierPredictor;
}

int ji_calc_frame(void *predictor, const JI_CV_FRAME *inFrame, const char *args,
        JI_CV_FRAME *outframe, JI_EVENT *event) {
    if (predictor == NULL || inFrame == NULL) {
        return JISDK_RET_INVALIDPARAMS;
    }

    ClassifierPredictor *classifierPtr = reinterpret_cast<ClassifierPredictor *>(predictor);
    cv::Mat inMat(inFrame->rows, inFrame->cols, inFrame->type, inFrame->data, inFrame->step);
    if (inMat.empty()) {
        return JISDK_RET_FAILED;
    }
    cv::Mat outMat;
    int iRet = processMat(predictor, inMat, args, outMat, *event);

    if (iRet == JISDK_RET_SUCCEED) {
        if ((event->code != JISDK_CODE_FAILED) && (!outMat.empty()) && (outframe)) {
            outframe->rows = outMat.rows;
            outframe->cols = outMat.cols;
            outframe->type = outMat.type();
            outframe->data = outMat.data;
            outframe->step = outMat.step;
        }
    }
    return iRet;
}

int ji_calc_buffer(void *predictor, const void *buffer, int length, const char *args, const char *outFile,
                   JI_EVENT *event) {
    if (predictor == NULL || buffer == NULL || length <= 0) {
        return JISDK_RET_INVALIDPARAMS;
    }

    ClassifierPredictor *classifierPtr = reinterpret_cast<ClassifierPredictor *>(predictor);

    const unsigned char *b = (const unsigned char *) buffer;
    std::vector<unsigned char> vecBuffer(b, b + length);
    cv::Mat inMat = cv::imdecode(vecBuffer, cv::IMREAD_COLOR);
    if (inMat.empty()) {
        return JISDK_RET_FAILED;
    }

    cv::Mat outMat;
    int iRet = processMat(predictor, inMat, args, outMat, *event);

    if (iRet == JISDK_RET_SUCCEED) {
        if ((event->code != JISDK_CODE_FAILED) && (!outMat.empty()) && (outFile)) {
            cv::imwrite(outFile,outMat);
        }
    }
    return iRet;
}

int ji_calc_file(void *predictor, const char *inFile, const char *args, const char *outFile, JI_EVENT *event) {
    if (predictor == NULL || inFile == NULL) {
        return JISDK_RET_INVALIDPARAMS;
    }

    ClassifierPredictor *classifierPtr = reinterpret_cast<ClassifierPredictor *>(predictor);
    cv::Mat inMat = cv::imread(inFile);
    if (inMat.empty()) {
        return JISDK_RET_FAILED;
    }

    cv::Mat outMat;
    int iRet = processMat(predictor, inMat, args, outMat, *event);
    if (iRet == JISDK_RET_SUCCEED) {
        if ((event->code != JISDK_CODE_FAILED) && (!outMat.empty()) && (outFile)) {
            cv::imwrite(outFile, outMat);
        }
    }

    return iRet;
}

int ji_calc_video_file(void *predictor, const char *infile, const char* args,
                       const char *outfile, const char *jsonfile) {
    return JISDK_RET_SUCCEED;
}