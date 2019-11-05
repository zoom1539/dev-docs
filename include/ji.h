#ifndef __JI_H__
#define __JI_H__

#ifdef __cplusplus
extern "C"
{
#endif

// 版本号,基于license:v10
#define EV_SDK_VERSION  "version:v2.8"

// 函数返回值定义
#define JISDK_RET_SUCCEED               (0)             // 成功
#define JISDK_RET_FAILED                (-1)            // 失败
#define JISDK_RET_UNUSED                (-2)            // 未实现
#define JISDK_RET_INVALIDPARAMS         (-3)            // 参数错误
#define JISDK_RET_OVERMAXQPS            (-99)           // 超过最大请求量
#define JISDK_RET_UNAUTHORIZED          (-999)          // 未授权

/**
 * 检测器类型值定义
 *
 * 当封装的算法为检测类算法时，根据需要，可能需要创建带状态的检测器，为此定义以下宏
 * 1. 如果算法仅支持不带状态的检测器, 传任何参数均创建不带状态的检测器;
 * 2. 如果算法仅支持带状态的检测器, 传任何参数均创建带状态的检测器;
 * 3. 如果算法同时支持不带或带状态的检测器时:
 *    JISDK_PREDICTOR_DEFAULT:       开发者自行决定创建;
 *    JISDK_PREDICTOR_SEQUENTIAL:    创建带状态的检测器;
 *    JISDK_PREDICTOR_NONSEQUENTIAL: 创建不带状态的检测器;
 */
#define JISDK_PREDICTOR_DEFAULT         (0)             // 默认
#define JISDK_PREDICTOR_SEQUENTIAL      (1)             // 连续的,即带状态的
#define JISDK_PREDICTOR_NONSEQUENTIAL   (2)             // 非连续的,即不带状态的

// 分析输出code值定义
#define JISDK_CODE_ALARM                (0)             // 报警
#define JISDK_CODE_NORMAL               (1)             // 正常
#define JISDK_CODE_FAILED               (-1)            // 失败


// 单帧信息，参考cv::Mat
typedef struct {
	int rows;           // cv::Mat::rows
	int cols;           // cv::Mat::cols
	int type;           // cv::Mat::type()
	void *data;         // cv::Mat::data
	int step;           // cv::Mat::step
} JI_CV_FRAME;

// 分析输出信息
typedef struct {
	int code;           // 详见"分析输出code值定义"
	const char *json;   // 算法输出结果,json格式
} JI_EVENT;

/**
 * sdk初始化,可在函数接入license授权功能,glog,log4cpp等,参数格式由开发者自行定义
 *
 * @param[in] argc 参数数量
 * @param[in] argv 参数数组
 * @return 详见"函数返回值定义",成功返回JISDK_RET_SUCCEED,其它表示失败
 */
int ji_init(int argc, char **argv);

/**
 * sdk反初始化函数,主要用于联网license校验线程终止等
 */
void ji_reinit();

/**
 * 创建检测器,即每个检测实例的上下文
 *
 * @param[in] pdtype [必选]检测器实例类型,详见"检测器类型值定义"
 * @return 成功返回检测器实例指针,错误返回NULL, 注意：授权未通过或超过有效期时，也返回NULL
 */
void* ji_create_predictor(int pdtype);

/**
 * 释放检测器实例
 *
 * @param[in] predictor [必选] 检测器实例
 */
void ji_destroy_predictor(void *predictor);

/**
 * 算法检测针对一帧
 *
 * @param[in] predictor [必选] 不允许为NULL,检测器实例; 有ji_create_predictor函数生成.
 * @param[in] inFrame   [必选] 不允许为NULL,输入源帧.
 * @param[in] args      [必选] 允许为NULL,检测参数; 由开发者自行定义，例如:roi.
 * @param[out] outFrame [必选] 允许为NULL,输出结果帧; outFrame->data由开发者创建和释放.
 * @param[out] event    [必选] 允许为NULL,输出结果; event->json由开发者创建和释放.
 * @return 成功返回JISDK_RET_SUCCEED,其它表示失败,详见"函数返回值定义"
 */
int ji_calc_frame(void *predictor, const JI_CV_FRAME *inFrame, const char *args,
                  JI_CV_FRAME *outFrame, JI_EVENT *event);

/**
 * 算法检测针对图片缓冲
 *
 * @param[in] predictor[in] [必选] 不允许为NULL,检测器实例;由ji_create_predictor函数生成.
 * @param[in] buffer[in]    [必选] 不允许为NULL,源图片缓冲地址.
 * @param[in] length[in]    [必选] 必须大于0,源图片缓冲大小.
 * @param[in] args[in]      [必选] 允许为NULL,检测参数;由开发者自行定义，例如:roi.
 * @param[out] outFile[in]  [必选] 允许为NULL,输出结果图片文件路径.
 * @param[out] event[out]   [必选] 允许为NULL,输出结果;event->json由开发者创建和释放.
 * @return 成功返回JISDK_RET_SUCCEED,其它表示失败,详见"函数返回值定义"
 */
int ji_calc_buffer(void *predictor, const void *buffer, int length,
                   const char *args, const char *outFile, JI_EVENT *event);

/**
 * 算法检测针对图片文件
 *
 * @param[in] predictor[in] [必选] 不允许为NULL,检测器实例;有ji_create_predictor函数生成.
 * @param[in] inFile        [必选] 不允许为NULL,源图片文件路径.
 * @param[in] args[in]      [必选] 允许为NULL,检测参数;由开发者自行定义，例如:roi.
 * @param[out] outFile[in]  [必选] 允许为NULL,输出结果图片文件路径.
 * @param[out] event[out]   [必选] 允许为NULL,输出结果;event->json由开发者创建和释放.
 * @return 成功返回JISDK_RET_SUCCEED,其它表示失败,详见"函数返回值定义"
 */
int ji_calc_file(void *predictor, const char *inFile, const char *args, const char *outFile, JI_EVENT *event);

/**
 * 算法检测针对视频文件
 *
 * @param[in] predictor
 * @param[in] inFile
 * @param[in] args
 * @param[out] outFile
 * @param[out] jsonFile
 * @return 成功返回JISDK_RET_SUCCEED,其它表示失败,详见"函数返回值定义"
 */
int ji_calc_video_file(void *predictor, const char *inFile, const char* args,
                       const char *outFile, const char *jsonFile);
		
#ifdef __cplusplus
}
#endif

#endif
