# sample-project/Alpha_Comp/CMakeLists.txt
#
# Static library implementing the Alpha_Comp UoP by deriving from the
# generated Alpha_CompBase class. Alpha_Comp_lib (the generated library)
# already links PUBLIC against face_data_model and FACE::Util; linking
# PUBLIC against it here propagates both transitively to anything that
# links Alpha_Comp_impl (e.g. the integration executable).

add_library(Alpha_Comp_impl STATIC
    src/Alpha_CompImpl.cpp
)
target_include_directories(Alpha_Comp_impl PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_link_libraries(Alpha_Comp_impl
    PUBLIC  Alpha_Comp_lib
    PRIVATE sample_common
)
