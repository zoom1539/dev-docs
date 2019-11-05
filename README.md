# EV_SDK

## 说明
### EV_SDK的目标
开发者专注于算法开发及优化，最小化业务层编码，即可快速部署到生产环境，共同打造商用级高质量算法。
### 极市平台做了哪些
1. 统一定义算法接口：针对万千视频和图片分析算法，抽象出接口，定义在`include`目录下的`ji.h`文件中
2. 提供工具包：比如算法授权（必须实现）、模型加密，在`3rd`目录下
3. 应用层服务：此模块不在ev_sdk中，比如视频处理服务、算法对外通讯的http服务等

### 开发者需要做什么
1. 模型的训练和调优
2. 实现`ji.h`约定的接口，同时包括授权、支持分析区域等功能
3. 实现约定的输入输出
4. 其他后面文档提到的功能

## 目录

### 代码目录结构

```
ev_sdk
|-- 3rd     # 第三方源码或库目录，包括boost for wkt(roi),cJSON,license,enctry model，发布时需移除
|   |-- boost_interface # boost for wkt(roi)
|   |-- cJSON               # c版json库,简单易用
|   |-- encrypt_module  # 模型加密库及相关工具
|   `-- license             # SDK授权库及相关工具
|-- CMakeLists.txt          # 本项目的cmake配置文件
|-- README.md
|-- cmake                   # 本项目所依赖的cmake模块
|   |-- FindEncryptModule.cmake
|   `-- FindJILicense.cmake
|-- doc
|-- include         # 库头文件目录
|   `-- ji.h        # libji.so的头文件，理论上仅有唯一一个头文件
|-- lib             # 本项目编译并安装之后，默认会将依赖的库放在该目录，包括libji.so
|-- sample-classifier       # 示例程序，使用darknet实现的图像分类器，实现了EV_SDK的接口规范
|-- src
`-- test            # EV_SDK接口测试程序的代码
```
## 使用示例
作为示例，我们提供了一个使用`darknet`实现的图像分类器，并将其使用`EV_SDK`规范进行封装。使用如下步骤尝试编译和测试该图像分类器：

#### 下载`EV_SDK`

```shell
git clone https://github.com/ExtremeMart/dev-docs
mv dev-docs /usr/local/ev_sdk
```

#### 编译

`sample-classifier`已经生成公钥和私钥，并把私钥放置在`/usr/local/ev_sdk/sample-classifier/generated-data/privateKey.pem`，公钥已经转换成头文件`/usr/loca/ev_sdk/include/pubKey.hpp`，加密后的模型文件也已经转换成头文件`/usr/local/ev_sdk/include/model_str.hpp`。

编译和安装`libji.so`：

```shell
mkdir -p /usr/local/ev_sdk/build
cd /usr/local/ev_sdk/build
cmake ..
make install
```

#### 测试示例程序和接口规范

执行完成之后，`/usr/local/ev_sdk/lib`下将生成`libji.so`和相关的依赖库，以及`/usr/local/ev_sdk/bin/`下的测试程序。

1. 要使用`/usr/local/ev_sdk/test-ji-api`测试`EV_SDK`的接口，需要重新生成参考码`reference.txt`，并使用私钥对其进行加密后重新生成授权文件`license.txt`

   ```shell
   cd /usr/local/ev_sdk
   ./3rd/license/v10/tools/ev_license -r reference.txt
   ./3rd/license/v10/tools/ev_license -l sample-classifier/generated-data/privateKey.pem reference.txt license.txt
   ```

2. 使用`test-ji-api`测试`EV_SDK`接口

   ```shell
   ./bin/test-ji-api -f 3 -l ./license.txt -a 3 -i ./data/test_sub.jpg
   ```

   输出内容样例：

   ```shell
   I0905 17:25:43.897959 10201 test.cpp:327] call ji_calc_file, return 0
   I0905 17:25:43.897985 10201 test.cpp:331] event info:
           code: 0
           json: {
           "results":   [{
                           "className":    "desk",
                           "score":        67.176292
                   }, {
                           "className":    "desktop computer",
                           "score":        11.968087
                   }, {
                           "className":    "monitor",
                           "score":        2.054801
                   }]
                }
   ```

### 使用`EV_SDK`快速封装算法

