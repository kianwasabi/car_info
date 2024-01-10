cmake_minimum_required (VERSION 3.15)
project(Car_Info VERSION 0.1 LANGUAGES CXX)

### PACKAGES ###
find_package(Python3 COMPONENTS Development)
find_package(Boost REQUIRED COMPONENTS python3)
find_package(CommonAPI REQUIRED)
find_package(CommonAPI-SomeIP REQUIRED)

### VARIABLES ###
set(APP_PACKAGE_NAME CarInfo-someip)
set(MAJOR_VERSION 0)
set(MINOR_VERSION 1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++11")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

### INSTALLATION DIRECTORIES ###
set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for binaries")
set(INSTALL_INCLUDE_DIR include/CarInfoService CACHE PATH "Installation directory for header files")
set(DEF_INSTALL_CMAKE_DIR lib/cmake/CarInfoService-someip)
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH "Installation directory for CMake files")

foreach(p LIB INCLUDE CMAKE)
  set(var INSTALL_${p}_DIR)
  if(NOT IS_ABSOLUTE "${${var}}")
    set(ABSOLUTE_${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
  endif()
endforeach()

set(Car_Info_SOURCES
    src/CarInfoService.cpp
    src/CarInfoStubImpl.cpp
    src/PiRacer.cpp
)

FILE(GLOB PRJ_SOMEIP_LIB_SRCS ./src-gen/someip/v${MAJOR_VERSION}/${APP_PACKAGE_NAME}/*.cpp)

### INCLUDE AND LINK DIRECTORIES ###
include_directories(
    ./include
    ./src-gen/core
    ./src-gen/someip
    ${Boost_INCLUDE_DIRS}
    ${Python3_INCLUDE_DIRS}
    ${COMMONAPI_INCLUDE_DIRS}
    ${COMMONAPI_SOMEIP_INCLUDE_DIRS}
)
link_directories(
    ${Boost_LIBRARY_DIR}
    ${Python3_LIBRARY_DIRS}
    ${COMMONAPI_LIBDIR}
    ${COMMONAPI_SOMEIP_CMAKE_DIR}/build
)

### BUILD TARGETS ###
# Service
add_executable(CarInfoService 
    ${Car_Info_SOURCES}
)
target_link_libraries(CarInfoService 
    CommonAPI 
    CommonAPI-SomeIP 
    vsomeip3
    Python3::Python 
    Boost::python3
)
add_library(CarInfoService-someip
    SHARED ${PRJ_SOMEIP_LIB_SRCS}
)
target_link_libraries(CarInfoService-someip
    CommonAPI
    CommonAPI-SomeIP
)

set_target_properties(CarInfoService-someip PROPERTIES 
    VERSION ${MAJOR_VERSION}.${MINOR_VERSION} 
    SOVERSION ${MAJOR_VERSION}.${MINOR_VERSION}
)

target_include_directories(CarInfoService-someip PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src-gen/core>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src-gen/someip>
    $<INSTALL_INTERFACE:${INSTALL_INCLUDE_DIR}>
)

### INSTALLATION ###
install(TARGETS CarInfoService
        RUNTIME DESTINATION ${INSTALL_BIN_DIR}
)
install(TARGETS CarInfoService-someip
        LIBRARY DESTINATION ${INSTALL_LIB_DIR}
        ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
)

file(GLOB_RECURSE HEADER_FILES "src-gen/core/v${MAJOR_VERSION}/${APP_PACKAGE_NAME}/*Proxy*.hpp")
foreach(HEADER ${HEADER_FILES})
    get_filename_component(HEADER_ABS_PATH ${HEADER} ABSOLUTE)
    file(RELATIVE_PATH HEADER_REL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src-gen/core" ${HEADER_ABS_PATH})
    get_filename_component(HEADER_DIR ${HEADER_REL_PATH} DIRECTORY)
    install(FILES ${HEADER_ABS_PATH} DESTINATION ${INSTALL_INCLUDE_DIR}/${HEADER_DIR})
endforeach()

# Add all targets to the build-tree export set
export(TARGETS CarInfoService-someip
  FILE "${PROJECT_BINARY_DIR}/CarInfo-someipTargets.cmake"
)

# Export the package for use from the build-tree
export(PACKAGE CarInfo-someip)

# Create the CarInfo-someipConfig.cmake and CarInfo-someipConfigVersion files
file(RELATIVE_PATH REL_INCLUDE_DIR "${ABSOLUTE_INSTALL_CMAKE_DIR}" "${ABSOLUTE_INSTALL_INCLUDE_DIR}")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/car_control/cmake/CarInfo-someipConfig.cmake.in
  "${PROJECT_BINARY_DIR}/CarInfo-someipConfig.cmake" @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/car_control/cmake/CarInfo-someipConfigVersion.cmake.in
  "${PROJECT_BINARY_DIR}/CarInfo-someipConfigVersion.cmake" @ONLY)

install(FILES
  "${PROJECT_BINARY_DIR}/CarInfo-someipConfig.cmake"
  "${PROJECT_BINARY_DIR}/CarInfo-someipConfigVersion.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}"
)

install(EXPORT CarInfo-someipTargets
  DESTINATION "${INSTALL_CMAKE_DIR}"
)




# # Client
# add_executable(CarInfoClient 
#     src/CarInfoClient.cpp
# )
# target_link_libraries(CarInfoClient 
#     CommonAPI 
#     CommonAPI-SomeIP 
#     vsomeip3
# )
# target_include_directories(CarInfoClient PRIVATE ${PRJ_INCLUDE_DIR})


