- 去除license
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
- 使用license
1) uncomment above

