cmake_minimum_required(VERSION 3.25.0)

project(FQGL VERSION 0.1.0 LANGUAGES CXX)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
  SET(CMAKE_CXX_FLAGS "-O2")
else()
  SET(CMAKE_CXX_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage")
  set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE ON)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if (NOT BUILD_PREFIX )
  set(BUILD_PREFIX ~/build)
endif()
message("using ${BUILD_PREFIX} for installing library and headers")

if("${BUILD_TYPE}" STREQUAL "SHARED")
    message("Build type: ${BUILD_TYPE}")
endif()

# dependencies
find_package(Qt6 REQUIRED COMPONENTS Core Gui OpenGLWidgets Test)
find_package(OpenGL REQUIRED)

include_directories( ${OPENGL_INCLUDE_DIRS} )
qt_standard_project_setup()

# install
#set(FQPCLIENT_INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include/FQPClient)
#set(FQPCLIENT_INSTALL_BIN_DIR ${CMAKE_INSTALL_PREFIX}/bin)
#set(FQNET_INSTALL_TOOLS_DIR ${CMAKE_INSTALL_PREFIX}/tools/FQGL)
#set(FQNET_INSTALL_LIB_DIR ${CMAKE_INSTALL_PREFIX}/lib)
set(FQNET_INSTALL_INCLUDE_DIR ${BUILD_PREFIX}/include/FQGL)
set(FQNET_INSTALL_LIB_DIR ${BUILD_PREFIX}/lib)

set(HEADER_FILES
  src/FQGLCamera.h
  src/FQGLFrustum.h
  src/FQGLPrim.h
  src/FQGLTypes.h
  src/FQGLController.h
  src/fqgl_global.h
  src/FQGLResponder.h
  src/FQGLWidget.h
  src/FQGLFrustumCompute.h
  src/FQGLPlane.h
  src/FQGLScene.h
)

qt_add_library(FQGL SHARED
  MANUAL_FINALIZATION
  ${HEADER_FILES}
  src/FQGLCamera.cpp
  src/FQGLFrustum.cpp
  src/FQGLResponder.cpp
  src/FQGLController.cpp
  src/FQGLPlane.cpp
  src/FQGLScene.cpp
  src/FQGLFrustumCompute.cpp
  src/FQGLPrim.cpp
  src/FQGLWidget.cpp
)

target_link_libraries(FQGL PUBLIC Qt6::Core Qt6::Gui Qt6::OpenGLWidgets)
# target_link_libraries(FQNClientTest PUBLIC FQGL Qt6::Core Qt6::OpenGL Qt6::Test)
# target_link_libraries(FQNOperationTest PUBLIC FQGL Qt6::Core Qt6::OpenGL Qt6::Test)

# this is for users of the library
#target_include_directories(FQGL INTERFACE ${BUILD_PREFIX}/include/FQGL/)

# qt_finalize_executable(FQNClientTest)
# qt_finalize_executable(FQNOperationTest)

# add_custom_target(cover
#   COMMAND mkdir -p coverage
#   COMMAND ${CMAKE_MAKE_PROGRAM} FQNClientTest
#   COMMAND ${CMAKE_MAKE_PROGRAM} FQNOperationTest
#   WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
# )

# Create OBJECT_DIR variable
# set(CLIENT_OBJECT_DIR ${CMAKE_BINARY_DIR}/CMakeFiles/FQNClientTest.dir)
# set(OPERATION_OBJECT_DIR ${CMAKE_BINARY_DIR}/CMakeFiles/FQNOperationTest.dir)
set(BASE_OBJECT_DIR ${CMAKE_BINARY_DIR}/CMakeFiles)

# add_custom_command(TARGET cover
#   WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
#   COMMAND ./FQNClientTest
#   COMMAND ./FQNOperationTest
#   COMMAND echo "=================== GCOV ===================="
#   COMMAND gcov ${CMAKE_SOURCE_DIR}/FQNClientTest.cpp --object-directory ${CLIENT_OBJECT_DIR}
#         | grep -A 5 "FQNClientTest.cpp" > CoverateSummary.tmp
#   COMMAND gcov ${CMAKE_SOURCE_DIR}/FQNOperationTest.cpp --object-directory ${OPERATION_OBJECT_DIR}
#         | grep -A 5 "FQNOperationTest.cpp" >> CoverateSummary.tmp
# #  COMMAND lcov --directory ${CLIENT_OBJECT_DIR} --capture --output-file coverage.info
# #  COMMAND lcov --directory ${OPERATION_OBJECT_DIR} --capture --output-file coverage.info
# # use the parent directory because lcov recurses to include all the tests
#   COMMAND lcov --directory ${BASE_OBJECT_DIR} --capture --output-file coverage.info
#   COMMAND genhtml -o html_report coverage.info
#   COMMAND xdg-open html_report/index.html
# )

install(TARGETS FQGL DESTINATION ${FQGL_INSTALL_LIB_DIR})
install(FILES ${HEADER_FILE} DESTINATION ${FQGL_INSTALL_INCLUDE_DIR})
