# face-core/CMakeLists.txt
# FACE::Core -- header-only library of FACE Technical Standard 3.2 C++
# interfaces (TSS, LCM, IOSS) and their Injectable adapter namespaces.

add_library(face_core INTERFACE)
add_library(FACE::Core ALIAS face_core)
set_target_properties(face_core PROPERTIES EXPORT_NAME Core)

target_include_directories(face_core INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_features(face_core INTERFACE cxx_std_17)

install(TARGETS face_core
    EXPORT BLUSHTargets
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)
install(DIRECTORY include/ DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

if(BLUSH_BUILD_TESTS AND BUILD_TESTING)
    add_subdirectory(test)
endif()
