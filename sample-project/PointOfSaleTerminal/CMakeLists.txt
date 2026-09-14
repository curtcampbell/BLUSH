# sample-project/PointOfSaleTerminal/CMakeLists.txt
#
# See Alpha_Comp/CMakeLists.txt for the pattern this follows.

add_library(PointOfSaleTerminal_impl STATIC
    src/PointOfSaleTerminalImpl.cpp
)
target_include_directories(PointOfSaleTerminal_impl PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_link_libraries(PointOfSaleTerminal_impl
    PUBLIC  PointOfSaleTerminal_lib
    PRIVATE sample_common
)
