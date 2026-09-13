# BLUSHUopCodegen.cmake
#
# blush_add_uop_libraries(
#     FACE_FILE     <path-to-.face-file>
#     [TEMPLATE_DIR <dir>]           # default: BLUSH_UOP_TEMPLATE_DIR
#     [OUTPUT_DIR   <dir>]           # default: <binary-dir>/uop-generated/<face-file-name>
#     [EXCLUDE_FROM_ALL]             # generated <UoP>_lib targets are excluded from the default build
#     [OUT_LIBRARIES <var>]          # receives the list of generated <UoP>_lib target names
# )
#
# Runs the FACE-IDL-Parser pipeline (face-idl-gen -> face-codegen) against a
# .face XMI model using BLUSH's uop-generator templates, and adds one library
# target per UoP the model defines -- face-codegen itself writes a
# CMakeLists.txt into each generated UoP directory (see
# templates/UoPCMakeLists.txt.vm), so this function never needs to know UoP
# names or parse the .face file itself; it only needs to run codegen and
# add_subdirectory() whatever came out.
#
# Every generated UoP CMakeLists.txt links PUBLIC against `face_data_model`
# (the shared DM/TSS headers -- see data-model/ and blush_add_data_model_library())
# and FACE::Util. This function ensures face_data_model exists (defining it,
# from the same FACE_FILE, if some other part of the project hasn't already).

include("${CMAKE_CURRENT_LIST_DIR}/BLUSHFaceCodegenPipeline.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/BLUSHDataModelCodegen.cmake")

function(blush_add_uop_libraries)
    set(options EXCLUDE_FROM_ALL)
    set(oneValueArgs FACE_FILE TEMPLATE_DIR OUTPUT_DIR OUT_LIBRARIES)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "" ${ARGN})

    if(NOT ARG_FACE_FILE)
        message(FATAL_ERROR "blush_add_uop_libraries: FACE_FILE is required")
    endif()
    get_filename_component(face_file "${ARG_FACE_FILE}" ABSOLUTE BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
    if(NOT EXISTS "${face_file}")
        message(FATAL_ERROR "blush_add_uop_libraries: FACE_FILE not found: ${face_file}")
    endif()

    if(ARG_TEMPLATE_DIR)
        get_filename_component(template_dir "${ARG_TEMPLATE_DIR}" ABSOLUTE BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
    elseif(DEFINED BLUSH_UOP_TEMPLATE_DIR)
        set(template_dir "${BLUSH_UOP_TEMPLATE_DIR}")
    else()
        message(FATAL_ERROR
            "blush_add_uop_libraries: TEMPLATE_DIR not given and BLUSH_UOP_TEMPLATE_DIR is not set "
            "(it is normally provided by find_package(BLUSH))")
    endif()

    if(ARG_OUTPUT_DIR)
        get_filename_component(output_dir "${ARG_OUTPUT_DIR}" ABSOLUTE BASE_DIR "${CMAKE_CURRENT_BINARY_DIR}")
    else()
        get_filename_component(face_name "${face_file}" NAME_WE)
        set(output_dir "${CMAKE_CURRENT_BINARY_DIR}/uop-generated/${face_name}")
    endif()

    blush_add_data_model_library(FACE_FILE "${face_file}")

    _blush_run_face_codegen_pipeline(
        FACE_FILE       "${face_file}"
        TEMPLATE_DIR    "${template_dir}"
        OUTPUT_DIR      "${output_dir}"
        STAMP_NAME      "blush-uop-stamp"
        OUT_CODEGEN_DIR codegen_dir
    )

    # face-codegen already wrote a CMakeLists.txt into each UoP's directory;
    # discovering them here (rather than parsing the .face file ourselves) is
    # what lets this function stay agnostic to UoP names and count.
    set(subdirectory_args "")
    if(ARG_EXCLUDE_FROM_ALL)
        set(subdirectory_args EXCLUDE_FROM_ALL)
    endif()

    set(uop_libraries "")
    file(GLOB uop_dirs LIST_DIRECTORIES true "${codegen_dir}/*")
    foreach(_dir ${uop_dirs})
        if(EXISTS "${_dir}/CMakeLists.txt")
            get_filename_component(_uop_name "${_dir}" NAME)
            add_subdirectory("${_dir}" "${CMAKE_CURRENT_BINARY_DIR}/uop-build/${_uop_name}" ${subdirectory_args})
            list(APPEND uop_libraries "${_uop_name}_lib")
        endif()
    endforeach()

    if(ARG_OUT_LIBRARIES)
        set(${ARG_OUT_LIBRARIES} "${uop_libraries}" PARENT_SCOPE)
    endif()
endfunction()