以下示例如何将开发的算法以`EV_SDK`规范进行封装

#### 实现自己的模型

假设我们使用`darknet`开发了一个图像分类算法，其包含模型文件`classifier-model.cfg`。

#### 下载`EV_SDK`

```shell
git clone https://github.com/ExtremeMart/dev-docs
mv dev-docs /usr/local/ev_sdk
```

#### 添加授权功能

1. 使用`EV_SDK`提供的工具生成公钥和私钥

   ```shell
   mkdir -p /usr/local/ev_sdk/generated-data/
   cd /usr/local/ev_sdk/generated-data/
   /usr/local/3rd/license/v10/tools/generateRsaKey.sh
   ```

   执行成功后将生成公钥`generated-data/pubKey.perm`和私钥`generated-data/privateKey.pem`。

2. 将公钥转换成`C++`头文件

   ```shell
   /usr/local/ev_sdk/3rd/license/v10/tools/ev_codec -c generated-data/pubKey.perm generated-data/pubKey.hpp
   # 将头文件移动到代码处
   mv /usr/local/ev_sdk/generated-data/pubKey.hpp /usr/local/ev_sdk/include
   ```

   这个包含公钥的头文件将被**硬编码**到`libji.so`。

3. 在`ji_init(int argc, char **argv)`的接口实现中，添加校验授权文件的功能

   ```c++
   // 使用公钥校验授权信息
   int ret = ji_check_license(pubKey, license, url, activation, timestamp, qps, version);
   return ret == EV_SUCCESS ? JISDK_RET_SUCCEED : JISDK_RET_UNAUTHORIZED;
   ```

> 更多授权功能的原理，请参考[算法授权](doc/Authorization.md)。

#### 添加模型加密与解密功能

1. 使用`EV_SDK`提供的工具加密模型，并生成`C++`头文件

   ```shell
   cd /usr/local/ev_sdk/generated-data/
   /usr/local/3rd/encrypt_module/encrypt_model classifier-model.cfg
   ```

   执行成功后会生成加密后的文件`model_str.hpp`，`encrypt_model`程序支持在加密模型时指定一个混淆字符串，具体方法请使用`encrypt_model -h`参考帮助文档。将头文件移动到代码区

   ```shell
   mv /usr/local/ev_sdk/generated-data/model_str.hpp /usr/local/ev_sdk/include
   ```

   这个加密后的模型将被**硬编码**到`libji.so`。

2. 在`ji_create_predictor(int)`的接口实现中，添加模型解密的功能

   ```c++
   // 创建解密句柄
   void *h = CreateEncryptor(model_str.c_str(), model_str.size(), key.c_str());
   // 获取解密后的字符串
   int fileLen = 0;
   model_struct_str = (char *) FetchBuffer(h, fileLen);
   // 获取解密后的文件句柄
   // file *file = (file *) FetchFile(h);
   DestroyEncrtptor(h);
   ```

   模型解密的详细接口函数请参考其头文件[encrypt_wrapper.h](3rd/ev_encrypt_module/include/encrypt_wrapper.hpp)

#### 实现`ji.h`中的接口

`ji.h`中定义了所有`EV_SDK`规范的接口，详细的接口定义和实现示例，请参考头文件[ji.h](include/ji.h)和示例代码[ji.cpp](src/ji.cpp)。

6. 将代码编译成`libji.so`

   ```shell
   mkdir -p /usr/local/ev_sdk/build
   cd /usr/local/ev_sdk/build
   cmake ..
   make install
   ```

   编译完成后，将在`/usr/local/ev_sdk/lib`下生成`libji.so`和其他依赖的库。

#### 测试接口规范

测试`libji.so`的授权功能是否正常工作以及`ji.h`的接口规范

1. 生成授权文件

   1. 使用`EV_SDK`提供的工具生成与特定主机关联的参考码

      ```shell
      /usr/local/ev_sdk/3rd/license/v10/tools/ev_license -r generated-data/reference.txt
      ```

   2. 使用私钥加密参考码，并生成授权文件`license.txt`

      ```shell
      /usr/local/ev_sdk/3rd/license/v10/tools/ev_license -l generated-data/privateKey.pem reference.txt license.txt
      ```

