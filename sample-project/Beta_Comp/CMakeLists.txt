# sample-project/Beta_Comp/CMakeLists.txt
#
# See Alpha_Comp/CMakeLists.txt for the pattern this follows.

add_library(Beta_Comp_impl STATIC
    src/Beta_CompImpl.cpp
)
target_include_directories(Beta_Comp_impl PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_link_libraries(Beta_Comp_impl
    PUBLIC  Beta_Comp_lib
    PRIVATE sample_common
)
