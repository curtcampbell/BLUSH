# face-utils/test/CMakeLists.txt
# GoogleTest-based unit tests for FACEUtil (tinyxml2 itself is untested here;
# it is a vendored third-party library, not FACEUtil code).

include(GoogleTest)

function(add_face_utils_test name)
    add_executable(${name} ${ARGN})
    target_include_directories(${name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(${name} PRIVATE FACE::Util GTest::gtest_main GTest::gmock)
    gtest_discover_tests(${name})
endfunction()

add_face_utils_test(Cancellable_test          Cancellable_test.cpp)
add_face_utils_test(EventRegistration_test    EventRegistration_test.cpp)
add_face_utils_test(EventDispatcher_test      EventDispatcher_test.cpp)
add_face_utils_test(IDGenerator_test          IDGenerator_test.cpp)
add_face_utils_test(ReadWriteLocked_test      ReadWriteLocked_test.cpp)
add_face_utils_test(UopBase_test              UopBase_test.cpp)
add_face_utils_test(FaceTypeTraits_test       FaceTypeTraits_test.cpp)
add_face_utils_test(FaceInjectable_test       FaceInjectable_test.cpp)
add_face_utils_test(TaskRunner_test           TaskRunner_test.cpp)
add_face_utils_test(TaskScheduler_test        TaskScheduler_test.cpp)
add_face_utils_test(PublisherConnection_test  PublisherConnection_test.cpp)
add_face_utils_test(SubscriberConnection_test SubscriberConnection_test.cpp)
add_face_utils_test(RequesterConnection_test  RequesterConnection_test.cpp)
add_face_utils_test(ResponderConnection_test  ResponderConnection_test.cpp)
add_face_utils_test(ConnectionResolver_test   ConnectionResolver_test.cpp)
