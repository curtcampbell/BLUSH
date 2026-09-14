# sample-project/Delta_Comp/CMakeLists.txt
#
# See Alpha_Comp/CMakeLists.txt for the pattern this follows.

add_library(Delta_Comp_impl STATIC
    src/Delta_CompImpl.cpp
)
target_include_directories(Delta_Comp_impl PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_link_libraries(Delta_Comp_impl
    PUBLIC  Delta_Comp_lib
    PRIVATE sample_common
)
