#ifndef FACETYPETRAITS_H
#define FACETYPETRAITS_H

//////////////////////////////////////////////////////////////////////////////////
/// Primary trait template — specialised per (model namespace, data type) pair
/// by DECLARE_FACE_TYPE_TRAITS / DECLARE_FACE_TYPE_TRAITS_DEFAULT below.
template <typename T>
struct Traits {};

//////////////////////////////////////////////////////////////////////////////////
/// DECLARE_FACE_TYPE_TRAITS(model_ns, data_type_name)
///
/// For uop:Template types whose generated struct lives inside a T_ wrapper
/// sub-namespace:
///
///   FACE::DM::<model_ns>::T_<data_type_name>::<data_type_name>  ← struct
///   FACE::TSS::<model_ns>::<data_type_name>::TypedTS             ← transport
///   FACE::TSS::<model_ns>::<data_type_name>_TypedTS_Injectable   ← injectable
///
/// The Injectable module is a SIBLING of <data_type_name> under <model_ns>
/// (not nested inside it) -- it is generated as its own top-level
/// "module ::FACE::Injectable<<data_type_name>::TypedTS>
/// <data_type_name>_TypedTS_Injectable;" instantiation (see
/// typed_ts_standard.vtl), mirroring the framework's own static
/// FACE::TSS::TPM::TPMTS_Injectable pattern (FACE/TSS/TPM_Injectable.idl)
/// rather than nesting one template instantiation inside another's expanded
/// body, which FACE-IDL-Parser's generic-module engine does not support.
///
/// Declares two Traits specialisations:
///
///   1. Traits<FACE::DM::model_ns::T_data_type_name::data_type_name>
///        TypedTS             — the TypedTS interface for this type
///        Read_Callback       — the read-callback typedef
///        DataType            — the data model type itself
///        InjectableInterface — same as spec 2's, below (needed here too:
///                              FaceInjectable<DataType>'s default template
///                              argument is Traits<DataType>::InjectableInterface,
///                              where DataType is THIS (DM-keyed) specialisation,
///                              not the TypedTS-keyed one)
///        Name[]              — runtime string name
///
///   2. Traits<FACE::TSS::model_ns::data_type_name::TypedTS>
///        DataType            — back-reference to the data model type
///        InjectableInterface — the <data_type_name>_TypedTS_Injectable::Injectable interface
///
/// model_ns must be the single namespace token that appears in both
/// FACE::DM::<model_ns> and FACE::TSS::<model_ns>
/// (e.g. CheckoutGateway_Templates, CORE_Templates).
///
/// Name[] is declared `static constexpr` (a C++17 inline variable), not
/// `static const` with a separate out-of-line definition: a DM type shared
/// by two UoPs gets this same DECLARE macro invoked once per UoP, and if
/// both UoPs' generated headers ever land in one translation unit (e.g. an
/// executable linking more than one UoP), identical inline definitions are
/// legal under the ODR where two out-of-line `.cpp` definitions would be a
/// duplicate-symbol link error. There is no paired DEFINE macro to call.
///
#define DECLARE_FACE_TYPE_TRAITS(model_ns, data_type_name)                                          \
    template <>                                                                                     \
    struct Traits<FACE::DM::model_ns::T_##data_type_name::data_type_name> {                         \
        typedef FACE::TSS::model_ns::data_type_name::TypedTS       TypedTS;                         \
        typedef FACE::TSS::model_ns::data_type_name::Read_Callback Read_Callback;                   \
        typedef FACE::DM::model_ns::T_##data_type_name::data_type_name DataType;                    \
        typedef FACE::TSS::model_ns::data_type_name##_TypedTS_Injectable::Injectable InjectableInterface; \
        static constexpr const char                                Name[] = #data_type_name;        \
    };                                                                                              \
                                                                                                    \
    template <>                                                                                     \
    struct Traits<FACE::TSS::model_ns::data_type_name::TypedTS> {                                   \
        typedef FACE::DM::model_ns::T_##data_type_name::data_type_name                        DataType; \
        typedef FACE::TSS::model_ns::data_type_name##_TypedTS_Injectable::Injectable InjectableInterface; \
    }