2. 检查授权功能和`ji.h`的接口规范性

   `EV_SDK`代码中提供了测试所有接口的测试程序，编译安装`libji.so`之后，会在`/usr/local/ev_sdk/bin`下生成`test-ji-api`可执行文件，`test-ji-api`用于测试`ji.h`的接口实现是否正常，例如，测试`ji_calc_file(void *, const char *, const char *, const char *, JI_EVENT *)`接口以及授权功能是否正常：

   ```shell
   /usr/local/ev_sdk/bin/test-ji-api -f 3 -l generated-data/license.txt -i data/in.jpg -o out.jpg
   ```

   接口测试程序的详细功能请查阅`test-ji-api --help`的帮助文档及其代码[test.cpp](test/src/test.cpp)

> 将算法封装成`EV_SDK`接口的`libji.so`之后，请把`/usr/local/ev_sdk/model_str.hpp`和`/usr/local/ev_sdk/pubKey.hpp`删除，并保存私钥`privateKey.pem`。这样就可以使用`privateKey.pem`对所开发算法进行授权。

### 如何将SDK发布到极市开发者平台

1. 2.0版本[极市平台](http://cvmart.net)流程：
   - 极市运营同学提供GPU资源；
   - 加载对应数据集，并创建虚拟机，如果没有对应数据集，需自己上传；
   - 在线训练；
   - 提交模型自动测试；
   - 测试完成后，按流程提交模型测试：
     模型测试 -> 新建测试->刷新页面查看测试状态 ->模型测试结果通知后查看测试详情
   - 测试通过后，按如下流程提交sdk:
     生成服务 -> 专家模式 -> 填写表单 -> 点击提交
     ->刷新页面查看SDK生成状态 
     ->查看服务生成结果
   - 显示发布成功即表示发布sdk完成！
2. 1.0版本算法发布流程：
   - GPU：联系极市运营同学，获取GPU资源；
   - CPU：登录[极市平台](http://cvmart.net)，点击我的算法，点击“+”，创建算法，填写相关信息，点击申请上架后，极市开始对算法进行测试审核。审核通过后算法将发布在极市算法市场。

### 哪些必须完成才能通过测试
#### 算法总共4个接口，由于需求场景不同，接口不同，项目经理会告知具体需要实现的接口
1. `ji_calc_frame` : 实时视频流分析
2. `ji_calc_buffer` : 分析图片
3. `ji_calc_file` : 分析视频文件
4. `ji_calc_video_file` ：用于极市平台测试组测试和开发者自测视频文件

##### 注意：由于算法成千上万，要保证高效算法，不仅需要实现相关功能，也要保证基本文件路径、文件夹、文件名称一致

####规范要求

1. 基础要求
   * 路径：`/usr/local/ev_sdk/test` 文件：`Makefile`，`test.cpp`       请勿修改`Makefile`，`test.cpp`，并在提交版本的时候，需要使用极市平台提供的`Makefile`，`test.cpp`，确定算法能够编译成功且算法程序能正常运行
   
   * 路径：`/usr/local/ev_sdk/bin`  文件：`test`     test文件和license.txt复制到任何目录下都能正常运行。编码需要使用全路径
   
   * 需要实现授权，若授权失败所有接口返回是`-999`
   
   * 整个SDK算法框架都需要按照极视角提供的算法框架模板编码，请勿修改提供的文件夹名称或者文件名称，也勿修改文件夹路径或者文件路径
   
   * 针对需要实现的报警算法：
   
   * 针对需要实现的报警算法：
     * 报警输出：`JI_EVENT.code=0(JISDK_CODE_ALARM)`,`JI_EVENT.json`内部`"alert_flag"=1`；
     * 未报警输出：`JI_EVENT.code=1(JISDK_CODE_NORMAL)`,`JI_EVENT.json`内部`"alert_flag"=0`；
     * 失败的接口返回`JI_EVENT.code=-1(JISDK_CODE_FAILED)`
   
2. 函数规范
   * 若算法实现了多个接口，需要调式不同接口运行相同数据，查看运行结果是否一致
   * 实现的接口：输入图片数据或者视频数据的分辨率要与输出数据的分辨率一致
   * 未实现的接口，如需要调用该接口，该接口需要返回`-2`
   * 实现的接口传入异常参数的时候，该接口需要返回`-3`

3. 配置规范   `/usr/local/ev_sdk/model`, 文件：`algo_config.json`
	 基本配置项：
   
	- `draw_roi_area`：绘制`roi`分析区域 
	-  ` draw_result`：绘制分析结果 
	-  `show_result`：算法运行实时显示  
	-  `gpu_id`：切换gpu设备  
	-  ` threshold`：算法阈值
	
	每个配置都需要实现，多个配置组合修改情况如下：
	 * `draw_roi_area`：1  `draw_result`：1  绘制`roi`分析区域，并且显示绘制分析结果
	* `draw_roi_area`：1  `draw_result`：0  绘制`roi`分析区域，不显示绘制分析结果
	* `draw_roi_area`：0  `draw_result`：1  不绘制`roi`分析区域，显示绘制分析结果
	* `draw_roi_area`：0  `draw_result`：0  不绘制`roi`分析区域，不显示绘制分析结果
	
	`roi`区域相关的配置
	
	* 算法只需实现分析`roi`区域内的场景，`roi`区域外就算有需要识别的场景也不需要识别
	* 算法识别的检测框需要显示正确
	* `roi`可以绘制多边形，绘制显示结果需于绘制的形状一致。例如6边型，不规则图形
	* `roi`默认传入为空，则需分析全部区域
	
4. 其他
  * 实现的算法识别的`json` 格式返回值需和项目经理提供的一致
  * 算法配置：配置路径验证`/usr/local/ev_sdk/model` 配置名称`algo_config.json`
  * 除了极市平台基本参数外，如果有其他参数提供`README.md`， 存在`threshold`：参数需要在`README.md`中添加默认值
  * 公私钥请放在`/usr/local/ev_sdk/bin` ，且勿要更新公私钥，因为我们这边只会保存第一版的公私钥


### FAQ

### `args`使用方式

在`EV_SDK`的接口（如`int ji_calc_frame(void *, const JI_CV_FRAME *, const char *args, JI_CV_FRAME *, JI_EVENT *)`，其`args`参数由开发者自行定义和解析。

在实际项目需求中，通常需要`args`支持输入ROI区域等参数，此时开发者需要实现解析这一`args`的代码。`args`有两种输入格式：
1. 使用`|`分隔：
    ```shell
    "cid=1000|POINT(0.38 0.10)|POINT(0.47 0.41)|LINESTRING(0.07 0.21,0.36 0.245,0.58 0.16,0.97 0.27)|POLYGON((0.048 0.357,0.166 0.0725,0.393 0.0075,0.392 0.202,0.242 0.375))|POLYGON((0.513 0.232,0.79 0.1075,0.928 0.102,0.953 0.64,0.759 0.89,0.51 0.245))|POLYGON((0.115 0.497,0.592 0.82,0.581 0.917,0.14 0.932))"
   ```
2. 使用`json`格式封装（推荐开发者使用这种格式）
    ```json
    {
        "cid": "1000",
        "POINT": [
            "POINT(0.38 0.10)",
            "POINT(0.47 0.41)"
        ],
        "LINESTRING": "LINESTRING(0.070.21,0.360.245,0.580.16,0.970.27)",
        "POLYGON": [
            "POLYGON((0.0480.357,0.1660.0725,0.3930.0075,0.3920.202,0.2420.375))",
            "POLYGON((0.5130.232,0.790.1075,0.9280.102,0.9530.64,0.7590.89,0.510.245))",
            "POLYGON((0.1150.497,0.5920.82,0.5810.917,0.140.932))"
        ]
    }
    ```

开发者需要将这一`args`读入并解析，例如当算法支持`ROI`输入，并且支持的`args`格式为`json`时：
```json
{
    "ROI": [
        "POLYGON((0.0480.357,0.1660.0725,0.3930.0075,0.3920.202,0.2420.375))",
        "POLYGON((0.5130.232,0.790.1075,0.9280.102,0.9530.64,0.7590.89,0.510.245))",
        "POLYGON((0.1150.497,0.5920.82,0.5810.917,0.140.932))"
    ]
}
```
那么开发者需要解析这一`args`，提取其中的ROI参数，并使用`WKTParser`对其进行解析，并应用到自己的算法使用逻辑中。