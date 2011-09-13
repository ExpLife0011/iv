#ifndef IV_LV5_JSVAL_FWD_H_
#define IV_LV5_JSVAL_FWD_H_
#include <cmath>
#include <algorithm>
#include "detail/cstdint.h"
#include "detail/type_traits.h"
#include "detail/cinttypes.h"
#include "byteorder.h"
#include "gc_template.h"
#include "enable_if.h"
#include "static_assert.h"
#include "platform_math.h"
#include "canonicalized_nan.h"
#include "conversions.h"
#include "none.h"
#include "utils.h"
#include "lv5/hint.h"
#include "lv5/jsstring_fwd.h"
#include "lv5/cell.h"

namespace iv {
namespace lv5 {

class Error;
class JSEnv;
class Context;
class JSReference;
class JSEnv;
class JSObject;

namespace detail {
template<std::size_t PointerSize, bool IsLittle>
struct Layout;

template<>
struct Layout<4, true> {
  union {
    uint64_t bytes_;
    struct {
      double as_;
    } number_;
    struct {
      union {
        bool boolean_;
        JSObject* object_;
        JSString* string_;
        JSReference* reference_;
        JSEnv* environment_;
        int32_t int32_;
        radio::Cell* cell_;
      } payload_;
      uint32_t tag_;
    } struct_;
  };
};

template<>
struct Layout<4, false> {
  union {
    uint64_t bytes_;
    struct {
      double as_;
    } number_;
    struct {
      uint32_t tag_;
      union {
        bool boolean_;
        JSObject* object_;
        JSString* string_;
        JSReference* reference_;
        JSEnv* environment_;
        int32_t int32_;
        radio::Cell* cell_;
      } payload_;
    } struct_;
  };
};

#if defined(IV_OS_SOLARIS)

template<>
struct Layout<8, true> {
  union {
    struct {
      double as_;
    } number_;
    struct {
      uint32_t padding_;
      uint32_t tag_;
      union {
        bool boolean_;
        JSObject* object_;
        JSString* string_;
        JSReference* reference_;
        JSEnv* environment_;
        JSVal* jsvalref_;
        int32_t int32_;
        radio::Cell* cell_;
      } payload_;
    } struct_;
  };
};

template<>
struct Layout<8, false> {
  union {
    struct {
      double as_;
    } number_;
    struct {
      uint32_t tag_;
      uint32_t padding_;
      union {
        bool boolean_;
        JSObject* object_;
        JSString* string_;
        JSReference* reference_;
        JSEnv* environment_;
        JSVal* jsvalref_;
        int32_t int32_;
        radio::Cell* cell_;
      } payload_;
    } struct_;
  };
};

#else

template<bool IsLittle>
struct Layout<8, IsLittle> {
  union {
    uint64_t bytes_;
    radio::Cell* cell_;
  };
};

#endif

struct JSTrueType { };
struct JSFalseType { };
struct JSNullType { };
struct JSUndefinedType { };
struct JSEmptyType { };
struct JSNaNType { };

struct Int32Tag { };
struct UInt32Tag { };
struct UInt16Tag { };
struct OtherCellTag { };

}  // namespace detail

static const detail::JSTrueType JSTrue = {};
static const detail::JSFalseType JSFalse = {};
static const detail::JSNullType JSNull = {};
static const detail::JSUndefinedType JSUndefined = {};
static const detail::JSEmptyType JSEmpty = {};
static const detail::JSNaNType JSNaN = {};

class JSVal {
 public:
  typedef JSVal this_type;
  typedef detail::Layout<
      core::Size::kPointerSize,
      core::kLittleEndian> value_type;

#if defined(__GNUC__) && (__GNUC__ >= 4) && (__GNUC_MINOR__ >= 3)
  IV_STATIC_ASSERT(std::is_pod<value_type>::value);
#endif

  JSVal() {
    set_undefined();
  }

  JSVal(const JSVal& rhs) : value_(rhs.value_) { }

  JSVal(double val)  // NOLINT
    : value_() {
    set_value(val);
    assert(IsNumber());
  }

  JSVal(JSObject* val)  // NOLINT
    : value_() {
    set_value(val);
    assert(IsObject());
  }

  JSVal(JSString* val)  // NOLINT
    : value_() {
    set_value(val);
    assert(IsString());
  }

  JSVal(JSReference* val)  // NOLINT
    : value_() {
    set_value(val);
    assert(IsReference());
  }

  JSVal(JSEnv* val)  // NOLINT
    : value_() {
    set_value(val);
    assert(IsEnvironment());
  }

  JSVal(detail::JSTrueType val)  // NOLINT
    : value_() {
    set_value(val);
    assert(IsBoolean());
  }

  JSVal(detail::JSFalseType val)  // NOLINT
    : value_() {
    set_value(val);
    assert(IsBoolean());
  }

  JSVal(detail::JSNullType val)  // NOLINT
    : value_() {
    set_null();
    assert(IsNull());
  }

  JSVal(detail::JSUndefinedType val)  // NOLINT
    : value_() {
    set_undefined();
    assert(IsUndefined());
  }

  JSVal(detail::JSEmptyType val)  // NOLINT
    : value_() {
    set_empty();
    assert(IsEmpty());
  }