//////////////////////////////////////////////////////////////////////////////////
/// DECLARE_FACE_TYPE_TRAITS_COMPOSITE(model_ns, data_type_name)
///
/// For uop:CompositeTemplate types whose generated struct lives directly in
/// the model namespace with no T_ wrapper:
///
///   FACE::DM::<model_ns>::<data_type_name>   ← struct
///   FACE::TSS::<model_ns>::<data_type_name>::TypedTS  ← transport
///
/// Identical in structure to DECLARE_FACE_TYPE_TRAITS; the only difference
/// is that the DM path omits the T_ sub-namespace.
///
#define DECLARE_FACE_TYPE_TRAITS_COMPOSITE(model_ns, data_type_name)                                \
    template <>                                                                                     \
    struct Traits<FACE::DM::model_ns::data_type_name> {                                             \
        typedef FACE::TSS::model_ns::data_type_name::TypedTS       TypedTS;                         \
        typedef FACE::TSS::model_ns::data_type_name::Read_Callback Read_Callback;                   \
        typedef FACE::DM::model_ns::data_type_name                 DataType;                        \
        typedef FACE::TSS::model_ns::data_type_name##_TypedTS_Injectable::Injectable InjectableInterface; \
        static constexpr const char                                Name[] = #data_type_name;        \
    };                                                                                              \
                                                                                                    \
    template <>                                                                                     \
    struct Traits<FACE::TSS::model_ns::data_type_name::TypedTS> {                                   \
        typedef FACE::DM::model_ns::data_type_name                                          DataType; \
        typedef FACE::TSS::model_ns::data_type_name##_TypedTS_Injectable::Injectable InjectableInterface; \
    }

//////////////////////////////////////////////////////////////////////////////////
/// DECLARE_FACE_TYPE_TRAITS_REQRESP(model_ns, req_name, resp_name)
///
/// For a CLIENT_SERVER connection's request/response pair. Unlike a STANDARD
/// (pub/sub) connection, there is no 1:1 type<->TypedTS-module relationship:
/// FACE TS 3.2's Extended TypedTS instantiation produces a single COMBINED
/// module for the pair,
///
///   FACE::TSS::<model_ns>::<req_name>_<resp_name>::TypedTS
///
/// keyed by neither name alone (see typed_ts_extended.vtl). RequesterConnection
/// / ResponderConnection (face-utils) key entirely off the REQUEST type
/// (Traits<RequestType>::TypedTS / ::Read_Callback), so only the request type
/// gets a Traits specialisation here -- there is no reverse (TypedTS-keyed)
/// specialisation and no InjectableInterface, because CLIENT_SERVER
/// connections are excluded from FaceInjectable<DataType> (UoPBase.h.vm skips
/// them building its FaceInjectable<...> base-class list) -- CLIENT_SERVER
/// injection is not yet supported by this framework.
///
/// req_name must be a uop:Template (T_ wrapper); use the _COMPOSITE variant
/// below when the request is a uop:CompositeTemplate instead.
///
#define DECLARE_FACE_TYPE_TRAITS_REQRESP(model_ns, req_name, resp_name)                             \
    template <>                                                                                     \
    struct Traits<FACE::DM::model_ns::T_##req_name::req_name> {                                     \
        typedef FACE::TSS::model_ns::req_name##_##resp_name::TypedTS       TypedTS;                 \
        typedef FACE::TSS::model_ns::req_name##_##resp_name::Read_Callback Read_Callback;            \
        typedef FACE::DM::model_ns::T_##req_name::req_name                 DataType;                \
        static constexpr const char                                        Name[] = #req_name;      \
    }

/// DECLARE_FACE_TYPE_TRAITS_REQRESP_COMPOSITE(model_ns, req_name, resp_name)
///
/// Identical to DECLARE_FACE_TYPE_TRAITS_REQRESP; the only difference is that
/// the request's DM path omits the T_ sub-namespace (uop:CompositeTemplate
/// request type).
///
#define DECLARE_FACE_TYPE_TRAITS_REQRESP_COMPOSITE(model_ns, req_name, resp_name)                   \
    template <>                                                                                     \
    struct Traits<FACE::DM::model_ns::req_name> {                                                   \
        typedef FACE::TSS::model_ns::req_name##_##resp_name::TypedTS       TypedTS;                 \
        typedef FACE::TSS::model_ns::req_name##_##resp_name::Read_Callback Read_Callback;            \
        typedef FACE::DM::model_ns::req_name                               DataType;                \
        static constexpr const char                                        Name[] = #req_name;      \
    }

//////////////////////////////////////////////////////////////////////////////////
/// Convenience wrappers — single-model codebases can define DEFAULT_NAMESPACE
/// once and use these shorter forms instead of spelling out the namespace on
/// every declaration.
///
///   #define DEFAULT_NAMESPACE CheckoutGateway_Templates
///   DECLARE_FACE_TYPE_TRAITS_DEFAULT(BasketItem);
///   DECLARE_FACE_TYPE_TRAITS_COMPOSITE_DEFAULT(MyComposite);
///
#ifndef DEFAULT_NAMESPACE
#define DEFAULT_NAMESPACE UVC
#endif

#define DECLARE_FACE_TYPE_TRAITS_DEFAULT(data_type_name) \
    DECLARE_FACE_TYPE_TRAITS(DEFAULT_NAMESPACE, data_type_name)

#define DECLARE_FACE_TYPE_TRAITS_COMPOSITE_DEFAULT(data_type_name) \
    DECLARE_FACE_TYPE_TRAITS_COMPOSITE(DEFAULT_NAMESPACE, data_type_name)

#endif // FACETYPETRAITS_H
