## 去除license
1) line 30 in ji.cpp
```
// #define ENABLE_JI_AUTHORIZATION 
```
2) line 38 in CMakeLists.txt
```
# ${JI_LICENSE_LDFLAGS}
```
3) in test.cpp
```
line 826-830
// if (!read_license(strLicense,l,u,a,t,qps,v))
// {
//     LOG(ERROR) << "[ERROR] read_license faild.";
//     return -1;
// }

line 704 
// ji_get_license_version(&license_version);
```
## 使用license
1) uncomment above
## 主要修改的地方
- ji.cpp
```
1. ji_create_predictor()
2. ji_destroy_predictor()
3. processMat()
```
- Configuration.hpp
```
1. typedef struct {
        // 算法配置可选的配置参数
        int fire_extinguisher_num;
        int frame_num;
} ALGO_CONFIG_TYPE;

2. // --------- 通常需要根据需要修改的算法配置 START ---------------
3. // 多路
```

