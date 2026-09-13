# BLUSHFaceCodegenPipeline.cmake
#
# Internal helper shared by BLUSHDataModelCodegen.cmake and BLUSHUopCodegen.cmake.
# Not part of BLUSH's public CMake API -- callers use blush_add_data_model_library()
# or blush_add_uop_libraries() instead.
#
# _blush_run_face_codegen_pipeline(
#     FACE_FILE       <path>
#     TEMPLATE_DIR    <dir>
#     OUTPUT_DIR      <dir>
#     STAMP_NAME      <name>          # distinct per caller so two callers sharing
#                                     # OUTPUT_DIR don't clobber each other's stamp
#     OUT_IDL_DIR     <var>           # set to <OUTPUT_DIR>/idl
#     OUT_CODEGEN_DIR <var>           # set to <OUTPUT_DIR>/codegen
# )
#
# Runs face-idl-gen (data-model IDL + C++ bindings) then face-codegen
# (TEMPLATE_DIR's *.vm templates + codegen.yaml) against FACE_FILE.
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
#    their old mtime and don't force unaffected targets to recompile. Stale
#    files left over from a previous generation (e.g. a UoP or connection
#    removed from the .face file) are pruned.

function(_blush_run_face_codegen_pipeline)
    set(oneValueArgs FACE_FILE TEMPLATE_DIR OUTPUT_DIR STAMP_NAME OUT_IDL_DIR OUT_CODEGEN_DIR)
    cmake_parse_arguments(ARG "" "${oneValueArgs}" "" ${ARGN})

    set(face_file    "${ARG_FACE_FILE}")
    set(template_dir "${ARG_TEMPLATE_DIR}")
    set(output_dir   "${ARG_OUTPUT_DIR}")

    # Reconfigure exactly when the model or templates change -- not on every build.
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${face_file}")
    file(GLOB_RECURSE template_inputs "${template_dir}/*")
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${template_inputs})

    find_program(FACE_IDL_GEN_EXECUTABLE face-idl-gen)
    find_program(FACE_CODEGEN_EXECUTABLE face-codegen)
    if(NOT FACE_IDL_GEN_EXECUTABLE OR NOT FACE_CODEGEN_EXECUTABLE)
        message(FATAL_ERROR
            "BLUSH code generation requires face-idl-gen and face-codegen on PATH "
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
    set(stamp_file    "${output_dir}/.${ARG_STAMP_NAME}")

    set(needs_regen TRUE)
    if(EXISTS "${stamp_file}" AND EXISTS "${codegen_dir}")
        file(READ "${stamp_file}" previous_hash)
        if(previous_hash STREQUAL content_hash)
            set(needs_regen FALSE)
        endif()
    endif()

    if(needs_regen)
        message(STATUS "BLUSH codegen (${ARG_STAMP_NAME}): regenerating from ${face_file}")

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
        # content changed, so without this step every generated target would
        # recompile on every regeneration even when unaffected by it.
        file(GLOB_RECURSE staged_files RELATIVE "${codegen_stage}" "${codegen_stage}/*")
        foreach(_rel ${staged_files})
            configure_file("${codegen_stage}/${_rel}" "${codegen_dir}/${_rel}" COPYONLY)
        endforeach()

        # Prune files left over from a previous generation that staging didn't
        # reproduce (e.g. a UoP, connection, or type removed from the .face file).
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

    if(ARG_OUT_IDL_DIR)
        set(${ARG_OUT_IDL_DIR} "${idl_dir}" PARENT_SCOPE)
    endif()
    if(ARG_OUT_CODEGEN_DIR)
        set(${ARG_OUT_CODEGEN_DIR} "${codegen_dir}" PARENT_SCOPE)
    endif()
endfunction()
