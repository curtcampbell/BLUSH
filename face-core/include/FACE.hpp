// FACE.hpp
// Master include header for the FACE Technical Standard 3.2 C++ Core Library.
//
// Include this single header to gain access to all FACE interfaces,
// or include individual sub-headers as needed to minimize compilation
// dependencies.
//
// +-------------------------------------------------------------------------+
// |  IDL TEMPLATE MODULE EXPANSION APPROACH                                 |
// |                                                                         |
// |  IDL template modules are expanded at the IDL level (not as C++         |
// |  templates) by substituting concrete type arguments throughout the       |
// |  template body, then applying the C++ language binding to the           |
// |  resulting concrete IDL.  Each instantiation produces a distinct         |
// |  concrete namespace.                                                     |
// |                                                                         |
// |  Three IDL template modules in this library have NO instantiation in    |
// |  the FACE 3.2 standard IDL files and are therefore represented here     |
// |  as C++ templates (convenience only -- not a conformant expansion):     |
// |    FACE::TSS::Typed<T>              (TypedTS.hpp)                        |
// |    FACE::TSS::Extended<T,R>         (Extended.hpp)                       |
// |    FACE::LCM::Stateful<Req,Rep>     (Stateful.hpp)                       |
// |  See the individual file headers for the 3-step IDL expansion process.  |
// +-------------------------------------------------------------------------+
//
// Namespace structure (concrete namespaces after IDL expansion):
//
//   FACE                                    Common types, Configuration
//   FACE::Configuration_Injectable          Injectable for Configuration
//
//   FACE::TSS                               Transport Services Segment
//   FACE::TSS::Base_Injectable              Injectable for TSS::Base
//   FACE::TSS::TypeAbstraction::TypeAbstractionTS_Injectable
//   FACE::TSS::CSP::CSP_Injectable          Injectable for TSS::CSP
//   FACE::TSS::TPM::TPMTS_Injectable        Injectable for TSS::TPM::TPMTS
//   FACE::TSS::Primitive_Marshalling_Injectable
//   FACE::TSS::Serialization_Injectable
//   FACE::TSS::MT_Utility_IF_Lookup_Injectable
//
//   FACE::LCM::Configurable::ConfigurableInstance_Injectable
//   FACE::LCM::Connectable::ConnectableInstance_Injectable
//   FACE::LCM::Initializable::InitializableInstance_Injectable
//
//   FACE::IOSS                              I/O Services Segment (shared types)
//   FACE::IOSS::Generic                     Generic I/O (incl. IO_Service)
//   FACE::IOSS::Generic::IO_Service_Injectable
//   FACE::IOSS::Discrete                    Discrete I/O
//   FACE::IOSS::Discrete::IO_Service_Injectable
//   FACE::IOSS::Analog                      Analog I/O
//   FACE::IOSS::Analog::IO_Service_Injectable
//   FACE::IOSS::Serial                      Serial I/O
//   FACE::IOSS::Serial::IO_Service_Injectable
//   FACE::IOSS::ARINC429                    ARINC 429 I/O
//   FACE::IOSS::ARINC429::IO_Service_Injectable
//   FACE::IOSS::ARINC825                    ARINC 825 (CAN) I/O
//   FACE::IOSS::ARINC825::IO_Service_Injectable
//   FACE::IOSS::M1553                       MIL-STD-1553 I/O (legacy)
//   FACE::IOSS::M1553::IO_Service_Injectable
//   FACE::IOSS::M1553_Mk2                   MIL-STD-1553 I/O (structured Mk2)
//   FACE::IOSS::M1553_Mk2::IO_Service_Injectable
//   FACE::IOSS::MultiChannelAnalog          Multi-channel analog I/O
//   FACE::IOSS::MultiChannelAnalog::IO_Service_Injectable
//   FACE::IOSS::MultiChannelDiscrete        Multi-channel discrete I/O
//   FACE::IOSS::MultiChannelDiscrete::IO_Service_Injectable
//   FACE::IOSS::Synchro                     Synchro angular I/O (32-bit)
//   FACE::IOSS::Synchro::IO_Service_Injectable
//   FACE::IOSS::PrecisionSynchro            Synchro angular I/O (64-bit)
//   FACE::IOSS::PrecisionSynchro::IO_Service_Injectable
//   FACE::IOSS::I2C                         I2C master/slave I/O
//   FACE::IOSS::I2C::Combined_RW_IO_Service_Injectable

