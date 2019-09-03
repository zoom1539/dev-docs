#include <darknet.h>

void predict_classifier(char *datacfg, char *cfgfile, char *weightfile, char *filename, int top) {

}

int main(int argc, const char *argv[]) {
    predict_classifier("/root/ev_sdk/sample-classifier/model/config/imagenet1k.data", "/root/ev_sdk/sample-classifier/model/tiny.cfg",
                       "/root/ev_sdk/sample-classifier/model/tiny.weights", "/root/ev_sdk/sample-classifier/data/sample.jpg", 5);
}