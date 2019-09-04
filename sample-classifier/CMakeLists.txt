list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(Darknet REQUIRED)
find_package(OpenCV REQUIRED)
find_library(glog REQUIRED)

# ClassifierPredictor
add_library(ClassifierPredictor SHARED ${CMAKE_CURRENT_SOURCE_DIR}/src/ClassifierPredictor.cpp)
target_link_libraries(ClassifierPredictor
        pthread
        ${darknet_LIBRARIES}
        ${OpenCV_LIBS}
        glog
        cjson
)
target_include_directories(ClassifierPredictor
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
        PUBLIC ${darknet_INCLUDE_DIRS}
        PRIVATE ${OpenCV_INCLUDE_DIR}
)

# test ClassifierPredictor
add_executable(classifier ${CMAKE_CURRENT_SOURCE_DIR}/src/ClassifierPredictor.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
target_link_libraries(classifier
        ClassifierPredictor
        ${OpenCV_LIBS}
        pthread
        glog
)

install(TARGETS classifier DESTINATION bin)
install(TARGETS ClassifierPredictor DESTINATION lib)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/darknet/lib/libdarknet.so DESTINATION lib)