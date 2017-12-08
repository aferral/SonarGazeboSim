
cmake_minimum_required(VERSION 2.8)
project(Sim_C)



find_package( OpenCV REQUIRED )

add_library(parser parser.c)
add_library(model model.c)

add_executable(out main.c)

target_link_libraries(out parser)
target_link_libraries(out model)
target_link_libraries(out ${OpenCV_LIBS} )
