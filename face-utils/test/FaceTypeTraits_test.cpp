#include "FaceTypeTraits.h"

#include <gtest/gtest.h>
#include <type_traits>

// Fakes standing in for FACE-IDL-generated bindings, shaped exactly as
// DECLARE_FACE_TYPE_TRAITS / DECLARE_FACE_TYPE_TRAITS_COMPOSITE expect.

// --- uop:Template shape (T_<name> wrapper) ---------------------------------
namespace FACE { namespace DM { namespace TraitsTestModel { namespace T_Widget {
struct Widget { int id = 0; };
}}}}

namespace FACE { namespace TSS { namespace TraitsTestModel { namespace Widget {
class TypedTS {};
class Read_Callback {};
namespace TypedTS_Injectable { class Injectable {}; }
}}}}

DECLARE_FACE_TYPE_TRAITS(TraitsTestModel, Widget);
DEFINE_FACE_TYPE_TRAITS(TraitsTestModel, Widget);

// --- uop:CompositeTemplate shape (no T_ wrapper) ---------------------------
namespace FACE { namespace DM { namespace TraitsTestModel {
struct Gadget { int id = 0; };
}}}

namespace FACE { namespace TSS { namespace TraitsTestModel { namespace Gadget {
class TypedTS {};
class Read_Callback {};
namespace TypedTS_Injectable { class Injectable {}; }
}}}}

DECLARE_FACE_TYPE_TRAITS_COMPOSITE(TraitsTestModel, Gadget);
DEFINE_FACE_TYPE_TRAITS_COMPOSITE(TraitsTestModel, Gadget);

// --- DEFAULT_NAMESPACE convenience macros (resolve to UVC, uop:Template) --
namespace FACE { namespace DM { namespace UVC { namespace T_Sprocket {
struct Sprocket { int id = 0; };
}}}}

namespace FACE { namespace TSS { namespace UVC { namespace Sprocket {
class TypedTS {};
class Read_Callback {};
namespace TypedTS_Injectable { class Injectable {}; }
}}}}

DECLARE_FACE_TYPE_TRAITS_DEFAULT(Sprocket);
DEFINE_FACE_TYPE_TRAITS_DEFAULT(Sprocket);

namespace {

using WidgetDM      = FACE::DM::TraitsTestModel::T_Widget::Widget;
using WidgetTypedTS = FACE::TSS::TraitsTestModel::Widget::TypedTS;

static_assert(std::is_same<Traits<WidgetDM>::TypedTS, WidgetTypedTS>::value,
              "Traits<DM type>::TypedTS should resolve to the TSS TypedTS");
static_assert(std::is_same<Traits<WidgetDM>::Read_Callback,
                            FACE::TSS::TraitsTestModel::Widget::Read_Callback>::value,
              "Traits<DM type>::Read_Callback should resolve to the TSS Read_Callback");
static_assert(std::is_same<Traits<WidgetDM>::DataType, WidgetDM>::value,
              "Traits<DM type>::DataType should be the DM type itself");
static_assert(std::is_same<Traits<WidgetTypedTS>::DataType, WidgetDM>::value,
              "Traits<TypedTS>::DataType should point back to the DM type");
static_assert(std::is_same<Traits<WidgetTypedTS>::InjectableInterface,
                            FACE::TSS::TraitsTestModel::Widget::TypedTS_Injectable::Injectable>::value,
              "Traits<TypedTS>::InjectableInterface should resolve correctly");

using GadgetDM      = FACE::DM::TraitsTestModel::Gadget;
using GadgetTypedTS = FACE::TSS::TraitsTestModel::Gadget::TypedTS;

static_assert(std::is_same<Traits<GadgetDM>::TypedTS, GadgetTypedTS>::value,
              "Composite Traits<DM type>::TypedTS should resolve to the TSS TypedTS");
static_assert(std::is_same<Traits<GadgetDM>::DataType, GadgetDM>::value,
              "Composite Traits<DM type>::DataType should be the DM type itself");
static_assert(std::is_same<Traits<GadgetTypedTS>::DataType, GadgetDM>::value,
              "Composite Traits<TypedTS>::DataType should point back to the DM type");

using SprocketDM = FACE::DM::UVC::T_Sprocket::Sprocket;
static_assert(std::is_same<Traits<SprocketDM>::TypedTS,
                            FACE::TSS::UVC::Sprocket::TypedTS>::value,
              "DECLARE_FACE_TYPE_TRAITS_DEFAULT should expand under the UVC namespace");

} // namespace

TEST(FaceTypeTraitsTest, DeclareMacroDefinesRuntimeNameString) {
    EXPECT_STREQ(Traits<WidgetDM>::Name, "Widget");
}

TEST(FaceTypeTraitsTest, CompositeMacroDefinesRuntimeNameString) {
    EXPECT_STREQ(Traits<GadgetDM>::Name, "Gadget");
}

TEST(FaceTypeTraitsTest, DefaultNamespaceMacroDefinesRuntimeNameString) {
    EXPECT_STREQ(Traits<SprocketDM>::Name, "Sprocket");
}
