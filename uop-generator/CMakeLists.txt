# uop-generator/CMakeLists.txt
#
# uop-generator itself produces no library -- its deliverable is templates/
# (fed to face-codegen) plus the blush_add_uop_libraries() CMake function
# (cmake/BLUSHUopCodegen.cmake) that downstream FACE-aligned application
# repos call, with their own .face file, after find_package(BLUSH).
#
# examples/GROCERY_with_IM.face exercises that same function here as a
# worked example / smoke test of the templates. EXCLUDE_FROM_ALL: a known
# bug in the installed face-idl-gen's C++ language binding generator
# (unrelated to BLUSH -- a generated #include with no relative path) means
# the GROCERY UoPs don't fully compile yet, so this must not block a normal
# `cmake --build` for everyone else; build a specific <UoP>_lib target
# on-demand to exercise it.

find_program(FACE_IDL_GEN_EXECUTABLE face-idl-gen)
find_program(FACE_CODEGEN_EXECUTABLE face-codegen)

if(FACE_IDL_GEN_EXECUTABLE AND FACE_CODEGEN_EXECUTABLE)
    blush_add_uop_libraries(
        FACE_FILE        examples/GROCERY_with_IM.face
        TEMPLATE_DIR     templates
        EXCLUDE_FROM_ALL
        OUT_LIBRARIES    GROCERY_UOP_LIBRARIES
    )
else()
    message(STATUS
        "face-idl-gen/face-codegen not found on PATH -- skipping the "
        "uop-generator GROCERY example (see https://github.com/curtcampbell/FACE-IDL-Parser).")
endif()
