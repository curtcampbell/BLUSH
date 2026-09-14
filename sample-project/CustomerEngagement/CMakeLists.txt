# sample-project/CustomerEngagement/CMakeLists.txt
#
# See Alpha_Comp/CMakeLists.txt for the pattern this follows.

add_library(CustomerEngagement_impl STATIC
    src/CustomerEngagementImpl.cpp
)
target_include_directories(CustomerEngagement_impl PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_link_libraries(CustomerEngagement_impl
    PUBLIC  CustomerEngagement_lib
    PRIVATE sample_common
)
