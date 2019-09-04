//
// Created by hrh on 2019-09-02.
//

#ifndef JI_CLASSIFIERPREDICTOR_HPP
#define JI_CLASSIFIERPREDICTOR_HPP
#include <darknet.h>
#include <string>
#include <opencv2/core/mat.hpp>

/**
 * 使用darknet实现的目标分类器，分类类别使用imagenet1k数据
 */

class ClassifierPredictor {

public:
    ClassifierPredictor();

    /**
     * 初始化模型
     * @param[in] datacfg imagenet1k的配置信息
     * @param[in] model_cfg_str 使用字符串表示的darknet模型
     * @param[in] weightfile darknet模型的权重文件路径
     * @return 如果初始化正常，返回JISDK_
     */
    int init(char *datacfg, const char *model_cfg_str, char *weightfile);

    /**
     * 反初始化函数
     */
    void unInit();

    /**
     * 对cv::Mat格式的图片进行分类，并输出预测分数前top排名的目标名称到mProcessResult
     * @param[in] image 输入图片
     * @param[in] top 输出排名分数前top
     * @return 如果正常入理，则返回PROCESS_OK，否则返回`ERROR_*`出错新
     */
    int processImage(const cv::Mat &image, int top);

    /**
     * 获取上一次调用processImage之后生成的结果
     * @return 按照json格式存储的字符串
     */
    inline char* getProcessResult() {
        return mProcessResult;
    }

    static const int ERROR_BASE = 0x0100;
    static const int ERROR_INVALID_INPUT = 0x0101;
    static const int ERROR_INVALID_INIT_ARGS = 0x0102;

    static const int PROCESS_OK = 0x1001;
    static const int INIT_OK = 0x1002;

private:
    network* mNetworkPtr{nullptr};

    char *mNameList{nullptr};
    int mTop{5};
    int *mIndexes{nullptr};
    char **mNames{nullptr};
    list *mOptions{nullptr};

    char *mProcessResult;
};


#endif //JI_CLASSIFIERPREDICTOR_HPP