#ifndef FACE_HPP
#define FACE_HPP

// ---- Core ----------------------------------------------------------------
#include "FACE/Common.hpp"
#include "FACE/Injectable.hpp"
#include "FACE/Configuration.hpp"
#include "FACE/Configuration_Injectable.hpp"

// ---- Transport Services Segment (TSS) ------------------------------------
#include "FACE/TSS/Common.hpp"
#include "FACE/TSS/Base.hpp"
#include "FACE/TSS/Base_Injectable.hpp"
#include "FACE/TSS/TypedTS.hpp"
#include "FACE/TSS/Extended.hpp"
#include "FACE/TSS/TypeAbstraction.hpp"
#include "FACE/TSS/TypeAbstraction_Injectable.hpp"
#include "FACE/TSS/CSP.hpp"
#include "FACE/TSS/CSP_Injectable.hpp"
#include "FACE/TSS/TPM.hpp"
#include "FACE/TSS/TPM_Injectable.hpp"
#include "FACE/TSS/Primitive_Marshalling.hpp"
#include "FACE/TSS/Primitive_Marshalling_Injectable.hpp"
#include "FACE/TSS/Serialization.hpp"
#include "FACE/TSS/Serialization_Injectable.hpp"
#include "FACE/TSS/Message_Type_Utilities.hpp"
#include "FACE/TSS/MT_Utility_Injectable.hpp"

// ---- Life-Cycle Management (LCM) -----------------------------------------
#include "FACE/LCM/Configurable.hpp"
#include "FACE/LCM/Configurable_Injectable.hpp"
#include "FACE/LCM/Connectable.hpp"
#include "FACE/LCM/Connectable_Injectable.hpp"
#include "FACE/LCM/Initializable.hpp"
#include "FACE/LCM/Initializable_Injectable.hpp"
#include "FACE/LCM/Stateful.hpp"

// ---- I/O Services Segment (IOSS) -----------------------------------------
#include "FACE/IOSS/IOS.hpp"
#include "FACE/IOSS/Generic.hpp"
#include "FACE/IOSS/Generic_Injectable.hpp"
#include "FACE/IOSS/Discrete.hpp"
#include "FACE/IOSS/Discrete_Injectable.hpp"
#include "FACE/IOSS/Analog.hpp"
#include "FACE/IOSS/Analog_Injectable.hpp"
#include "FACE/IOSS/Serial.hpp"
#include "FACE/IOSS/Serial_Injectable.hpp"
#include "FACE/IOSS/ARINC429.hpp"
#include "FACE/IOSS/ARINC429_Injectable.hpp"
#include "FACE/IOSS/ARINC825.hpp"
#include "FACE/IOSS/ARINC825_Injectable.hpp"
#include "FACE/IOSS/M1553.hpp"
#include "FACE/IOSS/M1553_Injectable.hpp"
#include "FACE/IOSS/M1553_Mk2.hpp"
#include "FACE/IOSS/M1553_Mk2_Injectable.hpp"
#include "FACE/IOSS/MultiChannelAnalog.hpp"
#include "FACE/IOSS/MultiChannelAnalog_Injectable.hpp"
#include "FACE/IOSS/MultiChannelDiscrete.hpp"
#include "FACE/IOSS/MultiChannelDiscrete_Injectable.hpp"
#include "FACE/IOSS/Synchro.hpp"
#include "FACE/IOSS/Synchro_Injectable.hpp"
#include "FACE/IOSS/PrecisionSynchro.hpp"
#include "FACE/IOSS/PrecisionSynchro_Injectable.hpp"
#include "FACE/IOSS/I2C.hpp"
#include "FACE/IOSS/I2C_Injectable.hpp"

#endif // FACE_HPP