  JSVal(detail::JSNaNType val)  // NOLINT
    : value_() {
    set_value(val);
    assert(IsNumber());
  }

  JSVal(const value_type& val)
      : value_(val) {  }  // NOLINT

  // prohibit like this,
  //
  //   JSVal val = true;
  //
  // if you want Boolean value, use JSTrue / JSFalse,
  //
  //   JSVal val = JSTrue;
  //
  // if bool constructor provided, following code doesn't raise compile error
  // (and any warning in GCC)
  //
  //   class A {
  //    public:
  //     A() { }
  //     explicit A(bool v) { }
  //     explicit A(A* v) { }
  //   };
  //
  //   int main() {
  //     const A a;
  //     const A b(&a);  // not compile error...
  //     return 0;
  //   }
  //
  //  so not provide implicit constructor with bool.
  template<typename T>
  JSVal(T val, typename enable_if<std::is_same<bool, T> >::type* = 0) {
    typedef std::is_same<bool, T> cond;
    IV_STATIC_ASSERT(!(cond::value));
  }

  inline JSString* TypeOf(Context* ctx) const;

  inline JSObject* GetPrimitiveProto(Context* ctx) const;

  inline JSObject* ToObject(Context* ctx, Error* e) const;

  inline JSString* ToString(Context* ctx, Error* e) const;

  inline double ToNumber(Context* ctx, Error* e) const;

  inline int32_t ToInt32(Context* ctx, Error* e) const;

  inline uint32_t ToUInt32(Context* ctx, Error* e) const;

  inline uint32_t GetUInt32() const;

  inline bool GetUInt32(uint32_t* result) const;

  inline JSVal ToPrimitive(Context* ctx, Hint::Object hint, Error* e) const;

  inline bool IsCallable() const;

  inline void CheckObjectCoercible(Error* e) const;

  inline bool ToBoolean(Error* e) const;

  // 32 / 64bit system separately
  inline bool IsEmpty() const;

  inline bool IsUndefined() const;

  inline bool IsNull() const;

  inline bool IsBoolean() const;

  inline bool IsInt32() const;

  inline bool IsNumber() const;

  inline bool IsCell() const;

  inline radio::Cell* cell() const;

  inline bool IsString() const;

  inline bool IsObject() const;

  inline bool IsReference() const;

  inline bool IsEnvironment() const;

  inline bool IsOtherCell() const;

  inline bool IsPrimitive() const;

  inline JSReference* reference() const;

  inline JSEnv* environment() const;

  inline JSString* string() const;

  inline JSObject* object() const;

  inline bool boolean() const;

  inline int32_t int32() const;

  inline double number() const;

  inline void set_value_int32(int32_t val);

  inline void set_value_uint32(uint32_t val);

  inline void set_value(double val);

  inline void set_value_cell(radio::Cell* val);

  inline void set_value(JSObject* val);

  inline void set_value(JSString* val);

  inline void set_value(JSReference* val);

  inline void set_value(JSEnv* val);

  inline void set_value(detail::JSTrueType val);

  inline void set_value(detail::JSFalseType val);

  inline void set_value(detail::JSNaNType val);

  inline void set_null();

  inline void set_undefined();

  inline void set_empty();

  static inline bool SameValue(const this_type& lhs, const this_type& rhs);

  static inline bool StrictEqual(const this_type& lhs, const this_type& rhs);

  // type specified factory functions
  static inline JSVal Bool(bool val) {
    if (val) {
      return JSTrue;
    } else {
      return JSFalse;
    }
  }

  template<typename T>
  static inline JSVal UInt32(
      T val,
      typename enable_if<std::is_same<uint32_t, T> >::type* = 0) {
    return JSVal(val, detail::UInt32Tag());
  }

  template<typename T>
  static inline JSVal UInt16(
      T val,
      typename enable_if<std::is_same<uint16_t, T> >::type* = 0) {
    return JSVal(val, detail::UInt16Tag());
  }

  template<typename T>
  static inline JSVal Int32(
      T val,
      typename enable_if<std::is_same<int32_t, T> >::type* = 0) {
    return JSVal(val, detail::Int32Tag());
  }

  static inline JSVal Cell(radio::Cell* cell) {
    return JSVal(cell, detail::OtherCellTag());
  }

  inline const value_type& Layout() const {
    return value_;
  }

  inline void swap(this_type& rhs) {
    using std::swap;
    swap(value_, rhs.value_);
  }

  inline friend void swap(this_type& lhs, this_type& rhs) {
    return lhs.swap(rhs);
  }

 private:
  JSVal(uint32_t val, detail::UInt32Tag dummy)
    : value_() {
    set_value_uint32(val);
    assert(IsNumber());
  }

  JSVal(uint16_t val, detail::UInt16Tag dummy)
    : value_() {
    set_value_int32(val);
    assert(IsInt32());
  }

  JSVal(int32_t val, detail::Int32Tag dummy)
    : value_() {
    set_value_int32(val);
    assert(IsInt32());
  }

  JSVal(radio::Cell* val, detail::OtherCellTag dummy)
    : value_() {
    set_value_cell(val);
    assert(IsCell());
  }

  value_type value_;
};

typedef GCVector<JSVal>::type JSVals;

} }  // namespace iv::lv5
#endif  // IV_LV5_JSVAL_FWD_H_