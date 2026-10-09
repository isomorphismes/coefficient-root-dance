cmake_minimum_required(VERSION 3.22.1)
project(coefficient_root_dance C)

set(NATIVE_GLUE_DIR "${CMAKE_ANDROID_NDK}/sources/android/native_app_glue")

add_library(native_app_glue STATIC
    "${NATIVE_GLUE_DIR}/android_native_app_glue.c"
)
target_include_directories(native_app_glue PUBLIC "${NATIVE_GLUE_DIR}")

get_filename_component(APPLICATION_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../../.." ABSOLUTE)
set(AICI_ICK_ROOT "${APPLICATION_ROOT}/.ai-ci-ick" CACHE PATH "Pinned ai-ci checkout")
set(ICK_STAGE_ROOT "${APPLICATION_ROOT}/build/ick" CACHE PATH "Qualified ICK ABI stages")
include("${AICI_ICK_ROOT}/ick-android/OwnedC.cmake")
if(NOT ICK_COMPILER)
  ick_android_stage(ROOT "${ICK_STAGE_ROOT}/${ANDROID_ABI}" FORTIFY_SOURCE 2)
endif()
ick_android_objects(owned_objects NAME coefficient_root_dance SOURCES dance.c polynomial.c
  INCLUDES "${NATIVE_GLUE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}"
  OPTIONS -Wall -Wextra -Wpedantic STANDARD 11)

add_library(coefficient_root_dance SHARED ${owned_objects})
set_target_properties(coefficient_root_dance PROPERTIES LINKER_LANGUAGE C)
target_compile_features(coefficient_root_dance PRIVATE c_std_11)
target_compile_options(coefficient_root_dance PRIVATE -Wall -Wextra -Wpedantic)
target_link_options(coefficient_root_dance PRIVATE -u ANativeActivity_onCreate)
target_link_libraries(coefficient_root_dance
    native_app_glue
    android
    log
    EGL
    GLESv3
    m
)

option(COEFFICIENT_RENDERER_STATE_TEST "Build the real renderer's scalar upload regression" OFF)
if(COEFFICIENT_RENDERER_STATE_TEST)
  if(NOT ANDROID_ABI STREQUAL "x86_64")
    message(FATAL_ERROR "The executable Bionic renderer regression requires an x86_64 host")
  endif()
  ick_android_objects(renderer_test_objects NAME renderer_state_test
    SOURCES "${APPLICATION_ROOT}/tests/renderer_state_test.c" polynomial.c
    INCLUDES "${NATIVE_GLUE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}"
    OPTIONS -Wall -Wextra -Wpedantic -UNDEBUG -ffunction-sections -fdata-sections
    STANDARD 11)
  add_executable(renderer_state_test ${renderer_test_objects})
  set_target_properties(renderer_state_test PROPERTIES LINKER_LANGUAGE C)
  target_link_options(renderer_state_test PRIVATE -static -Wl,--gc-sections)
  target_link_libraries(renderer_state_test m)
endif()
