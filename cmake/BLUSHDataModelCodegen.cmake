# BLUSHDataModelCodegen.cmake
#
# blush_add_data_model_library(
#     FACE_FILE      <path>   |   CONFIG <path>   # exactly one of these
#     [TEMPLATE_DIR  <dir>]        # default: BLUSH_DATA_MODEL_TEMPLATE_DIR
#     [OUTPUT_DIR    <dir>]        # default: <binary-dir>/data-model-generated/<face-file-name>
#     [OUT_LIBRARY   <var>]        # receives the generated library's target name
# )
#
# Runs the FACE-IDL-Parser pipeline (face-idl-gen -> face-codegen) against a
# .face XMI model using BLUSH's data-model templates, and defines a single
# header-only `face_data_model` target (FACE::Core plus the generated DM/TSS
# headers) that both uop-generator's generated UoP libraries and hand-written
# FACE components link against. Idempotent: calling this more than once (e.g.
# once directly and once indirectly via blush_add_uop_libraries()) for the
# same model is a no-op after the first call.
#
# CONFIG points at a small declarative file (see examples/GROCERY.yaml) rather
# than passing options as CMake arguments, specifically so the same file could
# be read identically by a future non-CMake build-system integration (Gradle,
# Cargo, ...) -- CMake treats its *contents* as close to opaque as practical:
# _blush_read_data_model_config() below only understands a flat `key: value`
# subset (no nesting, no lists), just enough for today's single `face_file:`
# key. It is deliberately not a general YAML parser. Once real per-language or
# per-namespace options are needed, that's the point to either grow this
# reader properly or (better, so every build-system wrapper benefits from one
# implementation) push config interpretation into the FACE-IDL-Parser tooling
# itself and have this function just hand the config path through, the way it
# already hands FACE_FILE to face-idl-gen/face-codegen.
#
# A relative face_file: path in a CONFIG file resolves relative to the config
# file's own directory, not the calling CMakeLists.txt -- again so a
# non-CMake reader would resolve it the same way.

include("${CMAKE_CURRENT_LIST_DIR}/BLUSHFaceCodegenPipeline.cmake")

function(_blush_read_data_model_config config_file out_face_file)
    file(STRINGS "${config_file}" _lines)
    set(_face_file "")
    foreach(_line ${_lines})
        string(REGEX REPLACE "#.*$" "" _line "${_line}")
        string(STRIP "${_line}" _line)
        if(_line MATCHES "^face_file:[ \t]*(.+)$")
            set(_face_file "${CMAKE_MATCH_1}")
            string(STRIP "${_face_file}" _face_file)
            string(REGEX REPLACE "^[\"']|[\"']$" "" _face_file "${_face_file}")
        endif()
    endforeach()
    set(${out_face_file} "${_face_file}" PARENT_SCOPE)
endfunction()

function(blush_add_data_model_library)
    set(oneValueArgs FACE_FILE CONFIG TEMPLATE_DIR OUTPUT_DIR OUT_LIBRARY)
    cmake_parse_arguments(ARG "" "${oneValueArgs}" "" ${ARGN})

    if(ARG_CONFIG AND ARG_FACE_FILE)
        message(FATAL_ERROR "blush_add_data_model_library: specify only one of CONFIG or FACE_FILE")
    endif()

    if(ARG_CONFIG)
        get_filename_component(config_file "${ARG_CONFIG}" ABSOLUTE BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
        if(NOT EXISTS "${config_file}")
            message(FATAL_ERROR "blush_add_data_model_library: CONFIG not found: ${config_file}")
        endif()
        set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${config_file}")

        _blush_read_data_model_config("${config_file}" _face_file)
        if(NOT _face_file)
            message(FATAL_ERROR "blush_add_data_model_library: ${config_file} has no face_file: entry")
        endif()
        get_filename_component(config_dir "${config_file}" DIRECTORY)
        get_filename_component(face_file "${_face_file}" ABSOLUTE BASE_DIR "${config_dir}")
    elseif(ARG_FACE_FILE)
        get_filename_component(face_file "${ARG_FACE_FILE}" ABSOLUTE BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
    else()
        message(FATAL_ERROR "blush_add_data_model_library: one of CONFIG or FACE_FILE is required")
    endif()

    if(NOT EXISTS "${face_file}")
        message(FATAL_ERROR "blush_add_data_model_library: .face file not found: ${face_file}")
    endif()

    if(ARG_TEMPLATE_DIR)
        get_filename_component(template_dir "${ARG_TEMPLATE_DIR}" ABSOLUTE BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
    elseif(DEFINED BLUSH_DATA_MODEL_TEMPLATE_DIR)
        set(template_dir "${BLUSH_DATA_MODEL_TEMPLATE_DIR}")
    else()
        message(FATAL_ERROR
            "blush_add_data_model_library: TEMPLATE_DIR not given and BLUSH_DATA_MODEL_TEMPLATE_DIR "
            "is not set (it is normally provided by find_package(BLUSH))")
    endif()

    if(NOT TARGET face_data_model)
        if(ARG_OUTPUT_DIR)
            get_filename_component(output_dir "${ARG_OUTPUT_DIR}" ABSOLUTE BASE_DIR "${CMAKE_CURRENT_BINARY_DIR}")
        else()
            get_filename_component(face_name "${face_file}" NAME_WE)
            set(output_dir "${CMAKE_CURRENT_BINARY_DIR}/data-model-generated/${face_name}")
        endif()

        _blush_run_face_codegen_pipeline(
            FACE_FILE       "${face_file}"
            TEMPLATE_DIR    "${template_dir}"
            OUTPUT_DIR      "${output_dir}"
            STAMP_NAME      "blush-data-model-stamp"
            OUT_IDL_DIR     idl_dir
            OUT_CODEGEN_DIR codegen_dir
        )

        add_library(face_data_model INTERFACE)
        target_link_libraries(face_data_model INTERFACE FACE::Core)
        target_include_directories(face_data_model INTERFACE
            "${idl_dir}/cpp/face-model/include"
            "${codegen_dir}/face-model/include"
        )
    endif()

    if(ARG_OUT_LIBRARY)
        set(${ARG_OUT_LIBRARY} face_data_model PARENT_SCOPE)
    endif()
endfunction()
