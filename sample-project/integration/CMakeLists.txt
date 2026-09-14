# sample-project/integration/CMakeLists.txt
#
# Links six of the seven UoP impl libraries, constructs one instance of
# each, drives it through the full FACE LCM sequence, and exits. See
# src/main.cpp and README.md.
#
# Charlie_Comp is deliberately NOT linked in here -- see "Why Charlie_Comp
# isn't in the integration binary" in README.md. It still builds as its own
# Charlie_Comp_impl library (Charlie_Comp/CMakeLists.txt); it's just not
# combined with the other six into one process.

add_executable(sample_integration
    src/main.cpp
)
target_link_libraries(sample_integration PRIVATE
    Alpha_Comp_impl
    Beta_Comp_impl
    CustomerEngagement_impl
    Delta_Comp_impl
    PointOfSaleTerminal_impl
    ReplenishmentService_impl
    sample_common
)
