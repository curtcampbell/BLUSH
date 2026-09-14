# sample-project/common/CMakeLists.txt
#
# Header-only helper shared by every UoP impl and the integration
# executable -- not part of face-utils because it exists purely to make
# this demo's console output readable, not because any real UoP needs it.

add_library(sample_common INTERFACE)
target_include_directories(sample_common INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_link_libraries(sample_common INTERFACE FACE::Util)
