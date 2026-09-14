# sample-project/ReplenishmentService/CMakeLists.txt
#
# See Alpha_Comp/CMakeLists.txt for the pattern this follows.

add_library(ReplenishmentService_impl STATIC
    src/ReplenishmentServiceImpl.cpp
)
target_include_directories(ReplenishmentService_impl PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_link_libraries(ReplenishmentService_impl
    PUBLIC  ReplenishmentService_lib
    PRIVATE sample_common
)
