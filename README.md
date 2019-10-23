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

   模型解密的详细接口函数请参考其头文件[encrypt_wrapper.h](3rd/encrypt_module/include/encrypt_wrapper.hpp)

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


### FAQ
