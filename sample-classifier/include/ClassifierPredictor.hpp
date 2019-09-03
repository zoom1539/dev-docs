//
// Created by rawk on 2019-09-02.
//

#ifndef JI_CLASSIFIERPREDICTOR_HPP
#define JI_CLASSIFIERPREDICTOR_HPP
#include <darknet.h>
#include <string>

class ClassifierPredictor {

public:
    ClassifierPredictor();
private:
    void init(char *datacfg, char *cfg_string, char *weightfile, char *filename, int top);
    void unInit();
    void processImage(std::string filename);

    network* mNetworkPtr{nullptr};
    char *mNameList{nullptr};
    int mTop{0};
    int *mIndexes{nullptr};
    char **mNames{nullptr};
    list *mOptions{nullptr};
};


#endif //JI_CLASSIFIERPREDICTOR_HPP
