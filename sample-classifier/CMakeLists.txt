find_library(darknet1 NAMES darknet HINTS ${CMAKE_CURRENT_SOURCE_DIR}/darknet/lib)

add_executable(classifier ${CMAKE_CURRENT_SOURCE_DIR}/src/ClassifierPredictor.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
target_include_directories(classifier
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/darknet/include)
target_link_libraries(classifier
        ${darknet1}
        pthread
)

install(TARGETS classifier DESTINATION bin)