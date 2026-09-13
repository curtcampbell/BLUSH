# face-utils/CMakeLists.txt
# FACE::Util -- UoP support utilities built on top of FACE::Core:
# connection classes (Publisher/Subscriber/Requester/Responder), the
# ConnectionResolver, task scheduling, and a vendored tinyxml2 for parsing
# uop-connections.xml.

# Explicitly STATIC (not left to BUILD_SHARED_LIBS): UoP shared objects that
# link this are meant to be self-contained plugins with no runtime ABI
# dependency on a separately-versioned libface_util.so.
add_library(face_util STATIC
    src/TaskRunner.cpp
    src/TaskScheduler.cpp
    src/tinyxml2.cpp
)
add_library(FACE::Util ALIAS face_util)
set_target_properties(face_util PROPERTIES EXPORT_NAME Util)

target_include_directories(face_util PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(face_util PUBLIC FACE::Core)
target_compile_features(face_util PUBLIC cxx_std_17)

install(TARGETS face_util
    EXPORT BLUSHTargets
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)
install(DIRECTORY include/ DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

if(BLUSH_BUILD_TESTS AND BUILD_TESTING)
    add_subdirectory(test)
endif()
