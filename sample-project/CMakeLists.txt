# sample-project/CMakeLists.txt
#
# Demonstration project: one static library per Unit of Portability (UoP)
# defined in face-models/GROCERY_with_IM.face, each implemented by deriving
# from its generated <UoP>Base class, plus a single `integration` executable
# that links all of them, constructs one instance of each, and exits.
#
# This mirrors -- line for line, minus find_package(BLUSH) -- how a
# downstream application repo consumes BLUSH once installed:
#
#   find_package(BLUSH REQUIRED)
#   blush_add_uop_libraries(
#       FACE_FILE     path/to/YourModel.face
#       OUT_LIBRARIES MY_UOP_LIBS
#   )
#   # ... implement classes deriving from the generated <UoP>Base classes ...
#
# Here TEMPLATE_DIR is given explicitly because this runs in-tree, before
# BLUSH is installed (find_package(BLUSH) is what sets BLUSH_UOP_TEMPLATE_DIR
# for an out-of-tree consumer). No real TSS is injected anywhere in this
# example -- see README.md for what that means for the demo's output.

find_program(FACE_IDL_GEN_EXECUTABLE face-idl-gen)
find_program(FACE_CODEGEN_EXECUTABLE face-codegen)

if(NOT FACE_IDL_GEN_EXECUTABLE OR NOT FACE_CODEGEN_EXECUTABLE)
    message(STATUS
        "face-idl-gen/face-codegen not found on PATH -- skipping sample-project "
        "(see https://github.com/curtcampbell/FACE-IDL-Parser).")
    return()
endif()

blush_add_uop_libraries(
    FACE_FILE     "${CMAKE_CURRENT_SOURCE_DIR}/face-models/GROCERY_with_IM.face"
    TEMPLATE_DIR  "${CMAKE_SOURCE_DIR}/uop-generator/templates"
    OUT_LIBRARIES SAMPLE_UOP_LIBRARIES
)

add_subdirectory(common)
add_subdirectory(Alpha_Comp)
add_subdirectory(Beta_Comp)
add_subdirectory(Charlie_Comp)
add_subdirectory(CustomerEngagement)
add_subdirectory(Delta_Comp)
add_subdirectory(PointOfSaleTerminal)
add_subdirectory(ReplenishmentService)
add_subdirectory(integration)
