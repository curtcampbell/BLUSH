# sample-project/integration/CMakeLists.txt
#
# Links all seven UoP impl libraries, constructs one instance of each,
# drives it through the full FACE LCM sequence, and exits. See src/main.cpp
# and README.md.
#
# Charlie_Comp was excluded from this binary until the Traits<T> role-tag
# refactor (see session-docs/TRAITS-ROLE-CONFLICT.md and README.md's "Why
# Charlie_Comp is back in the integration binary"): it and Beta_Comp are the
# two roles of the same CLIENT_SERVER connection and used to need
# conflicting Traits<NewStockAgent_Request> specializations.

add_executable(sample_integration
    src/main.cpp
)
target_link_libraries(sample_integration PRIVATE
    Alpha_Comp_impl
    Beta_Comp_impl
    Charlie_Comp_impl
    CustomerEngagement_impl
    Delta_Comp_impl
    PointOfSaleTerminal_impl
    ReplenishmentService_impl
    sample_common
)
