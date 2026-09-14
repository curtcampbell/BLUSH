# uop-generator/CMakeLists.txt
#
# uop-generator itself produces no library -- its deliverable is templates/
# (fed to face-codegen) plus the blush_add_uop_libraries() CMake function
# (cmake/BLUSHUopCodegen.cmake) that downstream FACE-aligned application
# repos call, with their own .face file, after find_package(BLUSH).
#
# examples/GROCERY_with_IM.face is exercised in-tree by sample-project/,
# which both builds the generated <UoP>_lib targets and demonstrates
# deriving an implementation class from each -- a strictly better smoke
# test of these templates than instantiating the libraries here with
# nothing consuming them. See sample-project/CMakeLists.txt.
