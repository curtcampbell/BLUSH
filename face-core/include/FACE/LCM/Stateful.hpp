// FACE/LCM/Stateful.hpp
// C++ representation of the IDL template module:
//   module Stateful<typename REQUESTED_STATE_VALUE_TYPE,
//                   typename REPORTED_STATE_VALUE_TYPE> { ... }
// FACE Technical Standard Edition 3.2
//
// +--------------------------------------------------------------------------+
// |  UNINSTANTIATED IDL TEMPLATE MODULE                                      |
// |                                                                          |
// |  The FACE 3.2 IDL defines module Stateful<typename REQUESTED,            |
// |  typename REPORTED> but provides NO instantiation.  Instantiation is    |
// |  left to the system integrator or UoC developer.                         |
// |                                                                          |
// |  CORRECT IDL-FIRST EXPANSION PROCESS (for reference):                   |
// |                                                                          |
// |  Step 1 - IDL instantiation (in your own .idl file):                    |
// |    module Stateful<MyApp::RequestedState, MyApp::ReportedState>          |
// |        MyApp_StatefulInstance;                                           |
// |                                                                          |
// |  Step 2 - Expand the template body substituting MyApp::RequestedState   |
// |    for REQUESTED_STATE_VALUE_TYPE and MyApp::ReportedState for           |
// |    REPORTED_STATE_VALUE_TYPE.                                            |
// |                                                                          |
// |  Step 3 - Apply C++ language binding to the expanded concrete IDL:      |
// |    namespace FACE { namespace LCM { namespace MyApp_StatefulInstance {   |
// |      class StatefulInstance {                                            |
// |        virtual void Query_State(ReportedState&, RETURN_CODE_TYPE&) = 0; |
// |        virtual void Request_State_Transition(                            |
// |          const RequestedState&, RETURN_CODE_TYPE&) = 0;                 |
// |      };                                                                  |
// |    }}}                                                                   |
// |                                                                          |
// |  The C++ template below is a convenience representation ONLY.           |
// +--------------------------------------------------------------------------+

#ifndef FACE_LCM_STATEFUL_HPP
#define FACE_LCM_STATEFUL_HPP

#include "../Common.hpp"

namespace FACE {
namespace LCM {

/// C++ template representation of IDL:
///   module Stateful<typename REQUESTED_STATE_VALUE_TYPE,
///                   typename REPORTED_STATE_VALUE_TYPE> { ... }
///
/// USAGE: This template is provided as a reference for the interface shape.
/// For a conformant FACE implementation, instantiate and expand the IDL per
/// the 3-step process described in the file header comment.
template<typename REQUESTED_STATE_VALUE_TYPE,
         typename REPORTED_STATE_VALUE_TYPE>
class Stateful {
public:
    /// LCM Stateful instance interface.
    class StatefulInstance {
    public:
        virtual ~StatefulInstance() {}

        /// Retrieve the instance's current state.
        ///
        /// @param current_state  Output: the current state value.
        /// @param return_code    Output: NO_ERROR on success.
        virtual void Query_State(
            REPORTED_STATE_VALUE_TYPE& current_state,
            RETURN_CODE_TYPE&          return_code) = 0;

        /// Request that the instance transition to a new state.
        ///
        /// @param new_state    The desired target state.
        /// @param return_code  Output: NO_ERROR if accepted;
        ///                     INVALID_MODE if the transition is not permitted.
        virtual void Request_State_Transition(
            const REQUESTED_STATE_VALUE_TYPE& new_state,
            RETURN_CODE_TYPE&                 return_code) = 0;
    };
};

} // namespace LCM
} // namespace FACE

#endif // FACE_LCM_STATEFUL_HPP
