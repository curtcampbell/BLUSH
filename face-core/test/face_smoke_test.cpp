// face_smoke_test.cpp
// Compile-time verification of the FACE Technical Standard 3.2 C++ library.
// All checks are static_assert expressions that resolve at compile time.

#include "FACE.hpp"
#include <type_traits>

// --- READ_PAYLOAD_TYPE non-empty for all 13 services ----------------------
static_assert(sizeof(FACE::IOSS::Generic::IO_Service::READ_PAYLOAD_TYPE)           > 0, "Generic");
static_assert(sizeof(FACE::IOSS::Analog::IO_Service::READ_PAYLOAD_TYPE)             > 0, "Analog");
static_assert(sizeof(FACE::IOSS::Discrete::IO_Service::READ_PAYLOAD_TYPE)           > 0, "Discrete");
static_assert(sizeof(FACE::IOSS::Serial::IO_Service::READ_PAYLOAD_TYPE)             > 0, "Serial");
static_assert(sizeof(FACE::IOSS::ARINC429::IO_Service::READ_PAYLOAD_TYPE)           > 0, "ARINC429");
static_assert(sizeof(FACE::IOSS::ARINC825::IO_Service::READ_PAYLOAD_TYPE)           > 0, "ARINC825");
static_assert(sizeof(FACE::IOSS::M1553::IO_Service::READ_PAYLOAD_TYPE)              > 0, "M1553");
static_assert(sizeof(FACE::IOSS::M1553_Mk2::IO_Service::READ_PAYLOAD_TYPE)          > 0, "M1553_Mk2");
static_assert(sizeof(FACE::IOSS::MultiChannelAnalog::IO_Service::READ_PAYLOAD_TYPE) > 0, "MultiChannelAnalog");
static_assert(sizeof(FACE::IOSS::MultiChannelDiscrete::IO_Service::READ_PAYLOAD_TYPE) > 0, "MultiChannelDiscrete");
static_assert(sizeof(FACE::IOSS::Synchro::IO_Service::READ_PAYLOAD_TYPE)            > 0, "Synchro");
static_assert(sizeof(FACE::IOSS::PrecisionSynchro::IO_Service::READ_PAYLOAD_TYPE)   > 0, "PrecisionSynchro");
static_assert(sizeof(FACE::IOSS::I2C::IO_Service::READ_PAYLOAD_TYPE)                > 0, "I2C");

// --- Payload type correctness ---------------------------------------------
static_assert(sizeof(FACE::IOSS::Generic::IO_Service::READ_PAYLOAD_TYPE::payload)
           == sizeof(FACE::IOSS::Generic::ReadWriteBuffer), "Generic payload");

static_assert(sizeof(FACE::IOSS::ARINC825::IO_Service::READ_PAYLOAD_TYPE::payload)
           == sizeof(FACE::IOSS::ARINC825::DataFrameAbstraction), "ARINC825 payload");

static_assert(sizeof(FACE::IOSS::I2C::IO_Service::READ_PAYLOAD_TYPE::payload)
           == sizeof(FACE::IOSS::I2C::MASTER_COMMAND_TYPE), "I2C payload");

// --- I2C Combined_RW_IO_Service inherits IO_Service -----------------------
static_assert(std::is_base_of<
    FACE::IOSS::I2C::IO_Service,
    FACE::IOSS::I2C::Combined_RW_IO_Service>::value,
    "Combined_RW_IO_Service must derive from IO_Service");

// --- TSS Injectable namespaces accessible ---------------------------------
static_assert(sizeof(FACE::TSS::Base_Injectable::Injectable*)                            > 0, "TSS::Base_Injectable");
static_assert(sizeof(FACE::TSS::TypeAbstraction::TypeAbstractionTS_Injectable::Injectable*) > 0, "TypeAbstraction_Injectable");
static_assert(sizeof(FACE::TSS::CSP::CSP_Injectable::Injectable*)                       > 0, "CSP_Injectable");
static_assert(sizeof(FACE::TSS::TPM::TPMTS_Injectable::Injectable*)                     > 0, "TPM_Injectable");
static_assert(sizeof(FACE::TSS::Primitive_Marshalling_Injectable::Injectable*)          > 0, "Primitive_Marshalling_Injectable");
static_assert(sizeof(FACE::TSS::Serialization_Injectable::Injectable*)                  > 0, "Serialization_Injectable");

