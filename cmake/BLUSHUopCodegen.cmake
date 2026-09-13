# BLUSHUopCodegen.cmake
#
# blush_add_uop_libraries(
#     FACE_FILE     <path-to-.face-file>
#     [TEMPLATE_DIR <dir>]           # default: BLUSH_UOP_TEMPLATE_DIR
#     [OUTPUT_DIR   <dir>]           # default: <binary-dir>/uop-generated/<face-file-name>
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
# Every generated UoP CMakeLists.txt links PUBLIC against a `face_data_model`
# target, which this function defines once (FACE::Core + FACE::Util plus the
# generated data-model headers).
#
# Rebuild minimization:
#  - FACE_FILE and every file under TEMPLATE_DIR are registered via
#    CMAKE_CONFIGURE_DEPENDS, so a plain `cmake --build` only reconfigures
#    (and re-runs codegen) when the model or templates actually change.
#  - Even then, an MD5 stamp of those same inputs gates the two (JVM-based,
#    non-trivial-startup) generator invocations, so a reconfigure triggered
#    for an unrelated reason in the same project does not re-run them.
#  - Generated files are synced copy-if-different from a staging directory
#    into the real output tree, so files whose content didn't change keep
#    their old mtime and don't force unaffected UoPs to recompile. Stale
#    files left over from a previous generation (e.g. a UoP or connection
#    removed from the .face file) are pruned.

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

    # Reconfigure exactly when the model or templates change -- not on every build.
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${face_file}")
    file(GLOB_RECURSE template_inputs "${template_dir}/*")
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${template_inputs})

    find_program(FACE_IDL_GEN_EXECUTABLE face-idl-gen)
    find_program(FACE_CODEGEN_EXECUTABLE face-codegen)
    if(NOT FACE_IDL_GEN_EXECUTABLE OR NOT FACE_CODEGEN_EXECUTABLE)
        message(FATAL_ERROR
            "blush_add_uop_libraries requires face-idl-gen and face-codegen on PATH "
            "(https://github.com/curtcampbell/FACE-IDL-Parser)")
    endif()

    # Skip the generator invocations entirely when neither the model nor the
    # templates changed since the last successful run, regardless of why
    # this reconfigure happened.
    set(content_hash "")
    file(MD5 "${face_file}" _hash)
    string(APPEND content_hash "${_hash}")
    foreach(_f ${template_inputs})
        file(MD5 "${_f}" _hash)
        string(APPEND content_hash "${_hash}")
    endforeach()
    string(MD5 content_hash "${content_hash}")

    set(idl_dir       "${output_dir}/idl")
    set(codegen_stage "${output_dir}/.codegen-stage")
    set(codegen_dir   "${output_dir}/codegen")
    set(stamp_file    "${output_dir}/.blush-uop-stamp")

    set(needs_regen TRUE)
    if(EXISTS "${stamp_file}" AND EXISTS "${codegen_dir}")
        file(READ "${stamp_file}" previous_hash)
        if(previous_hash STREQUAL content_hash)
            set(needs_regen FALSE)
        endif()
    endif()

    if(needs_regen)
        message(STATUS "blush_add_uop_libraries: regenerating UoPs from ${face_file}")

        file(REMOVE_RECURSE "${idl_dir}" "${codegen_stage}")

        execute_process(
            COMMAND "${FACE_IDL_GEN_EXECUTABLE}" generate-tss-idl --cpp -o "${idl_dir}" "${face_file}"
            RESULT_VARIABLE idl_rc
            OUTPUT_VARIABLE idl_log
            ERROR_VARIABLE  idl_log
        )
        if(NOT idl_rc EQUAL 0)
            message(FATAL_ERROR "face-idl-gen failed for ${face_file}:\n${idl_log}")
        endif()

        execute_process(
            COMMAND "${FACE_CODEGEN_EXECUTABLE}" generate
                    -f "${face_file}"
                    -i "${idl_dir}/idl"
                    -t "${template_dir}"
                    -o "${codegen_stage}"
            RESULT_VARIABLE codegen_rc
            OUTPUT_VARIABLE codegen_log
            ERROR_VARIABLE  codegen_log
        )
        if(NOT codegen_rc EQUAL 0)
            message(FATAL_ERROR "face-codegen failed for ${face_file}:\n${codegen_log}")
        endif()

        # Sync staged output into the real tree copy-if-different: face-codegen
        # rewrites every file's mtime on every run regardless of whether its
        # content changed, so without this step every UoP would recompile on
        # every regeneration even when only one UoP's connections changed.
        file(GLOB_RECURSE staged_files RELATIVE "${codegen_stage}" "${codegen_stage}/*")
        foreach(_rel ${staged_files})
            configure_file("${codegen_stage}/${_rel}" "${codegen_dir}/${_rel}" COPYONLY)
        endforeach()

        # Prune files left over from a previous generation that staging didn't
        # reproduce (e.g. a UoP or connection removed from the .face file).
        if(EXISTS "${codegen_dir}")
            file(GLOB_RECURSE existing_files RELATIVE "${codegen_dir}" "${codegen_dir}/*")
            foreach(_rel ${existing_files})
                list(FIND staged_files "${_rel}" _idx)
                if(_idx EQUAL -1)
                    file(REMOVE "${codegen_dir}/${_rel}")
                endif()
            endforeach()
        endif()

        file(WRITE "${stamp_file}" "${content_hash}")
    endif()

    if(NOT TARGET face_data_model)
        add_library(face_data_model INTERFACE)
        target_link_libraries(face_data_model INTERFACE FACE::Core FACE::Util)
        target_include_directories(face_data_model INTERFACE
            "${idl_dir}/cpp/face-model/include"
            "${codegen_dir}/face-model/include"
        )
    endif()

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
