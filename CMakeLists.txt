### SETUP ###
cmake_minimum_required (VERSION 3.15)
project(Car_Info VERSION 0.1 LANGUAGES CXX)
### PACKAGES ###
find_package(Python3 COMPONENTS Development)
find_package(Boost REQUIRED COMPONENTS python310)
find_package(CommonAPI REQUIRED)
find_package(CommonAPI-SomeIP REQUIRED)

set(APP_PACKAGE_NAME CarInfo) 
set(MAJOR_VERSION 0)
set(MINOR_VERSION 1)

### VARIABLES ###

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++11")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PRJ_SRC_VAR src)
set(PRJ_SRC_GEN_VAR src-gen)
set(PRJ_SRC_GEN_PATH ${CMAKE_CURRENT_SOURCE_DIR}/car_info/${PRJ_SRC_GEN_VAR})
set(CORE_GEN_SRC_DIR ${PRJ_SRC_GEN_PATH}/core)
set(SOMEIP_GEN_SRC_DIR ${PRJ_SRC_GEN_PATH}/someip)
set(CORE_RUNTIME_DIR ~/capicxx-core-runtime)
set(SOMEIP_RUNTIME_DIR ~/capicxx-someip-runtime)
set(VSOMEIP_DIR ~/vsomeip)
set(COMMONAPI_INTERFACE_VER v0/commonapi)

FILE(GLOB PRJ_SOMEIP_LIB_SRCS car_info/src-gen/someip/v0/commonapi/*.cpp)

### DIRECTORIES ###
include_directories(
	car_info/include
    car_info/src-gen/core
    car_info/src-gen/someip
	${Python3_INCLUDE_DIRS}
    ${COMMONAPI_INCLUDE_DIRS}
    ${COMMONAPI_SOMEIP_INCLUDE_DIRS}
)

link_directories(
    ${COMMONAPI_LIBDIR}
    ${COMMONAPI_SOMEIP_CMAKE_DIR}/build
    ${Boost_LIBRARY_DIR}
    ${SOMEIP_RUNTIME_DIR}/build
    ${CORE_RUNTIME_DIR}/build
    ${VSOMEIP_DIR}/build
)

### BUILD ###
# Services
add_executable(CarInfoService 
    ${PRJ_SRC_VAR}/PiRacer.cpp
    ${PRJ_SRC_VAR}/CarInfoService.cpp
    ${PRJ_SRC_VAR}/CarInfoStubImpl.cpp
)
target_link_libraries(CarInfoService 
    CommonAPI 
    CommonAPI-SomeIP 
    vsomeip3
    Python3::Python 
    Boost::python
)
# Client 
add_executable(CarInfoClient
    ${PRJ_SRC_VAR}/CarInfoClient.cpp
    ${SOMEIP_GEN_SRC_DIR}/${COMMONAPI_INTERFACE_VER}/CarInfoSomeIPProxy.cpp
    ${SOMEIP_GEN_SRC_DIR}/${COMMONAPI_INTERFACE_VER}/CarInfoSomeIPDeployment.cpp
)
target_link_libraries(CarInfoClient 
    CommonAPI 
    CommonAPI-SomeIP 
    vsomeip3
)

add_library(CarInfoService-someip
    SHARED ${PRJ_SOMEIP_LIB_SRCS}
)
target_link_libraries(CarInfoService-someip
    CommonAPI
    CommonAPI-SomeIP
)

### INSTALL DIRECTORIES SETUP ###
set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for binaries")
set(INSTALL_INCLUDE_DIR include/${APP_PACKAGE_NAME} CACHE PATH "Installation directory for header files")
set(DEF_INSTALL_CMAKE_DIR lib/cmake/${APP_PACKAGE_NAME})
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH "Installation directory for CMake files")

### INSTALL ###
# Executables
install(TARGETS CarInfoService CarInfoClient
        RUNTIME DESTINATION ${INSTALL_BIN_DIR}
        ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
        LIBRARY DESTINATION ${INSTALL_LIB_DIR})

# Library
install(TARGETS CarInfoService-someip
        LIBRARY DESTINATION ${INSTALL_LIB_DIR}
        ARCHIVE DESTINATION ${INSTALL_LIB_DIR})

# Headers
file(GLOB_RECURSE HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/car_info/include/*.hpp")
foreach(HEADER ${HEADER_FILES})
    get_filename_component(HEADER_ABS_PATH ${HEADER} ABSOLUTE)
    file(RELATIVE_PATH HEADER_REL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/car_info/include" ${HEADER_ABS_PATH})
    get_filename_component(HEADER_DIR ${HEADER_REL_PATH} DIRECTORY)
    install(FILES ${HEADER_ABS_PATH} DESTINATION ${INSTALL_INCLUDE_DIR}/${HEADER_DIR})
endforeach()

# Export targets
export(TARGETS CarInfoService CarInfoClient CarInfoService-someip
  FILE "${PROJECT_BINARY_DIR}/${APP_PACKAGE_NAME}Targets.cmake")

# Package configurations
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${PROJECT_BINARY_DIR}/${APP_PACKAGE_NAME}ConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/${APP_PACKAGE_NAME}Config.cmake.in
    "${PROJECT_BINARY_DIR}/${APP_PACKAGE_NAME}Config.cmake" @ONLY)

install(FILES
  "${PROJECT_BINARY_DIR}/${APP_PACKAGE_NAME}Config.cmake"
  "${PROJECT_BINARY_DIR}/${APP_PACKAGE_NAME}ConfigVersion.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}"
)

install(EXPORT ${APP_PACKAGE_NAME}Targets
  FILE ${APP_PACKAGE_NAME}Targets.cmake
  DESTINATION "${INSTALL_CMAKE_DIR}"
)

set(PACKAGE_VERSION ${PROJECT_VERSION})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/${APP_PACKAGE_NAME}Config.cmake.in
               ${CMAKE_CURRENT_BINARY_DIR}/${APP_PACKAGE_NAME}Config.cmake @ONLY)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/${APP_PACKAGE_NAME}ConfigVersion.cmake.in
               ${CMAKE_CURRENT_BINARY_DIR}/${APP_PACKAGE_NAME}ConfigVersion.cmake @ONLY)