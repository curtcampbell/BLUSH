#ifndef UOPBASE_HPP
#define UOPBASE_HPP

#include "FACE/TSS/Base_Injectable.hpp"
#include "FACE/Common.hpp"
#include <map>
#include <stdexcept>

/// Base class for FACE UoP implementations.
///
/// Implements FACE::TSS::Base_Injectable::Injectable and provides protected
/// helpers for subclasses to store and retrieve injected interface references.
///
/// The injection store (m_injectionData) is populated during startup via
/// Set_Reference calls and is treated as read-only for the remainder of
/// runtime — no synchronization is required for reads after initialisation.
///
/// Outer key: UopBase instance (this)
/// Inner key: interface_name supplied by the external executive
/// Value:     the injected interface pointer (DataType)
class UopBase : public virtual FACE::TSS::Base_Injectable::Injectable {
public:
    void Set_Reference(
        const FACE::STRING_TYPE&  interface_name,
        FACE::TSS::Base*&         interface_reference,
        FACE::GUID_TYPE           id,
        FACE::RETURN_CODE_TYPE&   return_code) override {

        Inject(interface_name, interface_reference);
        return_code = FACE::RETURN_CODE_TYPE::NOT_AVAILABLE;
    }


    /// Returns true if an injection of DataType with the given name exists
    /// for this UoP instance.
    template<typename DataType>
    [[nodiscard]] bool InjectedExists(const FACE::STRING_TYPE& name) const {
        const auto outerIt = m_injectionData<DataType>.find(this);
        if (outerIt == m_injectionData<DataType>.cend()) {
            return false;
        }
        return outerIt->second.find(name) != outerIt->second.cend();
    }

    /// Retrieves an injected DataType reference by name.
    ///
    /// @throws std::runtime_error if no entry exists for this instance or name.
    template<typename DataType>
    DataType GetInjected(const FACE::STRING_TYPE& name) const {
        const auto outerIt = m_injectionData<DataType>.find(this);
        if (outerIt == m_injectionData<DataType>.cend()) {
            throw std::runtime_error("UopBase::GetInjected - no injected entries for requested type");
        }
        const auto innerIt = outerIt->second.find(name);
        if (innerIt == outerIt->second.cend()) {
            throw std::runtime_error("UopBase::GetInjected - no entry for requested name");
        }
        return innerIt->second;
    }

protected:
    UopBase() = default;

    /// Stores an injected interface pointer by name.
    ///
    /// Called by subclass Set_Reference overrides during startup.
    /// The id parameter is provided for subclass validation if required
    /// but is not used as a storage key.
    template<typename DataType>
    void Inject(const FACE::STRING_TYPE& name, DataType injected) {
        m_injectionData<DataType>[this][name] = injected;
    }

private:
    template<typename DataType>
    static std::map<const UopBase*, std::map<FACE::STRING_TYPE, DataType>> m_injectionData;
};

template<typename DataType>
std::map<const UopBase*, std::map<FACE::STRING_TYPE, DataType>> UopBase::m_injectionData;

#endif //UOPBASE_HPP
