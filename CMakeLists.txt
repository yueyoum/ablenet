cmake_minimum_required(VERSION 3.2)
project(ablenet)

include_directories(/usr/include/python2.7 /usr/include/x86_64-linux-gnu/python2.7 /opt/boost/include)
link_directories(/usr/lib/python2.7/config-x86_64-linux-gnu /usr/lib /opt/boost/lib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-strict-aliasing -D_FORTIFY_SOURCE=2 -fstack-protector-strong -Wformat -Werror=format-security  -DNDEBUG -fwrapv -O2 -Wall -Wstrict-prototypes")
set(CMAKE_EXE_LINKER_FLAGS " -Xlinker -export-dynamic -Wl,-O1 -Wl,-Bsymbolic-functions")

aux_source_directory(. DIR_SRCS)
add_executable(ablenet ${DIR_SRCS})

target_link_libraries(ablenet python2.7 pthread dl  util m boost_system boost_coroutine)
