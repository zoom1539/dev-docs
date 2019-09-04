find_library(darknet NAMES darknet HINTS ${CMAKE_CURRENT_SOURCE_DIR}/darknet/lib)
find_package(OpenCV REQUIRED)
find_library(glog REQUIRED)

# ClassifierPredictor
add_library(ClassifierPredictor SHARED ${CMAKE_CURRENT_SOURCE_DIR}/src/ClassifierPredictor.cpp)
target_link_libraries(ClassifierPredictor
        pthread
        ${darknet}
        ${OpenCV_LIBS}
        glog
        cjson
)
target_include_directories(ClassifierPredictor
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/darknet/include
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