// --- LCM Injectable namespaces accessible ---------------------------------
static_assert(sizeof(FACE::LCM::Configurable::ConfigurableInstance_Injectable::Injectable*)   > 0, "Configurable_Injectable");
static_assert(sizeof(FACE::LCM::Connectable::ConnectableInstance_Injectable::Injectable*)     > 0, "Connectable_Injectable");
static_assert(sizeof(FACE::LCM::Initializable::InitializableInstance_Injectable::Injectable*) > 0, "Initializable_Injectable");

// --- IOSS Injectable namespaces accessible --------------------------------
static_assert(sizeof(FACE::IOSS::Generic::IO_Service_Injectable::Injectable*)           > 0, "Generic_Injectable");
static_assert(sizeof(FACE::IOSS::Analog::IO_Service_Injectable::Injectable*)            > 0, "Analog_Injectable");
static_assert(sizeof(FACE::IOSS::Discrete::IO_Service_Injectable::Injectable*)          > 0, "Discrete_Injectable");
static_assert(sizeof(FACE::IOSS::Serial::IO_Service_Injectable::Injectable*)            > 0, "Serial_Injectable");
static_assert(sizeof(FACE::IOSS::ARINC429::IO_Service_Injectable::Injectable*)          > 0, "ARINC429_Injectable");
static_assert(sizeof(FACE::IOSS::ARINC825::IO_Service_Injectable::Injectable*)          > 0, "ARINC825_Injectable");
static_assert(sizeof(FACE::IOSS::M1553::IO_Service_Injectable::Injectable*)             > 0, "M1553_Injectable");
static_assert(sizeof(FACE::IOSS::M1553_Mk2::IO_Service_Injectable::Injectable*)         > 0, "M1553_Mk2_Injectable");
static_assert(sizeof(FACE::IOSS::MultiChannelAnalog::IO_Service_Injectable::Injectable*)    > 0, "MultiChannelAnalog_Injectable");
static_assert(sizeof(FACE::IOSS::MultiChannelDiscrete::IO_Service_Injectable::Injectable*)  > 0, "MultiChannelDiscrete_Injectable");
static_assert(sizeof(FACE::IOSS::Synchro::IO_Service_Injectable::Injectable*)           > 0, "Synchro_Injectable");
static_assert(sizeof(FACE::IOSS::PrecisionSynchro::IO_Service_Injectable::Injectable*)  > 0, "PrecisionSynchro_Injectable");
// I2C Injectable injects Combined_RW_IO_Service (not IO_Service)
static_assert(sizeof(FACE::IOSS::I2C::Combined_RW_IO_Service_Injectable::Injectable*)   > 0, "I2C_Injectable");

// --- IOSS module-level sentinel values ------------------------------------
static_assert(FACE::IOSS::INVALID_CONNECTION_HANDLE ==
    static_cast<FACE::IOSS::CONNECTION_HANDLE_TYPE>(-1), "INVALID_CONNECTION_HANDLE");
static_assert(FACE::IOSS::IGNORED_CONNECTION_HANDLE ==
    static_cast<FACE::IOSS::CONNECTION_HANDLE_TYPE>(0),  "IGNORED_CONNECTION_HANDLE");

// --- Uninstantiated template modules accessible as C++ templates ----------
// (Convenience representations only -- not IDL-expanded namespaces)
typedef FACE::TSS::Typed<int>::TypedTS               CheckTyped;
typedef FACE::TSS::Extended<int,int>::TypedTS        CheckExtended;
typedef FACE::LCM::Stateful<int,int>::StatefulInstance CheckStateful;

int main() { return 0; }
