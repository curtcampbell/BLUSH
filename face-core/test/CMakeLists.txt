# Core/test/CMakeLists.txt
# Compile-time verification tests for the FACE::Core interface library.
# These executables contain only static_asserts and are expected to compile
# and link cleanly; they produce no runtime output.

add_executable(face_smoke_test face_smoke_test.cpp)
target_link_libraries(face_smoke_test PRIVATE FACE::Core)

add_executable(minimal_test minimal_test.cpp)
target_link_libraries(minimal_test PRIVATE FACE::Core)

add_test(NAME face_smoke_test COMMAND face_smoke_test)
add_test(NAME minimal_test   COMMAND minimal_test)
