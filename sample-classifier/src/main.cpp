#include <darknet.h>
#include <ClassifierPredictor.hpp>
#include <fstream>
#include <opencv2/imgcodecs.hpp>

/**
 * 测试ClassifierPredictor
 */

int main(int argc, const char *argv[]) {
    ClassifierPredictor *predictor = new ClassifierPredictor();

    // Read model configuration file
    std::ifstream ifs = std::ifstream("/root/ev_sdk/sample-classifier/model/tiny.cfg", std::ios::binary);
    ifs.seekg(0, std::ios_base::end);
    long len = ifs.tellg();
    char *model_str = new char[len + 1];
    ifs.seekg(0, std::ios_base::beg);
    ifs.read(model_str, len);
    ifs.seekg(0);
    model_str[len] = '\0';

    // Init
    predictor->init("/root/ev_sdk/sample-classifier/model/config/imagenet1k.data", model_str,
                    "/root/ev_sdk/sample-classifier/model/tiny.weights");
    // Process
    predictor->processImage(cv::imread("/root/ev_sdk/sample-classifier/data/sample.jpg"), 5);
    printf("result: %s", predictor->getProcessResult());
}