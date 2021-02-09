CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

ENABLE_LANGUAGE(CXX)

PROJECT(sfmllab CXX)

#FIND_PACKAGE(SFML 2.0 REQUIRED system window graphics)
find_package(xtl REQUIRED)
find_package(xtensor REQUIRED)
#find_package(ROOT REQUIRED)


INCLUDE_DIRECTORIES(
  ${SFML_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  #${ROOT_INCLUDE_DIR}
)

#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
#ROOT_GENERATE_DICTIONARY(G__Event Event.h LINKDEF EventLinkDef.h)


FOREACH(MODULE src include)
  FILE(GLOB MODULES_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/${MODULE}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/${MODULE}/*.h)
  SET(SOURCES ${MODULES_SOURCES} ${SOURCES})
ENDFOREACH(MODULE)

ADD_EXECUTABLE(${PROJECT_NAME} main.cpp ${SOURCES} ${VERSION_FILE})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}  sfml-window sfml-system sfml-graphics sfml-audio xtensor xtensor::optimize xtensor::use_xsimd)
  #${SFML_LIBRARIES})


# debug config  
SET(CMAKE_BUILD_TYPE "Debug")  
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb `root-config --cflags --glibs` -lfftw3f")  
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall -g2 -ggdb `root-config --cflags --glibs` -lfftw3f")  
