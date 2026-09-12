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
///
/// Declares two Traits specialisations:
///
///   1. Traits<FACE::DM::model_ns::T_data_type_name::data_type_name>
///        TypedTS        — the TypedTS interface for this type
///        Read_Callback  — the read-callback typedef
///        DataType       — the data model type itself
///        Name[]         — runtime string name (defined by DEFINE_FACE_TYPE_TRAITS)
///
///   2. Traits<FACE::TSS::model_ns::data_type_name::TypedTS>
///        DataType            — back-reference to the data model type
///        InjectableInterface — the TypedTS_Injectable::Injectable interface
///
/// model_ns must be the single namespace token that appears in both
/// FACE::DM::<model_ns> and FACE::TSS::<model_ns>
/// (e.g. CheckoutGateway_Templates, CORE_Templates).
/// Place the DECLARE macro in a header; the paired DEFINE macro in one .cpp.
///
#define DECLARE_FACE_TYPE_TRAITS(model_ns, data_type_name)                                          \
    template <>                                                                                     \
    struct Traits<FACE::DM::model_ns::T_##data_type_name::data_type_name> {                         \
        typedef FACE::TSS::model_ns::data_type_name::TypedTS       TypedTS;                         \
        typedef FACE::TSS::model_ns::data_type_name::Read_Callback Read_Callback;                   \
        typedef FACE::DM::model_ns::T_##data_type_name::data_type_name DataType;                    \
        static const char                                          Name[];                          \
    };                                                                                              \
                                                                                                    \
    template <>                                                                                     \
    struct Traits<FACE::TSS::model_ns::data_type_name::TypedTS> {                                   \
        typedef FACE::DM::model_ns::T_##data_type_name::data_type_name              DataType;       \
        typedef FACE::TSS::model_ns::data_type_name::TypedTS_Injectable::Injectable InjectableInterface; \
    }

//////////////////////////////////////////////////////////////////////////////////
/// DEFINE_FACE_TYPE_TRAITS(model_ns, data_type_name)
///
/// Provides the out-of-line definition for the Name[] string member declared
/// by DECLARE_FACE_TYPE_TRAITS.  Place exactly once per type in a .cpp file.
///
#define DEFINE_FACE_TYPE_TRAITS(model_ns, data_type_name)                                           \
    const char Traits<FACE::DM::model_ns::T_##data_type_name::data_type_name>::Name[] = #data_type_name

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
        static const char                                          Name[];                          \
    };                                                                                              \
                                                                                                    \
    template <>                                                                                     \
    struct Traits<FACE::TSS::model_ns::data_type_name::TypedTS> {                                   \
        typedef FACE::DM::model_ns::data_type_name                                   DataType;      \
        typedef FACE::TSS::model_ns::data_type_name::TypedTS_Injectable::Injectable  InjectableInterface; \
    }

//////////////////////////////////////////////////////////////////////////////////
/// DEFINE_FACE_TYPE_TRAITS_COMPOSITE(model_ns, data_type_name)
///
/// Provides the out-of-line Name[] definition for a CompositeTemplate type.
/// Place exactly once per type in a .cpp file.
///
#define DEFINE_FACE_TYPE_TRAITS_COMPOSITE(model_ns, data_type_name)                                 \
    const char Traits<FACE::DM::model_ns::data_type_name>::Name[] = #data_type_name

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

#define DEFINE_FACE_TYPE_TRAITS_DEFAULT(data_type_name) \
    DEFINE_FACE_TYPE_TRAITS(DEFAULT_NAMESPACE, data_type_name)

#define DECLARE_FACE_TYPE_TRAITS_COMPOSITE_DEFAULT(data_type_name) \
    DECLARE_FACE_TYPE_TRAITS_COMPOSITE(DEFAULT_NAMESPACE, data_type_name)

#define DEFINE_FACE_TYPE_TRAITS_COMPOSITE_DEFAULT(data_type_name) \
    DEFINE_FACE_TYPE_TRAITS_COMPOSITE(DEFAULT_NAMESPACE, data_type_name)

#endif // FACETYPETRAITS_H
