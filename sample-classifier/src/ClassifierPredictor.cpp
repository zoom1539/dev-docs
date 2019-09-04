//
// Created by hrh on 2019-09-02.
//

#include "ClassifierPredictor.hpp"
#include <cstdlib>
#include <cstring>
#include "utils.cpp"
#include <cJSON.h>
#include <glog/logging.h>

ClassifierPredictor::ClassifierPredictor(): mNetworkPtr(NULL), mTop(5) {}

int ClassifierPredictor::init(char *datacfg, const char *model_cfg_str, char *weightfile) {
    LOG(INFO) << "Loading model...";
    mNetworkPtr = load_network_from_string(model_cfg_str, weightfile, 0);
    set_batch_network(mNetworkPtr, 1);
    srand(2222222);

    list *options = read_data_cfg(datacfg);

    mNameList = option_find_str(options, "names", 0);
    if (mNameList == nullptr) {
        return ClassifierPredictor::ERROR_INVALID_INIT_ARGS;
    }

    mNames = get_labels(mNameList);
    clock_t time;
    mIndexes = (int *)calloc(mTop, sizeof(int));

    LOG(INFO) << "Done.";
    return ClassifierPredictor::INIT_OK;
}

void ClassifierPredictor::unInit() {
    if (mIndexes) {
        free(mIndexes);
        mIndexes = nullptr;
    }
    if (mOptions) {
        free_list(mOptions);
        mOptions = nullptr;
    }
    if (mNetworkPtr) {
        free_network(mNetworkPtr);
        mNetworkPtr = nullptr;
    }
}

int ClassifierPredictor::processImage(const cv::Mat &cv_image, int top) {
    // 读入图像数据并处理
    image im = mat_to_image(cv_image);
    image r = letterbox_image(im, mNetworkPtr->w, mNetworkPtr->h);
    float *X = r.data;
    float *predictions = network_predict(mNetworkPtr, X);
    if (mNetworkPtr->hierarchy) hierarchy_predictions(predictions, mNetworkPtr->outputs, mNetworkPtr->hierarchy, 1, 1);
    top_k(predictions, mNetworkPtr->outputs, mTop, mIndexes);

    // 将结果封装成json格式
    cJSON *root = cJSON_CreateObject();
    cJSON *results = cJSON_CreateArray();
    for (int i = 0; i < mTop; ++i) {
        int index = mIndexes[i];

        cJSON *item = cJSON_CreateObject();
        cJSON *clsName = cJSON_CreateString(mNames[index]);
        cJSON *clsScore = cJSON_CreateNumber(predictions[index]*100);
        cJSON_AddItemToObject(item, "className", clsName);
        cJSON_AddItemToObject(item, "score", clsScore);
        printf("%5.2f%%: %s\n", predictions[index] * 100, mNames[index]);
        cJSON_AddItemToArray(results, item);
    }
    cJSON_AddItemToObject(root, "results", results);
    if (mProcessResult)
        delete mProcessResult;
    mProcessResult = cJSON_Print(root);

    // 释放资源
    cJSON_Delete(root);
    if (r.data != im.data) free_image(r);
    free_image(im);

    return ClassifierPredictor::PROCESS_OK;
}