cmake_minimum_required (VERSION 3.15)
project(Car_Info VERSION 0.1 LANGUAGES CXX)

### PACKAGES ###
find_package(Python3 COMPONENTS Development)
find_package(Boost REQUIRED COMPONENTS python310)
find_package(CommonAPI REQUIRED)
find_package(CommonAPI-SomeIP REQUIRED)

set(APP_PACKAGE_NAME CarInfo)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++11")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

### VARIABLES ###
set(PRJ_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(PRJ_SRC_GEN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src-gen)
set(CORE_GEN_SRC_DIR ${PRJ_SRC_GEN_DIR}/core)
set(SOMEIP_GEN_SRC_DIR ${PRJ_SRC_GEN_DIR}/someip)

### INCLUDE DIRECTORIES ###
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CORE_GEN_SRC_DIR}
    ${SOMEIP_GEN_SRC_DIR}
    ${Python3_INCLUDE_DIRS}
    ${COMMONAPI_INCLUDE_DIRS}
    ${COMMONAPI_SOMEIP_INCLUDE_DIRS}
)

### LINK DIRECTORIES ###
link_directories(
    ${COMMONAPI_LIBDIR}
    ${Boost_LIBRARY_DIR}
)

### SOURCES ###
file(GLOB PRJ_SOURCES "${PRJ_SRC_DIR}/*.cpp")
file(GLOB PRJ_SOMEIP_LIB_SRCS "${PRJ_SRC_GEN_DIR}/someip/v0/commonapi/*.cpp")

### BUILD TARGETS ###
# Services
add_executable(CarInfoService ${PRJ_SOURCES})
target_link_libraries(CarInfoService CommonAPI CommonAPI-SomeIP Python3::Python Boost::python)

# Client 
add_executable(CarInfoClient ${PRJ_SRC_DIR}/CarInfoClient.cpp)
target_link_libraries(CarInfoClient CommonAPI CommonAPI-SomeIP)

add_library(CarInfoService-someip SHARED ${PRJ_SOMEIP_LIB_SRCS})
target_link_libraries(CarInfoService-someip CommonAPI CommonAPI-SomeIP)

### INSTALLATION PATHS ###
# .so files installation
install(TARGETS CarInfoService-someip
        LIBRARY DESTINATION /usr/local/lib)

# Executable binaries installation
install(TARGETS CarInfoService CarInfoClient
        RUNTIME DESTINATION /usr/bin)

# .cmake files installation
install(FILES
    "${PROJECT_BINARY_DIR}/${APP_PACKAGE_NAME}Config.cmake"
    "${PROJECT_BINARY_DIR}/${APP_PACKAGE_NAME}ConfigVersion.cmake"
    DESTINATION /usr/local/lib/cmake)

# Proxy headers installation
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
        DESTINATION /usr/local/include/${APP_PACKAGE_NAME}/${PROJECT_VERSION}/${APP_PACKAGE_NAME}
        FILES_MATCHING PATTERN "*.hpp")
