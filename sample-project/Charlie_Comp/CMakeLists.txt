# sample-project/Charlie_Comp/CMakeLists.txt
#
# See Alpha_Comp/CMakeLists.txt for the pattern this follows.

add_library(Charlie_Comp_impl STATIC
    src/Charlie_CompImpl.cpp
)
target_include_directories(Charlie_Comp_impl PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_link_libraries(Charlie_Comp_impl
    PUBLIC  Charlie_Comp_lib
    PRIVATE sample_common
)
