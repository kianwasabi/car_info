### SETUP ###
cmake_minimum_required (VERSION 3.0)
set(PROJ_NAME Car_Info)
project(${PROJ_NAME})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++11")

### PACKAGES ###
find_package(Python3 COMPONENTS Development)
find_package(Boost REQUIRED COMPONENTS python3)
find_package(CommonAPI REQUIRED)
find_package(CommonAPI-SomeIP REQUIRED)

### VARIABLES ###
set(PRJ_SRC_VAR src)
set(PRJ_SRC_GEN_VAR src-gen)
set(PRJ_SRC_GEN_PATH ${CMAKE_CURRENT_SOURCE_DIR}/${PRJ_SRC_GEN_VAR})
set(CORE_GEN_SRC_DIR ${PRJ_SRC_GEN_PATH}/core)
set(SOMEIP_GEN_SRC_DIR ${PRJ_SRC_GEN_PATH}/someip)
set(CORE_RUNTIME_DIR ~/capicxx-core-runtime)
set(SOMEIP_RUNTIME_DIR ~/capicxx-someip-runtime)
set(VSOMEIP_DIR ~/vsomeip)
set(COMMONAPI_INTERFACE_VER v0/commonapi)

set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for binaries")
set(INSTALL_INCLUDE_DIR include/CanReceiver CACHE PATH "Installation directory for header files")
set(DEF_INSTALL_CMAKE_DIR lib/cmake/CanReceiver-someip)
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH "Installation directory for CMake files")

foreach(p LIB INCLUDE CMAKE)
  set(var INSTALL_${p}_DIR)
  if(NOT IS_ABSOLUTE "${${var}}")
    set(ABSOLUTE_${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
  endif()
endforeach()

FILE(GLOB PRJ_SOMEIP_LIB_SRCS ./src-gen/someip/v0/commonapi/*.cpp)

### DIRECTORIES ###
include_directories(
	./include
    ./src-gen/core
    ./src-gen/someip
	${Python3_INCLUDE_DIRS}
    ${COMMONAPI_INCLUDE_DIRS}
    ${COMMONAPI_SOMEIP_INCLUDE_DIRS}
)

link_directories(
    ${COMMONAPI_LIBDIR}
    ${COMMONAPI_SOMEIP_CMAKE_DIR}/build
    ${Boost_LIBRARY_DIR}
    # ${SOMEIP_RUNTIME_DIR}/build
    # ${CORE_RUNTIME_DIR}/build
    # ${VSOMEIP_DIR}/build
)

### BUILD ###
# Services
add_executable(CarInfo
    ${PRJ_SRC_VAR}/PiRacer.cpp
    ${PRJ_SRC_VAR}/CarInfoService.cpp
    ${PRJ_SRC_VAR}/CarInfoStubImpl.cpp
)
target_link_libraries(CarInfo
    CommonAPI 
    CommonAPI-SomeIP 
    vsomeip3
    Python3::Python 
    Boost::python
)

add_library(CarInfo-someip
    SHARED ${PRJ_SOMEIP_LIB_SRCS}
)
target_link_libraries(CarInfo-someip
    CommonAPI
    CommonAPI-SomeIP
)
set_target_properties(CarInfo-someip PROPERTIES VERSION ${MAJOR_VERSION}.${MINOR_VERSION} SOVERSION ${MAJOR_VERSION}.${MINOR_VERSION})
target_include_directories(CarInfo-someip PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src-gen/core>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src-gen/someip>
    $<INSTALL_INTERFACE:${INSTALL_INCLUDE_DIR}>
)
set_target_properties (CarInfo-someip PROPERTIES INTERFACE_LINK_LIBRARY "")

### INSTALL ###
file(GLOB_RECURSE HEADER_FILES "src-gen/core/v${MAJOR_VERSION}/${APP_PACKAGE_NAME}/*Proxy*.hpp")
foreach(HEADER ${HEADER_FILES})
    # Get absolute path of header files.
    get_filename_component(HEADER_ABS_PATH ${HEADER} ABSOLUTE)

    # Get relative path of header files.
    file(RELATIVE_PATH HEADER_REL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src-gen/core" ${HEADER_ABS_PATH})

    # Get directory name from file name.
    get_filename_component(HEADER_DIR ${HEADER_REL_PATH} DIRECTORY)

    # Install headers to desired directory.
    install(FILES ${HEADER_ABS_PATH} DESTINATION ${INSTALL_INCLUDE_DIR}/${HEADER_DIR})
endforeach()

install(TARGETS CarInfo
        EXPORT CarInfoTargets
        LIBRARY DESTINATION ${INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${INSTALL_BIN_DIR}
        ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
        PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}"
)

install(TARGETS CarInfo-someip
        EXPORT CarInfo-someipTargets
        LIBRARY DESTINATION ${INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${INSTALL_BIN_DIR}
        ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
        PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}"
)

# Add all targets to the build-tree export set
export(TARGETS CarInfo-someip
  FILE "${PROJECT_BINARY_DIR}/CarInfo-someipTargets.cmake"
)

# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE CarInfo-someip)

# Create the CarInfo-someipConfig.cmake and CarInfo-someipConfigVersion files ...
file(RELATIVE_PATH REL_INCLUDE_DIR "${ABSOLUTE_INSTALL_CMAKE_DIR}" "${ABSOLUTE_INSTALL_INCLUDE_DIR}")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/CarInfo-someipConfig.cmake.in
  "${PROJECT_BINARY_DIR}/CarInfo-someipConfig.cmake" @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/CarInfo-someipConfigVersion.cmake.in
  "${PROJECT_BINARY_DIR}/CarInfo-someipConfigVersion.cmake" @ONLY)

install(FILES
  "${PROJECT_BINARY_DIR}/CarInfo-someipConfig.cmake"
  "${PROJECT_BINARY_DIR}/CarInfo-someipConfigVersion.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}"
)

install(EXPORT CarInfo-someipTargets
  DESTINATION "${INSTALL_CMAKE_DIR}"
)