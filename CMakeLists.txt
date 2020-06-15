cmake_minimum_required(VERSION 3.10)

# set the project name
project(test_quicktle)

set ( PROJECT_LINK_LIBS libquicktle.so )

link_directories(/usr/lib)

include_directories(/usr/include)


# add the executable
add_executable(test_quicktle src/test_quicktle.cpp )
target_link_libraries(test_quicktle libquicktle.so)

add_Executable(genConstellation src/genConstellation.cpp)
target_link_libraries(genConstellation libquicktle.so)

add_Executable(genTle src/genTle.cpp)
target_link_libraries(genTle libquicktle.so)

