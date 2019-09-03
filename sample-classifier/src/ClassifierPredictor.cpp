//
// Created by hrh on 2019-09-02.
//

#include "ClassifierPredictor.hpp"
#include <cstdlib>
#include <cstring>

ClassifierPredictor::ClassifierPredictor(): mNetworkPtr(NULL) {}

void ClassifierPredictor::init(char *datacfg, char *cfg_string, char *weightfile, char *filename, int top) {
    mNetworkPtr = load_network(cfg_string, weightfile, 0);
    set_batch_network(mNetworkPtr, 1);
    srand(2222222);

    list *options = read_data_cfg(datacfg);

    mNameList = option_find_str(options, "names", 0);
    mTop = top;
    if (top == 0)
        mTop = option_find_int(options, "top", 1);

    mNames = get_labels(mNameList);
    clock_t time;
    mIndexes = (int *)calloc(top, sizeof(int));
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

void ClassifierPredictor::processImage(std::string filename) {
    char buff[256];
    char *input = buff;

    strncpy(input, filename, 256);
    image im = load_image_color(input, 0, 0);
    image r = letterbox_image(im, mNetworkPtr->w, mNetworkPtr->h);
    //image r = resize_min(im, 320);
    //printf("%d %d\n", r.w, r.h);
    //resize_network(net, r.w, r.h);
    //printf("%d %d\n", r.w, r.h);

    float *X = r.data;
    clock_t time = clock();
    float *predictions = network_predict(mNetworkPtr, X);
    if (mNetworkPtr->hierarchy) hierarchy_predictions(predictions, mNetworkPtr->outputs, mNetworkPtr->hierarchy, 1, 1);
    top_k(predictions, mNetworkPtr->outputs, mTop, mIndexes);
    fprintf(stderr, "%s: Predicted in %f seconds.\n", input, sec(clock() - time));
    for (int i = 0; i < mTop; ++i) {
        int index = mIndexes[i];
        printf("%5.2f%%: %s\n", predictions[index] * 100, mNames[index]);
    }
    if (r.data != im.data) free_image(r);
    free_image(im);
}