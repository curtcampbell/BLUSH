# data-model/CMakeLists.txt
#
# data-model produces no library of its own to build here -- its deliverable
# is templates/ (fed to face-codegen) plus the blush_add_data_model_library()
# CMake function (cmake/BLUSHDataModelCodegen.cmake) that downstream
# FACE-aligned projects call, with their own config file (or bare .face
# file), after find_package(BLUSH). uop-generator's own generated UoP
# libraries link the resulting `face_data_model` target too.
#
# examples/GROCERY.yaml exercises that same function here, via the CONFIG
# path specifically, as a worked example / smoke test.

find_program(FACE_IDL_GEN_EXECUTABLE face-idl-gen)
find_program(FACE_CODEGEN_EXECUTABLE face-codegen)

if(FACE_IDL_GEN_EXECUTABLE AND FACE_CODEGEN_EXECUTABLE)
    blush_add_data_model_library(
        CONFIG       examples/GROCERY.yaml
        TEMPLATE_DIR templates
    )
else()
    message(STATUS
        "face-idl-gen/face-codegen not found on PATH -- skipping the "
        "data-model GROCERY example (see https://github.com/curtcampbell/FACE-IDL-Parser).")
endif()
