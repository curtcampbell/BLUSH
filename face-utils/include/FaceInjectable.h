//
// FaceInjectable.h
//

#ifndef FACEINJECTABLE_H
#define FACEINJECTABLE_H

#include "FaceTypeTraits.h"
#include "UopBase.h"

/// FaceInjectable<DataType>
///
/// Mixin template that wires one FACE message type into a UoP class hierarchy
/// by implementing the type-specific Injectable interface for that type.
///
/// Template parameter:
///   DataType — a FACE::DM data model type for which
///              DECLARE_FACE_TYPE_TRAITS has been invoked.
///
/// The class inherits from two bases:
///   1. UopBase (virtual)   — shared across all FaceInjectable<T>
///                            specialisations in a concrete UoP;
///                            owns the Inject / GetInjected store.
///   2. InjectableInterface — the TypedTS-specific Injectable interface
///                            that the external executive uses to inject
///                            this connection type.
///
/// Multiple data types are supported by inheriting multiple specialisations:
///
///   class MyUoP : public FaceInjectable<NavModel::FlightPlan>,
///                 public FaceInjectable<NavModel::Waypoint>
///   { ... };
///
/// Virtual inheritance of UopBase guarantees a single shared base instance
/// regardless of how many FaceInjectable<T> specialisations are mixed in.
///
/// To retrieve an injected connection inside a concrete UoP, use
/// UopBase::GetInjected directly with the desired TypedTS pointer type:
///
///   auto* ts = this->template GetInjected<Traits<FlightPlan>::TypedTS*>(name);
///
template <typename DataType,
          typename InjectableInterface = typename Traits<DataType>::InjectableInterface,
          typename TypedTS             = typename Traits<DataType>::TypedTS>
class FaceInjectable
    : public virtual UopBase,
      public InjectableInterface
{
public:

    // -----------------------------------------------------------------------
    // Injectable interface implementation
    // -----------------------------------------------------------------------

    /// Typed Set_Reference — called by the external executive to inject
    /// the live TypedTS connection for DataType.
    ///
    /// interface_reference arrives already typed as TypedTS*; no cast is
    /// required.  The pointer is stored via UopBase::Inject and is
    /// subsequently retrievable via UopBase::GetInjected<TypedTS*>.
    void Set_Reference(
        const FACE::STRING_TYPE& interface_name,
        TypedTS*&                interface_reference,
        FACE::GUID_TYPE          id,
        FACE::RETURN_CODE_TYPE&  return_code) override
    {
        this->Inject(interface_name, interface_reference);
        return_code = FACE::RETURN_CODE_TYPE::NO_ERROR;
    }

protected:
    FaceInjectable() = default;
};

#endif // FACEINJECTABLE_H
