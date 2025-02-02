#pragma once
#include <array>
#include <cstdint>
#include <type_traits>
#include <string_view>
#include <span>
#include <string>
#include <vector>
#include "syscalls_fwd.hpp"

template<typename T>
struct is_string
	: public std::disjunction<
		std::is_same<char *, typename std::decay<T>::type>,
		std::is_same<const char *, typename std::decay<T>::type>
> {};

template<typename T>
struct is_u32string
	: public std::disjunction<
		std::is_same<char32_t *, typename std::decay<T>::type>,
		std::is_same<const char32_t *, typename std::decay<T>::type>
> {};

template<class T>
struct is_stdstring : public std::is_same<T, std::basic_string<char>> {};

struct Object; struct Node; struct Node2D; struct Node3D; struct Array; struct Dictionary; union String;
#include "color.hpp"
#include "packed_array.hpp"
#include "vector.hpp"

struct Variant
{
	enum Type {
		NIL,

		// atomic types
		BOOL,
		INT,
		FLOAT,
		STRING,

		// math types
		VECTOR2,
		VECTOR2I,
		RECT2,
		RECT2I,
		VECTOR3,
		VECTOR3I,
		TRANSFORM2D,
		VECTOR4,
		VECTOR4I,
		PLANE,
		QUATERNION,
		AABB,
		BASIS,
		TRANSFORM3D,
		PROJECTION,

		// misc types
		COLOR,
		STRING_NAME,
		NODE_PATH,
		RID,
		OBJECT,
		CALLABLE,
		SIGNAL,
		DICTIONARY,
		ARRAY,

		// typed arrays
		PACKED_BYTE_ARRAY,
		PACKED_INT32_ARRAY,
		PACKED_INT64_ARRAY,
		PACKED_FLOAT32_ARRAY,
		PACKED_FLOAT64_ARRAY,
		PACKED_STRING_ARRAY,
		PACKED_VECTOR2_ARRAY,
		PACKED_VECTOR3_ARRAY,
		PACKED_COLOR_ARRAY,

		VARIANT_MAX
	};

	enum Operator {
		// comparison
		OP_EQUAL,
		OP_NOT_EQUAL,
		OP_LESS,
		OP_LESS_EQUAL,
		OP_GREATER,
		OP_GREATER_EQUAL,
		// mathematic
		OP_ADD,
		OP_SUBTRACT,
		OP_MULTIPLY,
		OP_DIVIDE,
		OP_NEGATE,
		OP_POSITIVE,
		OP_MODULE,
		OP_POWER,
		// bitwise
		OP_SHIFT_LEFT,
		OP_SHIFT_RIGHT,
		OP_BIT_AND,
		OP_BIT_OR,
		OP_BIT_XOR,
		OP_BIT_NEGATE,
		// logic
		OP_AND,
		OP_OR,
		OP_XOR,
		OP_NOT,
		// containment
		OP_IN,
		OP_MAX
	};

	Variant() { m_type = NIL; }
	Variant(const Variant &other);
	Variant(Variant &&other);
	~Variant() {}

	// Constructor for common types
	template <typename T>
	Variant(T value);

	Variant(const Array&);
	Variant(const Dictionary&);
	Variant(const String&);
	Variant(const Object&);
	Variant(const Node&);
	Variant(const Node2D&);
	Variant(const Node3D&);
	Variant(const PackedArray<uint8_t>&);
	Variant(const PackedArray<float>&);
	Variant(const PackedArray<double>&);
	Variant(const PackedArray<int32_t>&);
	Variant(const PackedArray<int64_t>&);
	Variant(const PackedArray<Vector2>&);
	Variant(const PackedArray<Vector3>&);
	Variant(const PackedArray<Color>&);

	// Constructor specifically the STRING_NAME type
	static Variant string_name(const std::string &name);

	// Create a new empty Array
	static Variant new_array();
	// Create a new Array from a vector of Variants
	static Variant from_array(const std::vector<Variant> &array);

	// Empty Dictionary constructor
	static Variant new_dictionary();

	// Conversion operators
	operator bool() const;
	operator int64_t() const;
	operator int32_t() const;
	operator int16_t() const;
	operator int8_t() const;
	operator uint64_t() const;
	operator uint32_t() const;
	operator uint16_t() const;
	operator uint8_t() const;
	operator double() const;
	operator float() const;
	operator std::string() const; // String for STRING and PACKED_BYTE_ARRAY
	operator std::u32string() const; // u32string for STRING, STRING_NAME
	operator String() const;
	operator Array() const;
	operator Dictionary() const;

	Object as_object() const;
	Node as_node() const;
	Node2D as_node2d() const;
	Node3D as_node3d() const;
	Array as_array() const;
	Dictionary as_dictionary() const;
	String as_string() const;
	std::string as_std_string() const;
	std::u32string as_std_u32string() const;
	PackedArray<uint8_t> as_byte_array() const;
	PackedArray<float> as_float32_array() const;
	PackedArray<double> as_float64_array() const;
	PackedArray<int32_t> as_int32_array() const;
	PackedArray<int64_t> as_int64_array() const;
	PackedArray<Vector2> as_vector2_array() const;
	PackedArray<Vector3> as_vector3_array() const;

	const Vector2 &v2() const;
	Vector2 &v2();
	const Vector2i &v2i() const;
	Vector2i &v2i();
	const Vector3 &v3() const;
	Vector3 &v3();
	const Vector3i &v3i() const;
	Vector3i &v3i();
	const Vector4 &v4() const;
	Vector4 &v4();
	const Vector4i &v4i() const;
	Vector4i &v4i();
	const Rect2 &r2() const;
	Rect2 &r2();
	const Rect2i &r2i() const;
	Rect2i &r2i();
	const Color &color() const;
	Color &color();

	operator Vector2() const { return v2(); }
	operator Vector2i() const { return v2i(); }
	operator Vector3() const { return v3(); }
	operator Vector3i() const { return v3i(); }
	operator Vector4() const { return v4(); }
	operator Vector4i() const { return v4i(); }
	operator Rect2() const { return r2(); }
	operator Rect2i() const { return r2i(); }
	operator Color() const { return color(); }

	void callp(std::string_view method, const Variant *args, int argcount, Variant &r_ret);

	template <typename... Args>
	Variant method_call(std::string_view method, Args&&... args);

	template <typename... Args>
	Variant call(Args... args);

	template <typename... Args>
	Variant operator ()(std::string_view method, Args... args);

	static void evaluate(const Operator &op, const Variant &a, const Variant &b, Variant &r_ret, bool &r_valid);

	Variant duplicate() const;

	Variant &operator=(const Variant &other);
	Variant &operator=(Variant &&other);
	bool operator==(const Variant &other) const;
	bool operator!=(const Variant &other) const;
	bool operator<(const Variant &other) const;

	Type get_type() const noexcept { return m_type; }
	unsigned get_internal_index() const noexcept { return v.i; }

private:
	Type m_type = NIL;
	union {
		int64_t  i;
		bool     b;
		double   f;
		Vector2  v2;
		Vector2i v2i;
		Vector3  v3;
		Vector3i v3i;
		Vector4  v4;
		Vector4i v4i;
		Rect2    r2;
		Rect2i   r2i;
		Color    color;
	} v;

	void internal_create_string(Type type, const std::string &value);
	void internal_create_u32string(Type type, const std::u32string &value);
	void internal_clone(const Variant &other);
	std::string internal_fetch_string() const;
	std::u32string internal_fetch_u32string() const;
};
static_assert(sizeof(Variant) == 24, "Variant size mismatch");

template <typename T>
inline Variant::Variant(T value)
{
	if constexpr (std::is_same_v<T, bool>) {
		m_type = BOOL;
		v.b = value;
	}
	else if constexpr (std::is_integral_v<T>) {
		m_type = INT;
		v.i = value;
	}
	else if constexpr (std::is_floating_point_v<T>) {
		m_type = FLOAT;
		v.f = value;
	}
	else if constexpr (std::is_same_v<T, Vector2>) {
		m_type = VECTOR2;
		v.v2 = value;
	}
	else if constexpr (std::is_same_v<T, Vector2i>) {
		m_type = VECTOR2I;
		v.v2i = value;
	}
	else if constexpr (std::is_same_v<T, Vector3>) {
		m_type = VECTOR3;
		v.v3 = value;
	}
	else if constexpr (std::is_same_v<T, Vector3i>) {
		m_type = VECTOR3I;
		v.v3i = value;
	}
	else if constexpr (std::is_same_v<T, Vector4>) {
		m_type = VECTOR4;
		v.v4 = value;
	}
	else if constexpr (std::is_same_v<T, Vector4i>) {
		m_type = VECTOR4I;
		v.v4i = value;
	}
	else if constexpr (std::is_same_v<T, Rect2>) {
		m_type = RECT2;
		v.r2 = value;
	}
	else if constexpr (std::is_same_v<T, Rect2i>) {
		m_type = RECT2I;
		v.r2i = value;
	}
	else if constexpr (std::is_same_v<T, Color>) {
		m_type = COLOR;
		v.color = value;
	}
	else if constexpr (is_u32string<T>::value || std::is_same_v<T, std::u32string>) {
		internal_create_u32string(STRING, value);
	}
	else if constexpr (is_string<T>::value || is_stdstring<T>::value || std::is_same_v<T, std::string>) {
		internal_create_string(STRING, value);
	}
	else
		static_assert(!std::is_same_v<T, T>, "Unsupported type");
}

#define Nil Variant()

inline Variant::Variant(const PackedArray<uint8_t> &array)
{
	m_type = PACKED_BYTE_ARRAY;
	v.i = array.get_variant_index();
}
inline Variant::Variant(const PackedArray<float> &array)
{
	m_type = PACKED_FLOAT32_ARRAY;
	v.i = array.get_variant_index();
}
inline Variant::Variant(const PackedArray<double> &array)
{
	m_type = PACKED_FLOAT64_ARRAY;
	v.i = array.get_variant_index();
}
inline Variant::Variant(const PackedArray<int32_t> &array)
{
	m_type = PACKED_INT32_ARRAY;
	v.i = array.get_variant_index();
}
inline Variant::Variant(const PackedArray<int64_t> &array)
{
	m_type = PACKED_INT64_ARRAY;
	v.i = array.get_variant_index();
}
inline Variant::Variant(const PackedArray<Vector2> &array)
{
	m_type = PACKED_VECTOR2_ARRAY;
	v.i = array.get_variant_index();
}
inline Variant::Variant(const PackedArray<Vector3> &array)
{
	m_type = PACKED_VECTOR3_ARRAY;
	v.i = array.get_variant_index();
}
inline Variant::Variant(const PackedArray<Color> &array)
{
	m_type = PACKED_COLOR_ARRAY;
	v.i = array.get_variant_index();
}

inline Variant Variant::string_name(const std::string &name) {
	Variant v;
	v.internal_create_string(STRING_NAME, name);
	return v;
}

inline Variant::operator bool() const
{
	if (m_type == BOOL || m_type == INT)
		return v.b;
	api_throw("std::bad_cast", "Failed to cast Variant to bool", this);
}

inline Variant::operator int64_t() const
{
	if (m_type == INT || m_type == FLOAT)
		return v.i;
	api_throw("std::bad_cast", "Failed to cast Variant to int64", this);
}

inline Variant::operator int32_t() const
{
	if (m_type == INT || m_type == FLOAT)
		return static_cast<int32_t>(v.i);
	api_throw("std::bad_cast", "Failed to cast Variant to int32", this);
}

inline Variant::operator int16_t() const
{
	if (m_type == INT || m_type == FLOAT)
		return static_cast<int16_t>(v.i);
	api_throw("std::bad_cast", "Failed to cast Variant to int16", this);
}

inline Variant::operator int8_t() const
{
	if (m_type == INT || m_type == FLOAT)
		return static_cast<int8_t>(v.i);
	api_throw("std::bad_cast", "Failed to cast Variant to int8", this);
}

inline Variant::operator uint64_t() const
{
	if (m_type == INT || m_type == FLOAT || m_type == OBJECT)
		return static_cast<uint64_t>(v.i);
	api_throw("std::bad_cast", "Failed to cast Variant to uint64", this);
}

inline Variant::operator uint32_t() const
{
	if (m_type == INT || m_type == FLOAT)
		return static_cast<uint32_t>(v.i);
	api_throw("std::bad_cast", "Failed to cast Variant to uint32", this);
}

inline Variant::operator uint16_t() const
{
	if (m_type == INT || m_type == FLOAT)
		return static_cast<uint16_t>(v.i);
	api_throw("std::bad_cast", "Failed to cast Variant to uint16", this);
}

inline Variant::operator uint8_t() const
{
	if (m_type == INT || m_type == FLOAT)
		return static_cast<uint8_t>(v.i);
	api_throw("std::bad_cast", "Failed to cast Variant to uint8", this);
}

inline Variant::operator double() const
{
	if (m_type == FLOAT)
		return v.f;
	if (m_type == INT)
		return static_cast<double>(v.i);
	api_throw("std::bad_cast", "Failed to cast Variant to double", this);
}

inline Variant::operator float() const
{
	if (m_type == FLOAT)
		return static_cast<float>(v.f);
	if (m_type == INT)
		return static_cast<float>(v.i);
	api_throw("std::bad_cast", "Failed to cast Variant to float", this);
}

inline Variant::operator std::string() const
{
	if (m_type == STRING || m_type == STRING_NAME || m_type == NODE_PATH || m_type == PACKED_BYTE_ARRAY)
		return internal_fetch_string();
	api_throw("std::bad_cast", "Failed to cast Variant to const std::string&", this);
}

inline Variant::operator std::u32string() const
{
	if (m_type == STRING || m_type == STRING_NAME)
		return internal_fetch_u32string();
	api_throw("std::bad_cast", "Failed to cast Variant to const std::u32string&", this);
}

inline std::string Variant::as_std_string() const {
	return static_cast<std::string>(*this);
}

inline std::u32string Variant::as_std_u32string() const {
	return static_cast<std::u32string>(*this);
}

inline const Vector2 &Variant::v2() const
{
	if (m_type == VECTOR2)
		return v.v2;
	api_throw("std::bad_cast", "Failed to cast Variant to Vector2", this);
}

inline Vector2 &Variant::v2()
{
	if (m_type == VECTOR2)
		return v.v2;
	api_throw("std::bad_cast", "Failed to cast Variant to Vector2", this);
}

inline const Vector2i &Variant::v2i() const
{
	if (m_type == VECTOR2I)
		return v.v2i;
	api_throw("std::bad_cast", "Failed to cast Variant to Vector2i", this);
}

inline Vector2i &Variant::v2i()
{
	if (m_type == VECTOR2I)
		return v.v2i;
	api_throw("std::bad_cast", "Failed to cast Variant to Vector2o", this);
}

inline const Vector3 &Variant::v3() const
{
	if (m_type == VECTOR3)
		return v.v3;
	api_throw("std::bad_cast", "Failed to cast Variant to Vector3", this);
}

inline Vector3 &Variant::v3()
{
	if (m_type == VECTOR3)
		return v.v3;
	api_throw("std::bad_cast", "Failed to cast Variant to Vector3", this);
}

inline const Vector3i &Variant::v3i() const
{
	if (m_type == VECTOR3I)
		return v.v3i;
	api_throw("std::bad_cast", "Failed to cast Variant to Vector3i", this);
}

inline Vector3i &Variant::v3i()
{
	if (m_type == VECTOR3I)
		return v.v3i;
	api_throw("std::bad_cast", "Failed to cast Variant to Vector3i", this);
}

inline const Vector4 &Variant::v4() const
{
	if (m_type == VECTOR4)
		return v.v4;
	api_throw("std::bad_cast", "Failed to cast Variant to Vector4", this);
}

inline Vector4 &Variant::v4()
{
	if (m_type == VECTOR4)
		return v.v4;
	api_throw("std::bad_cast", "Failed to cast Variant to Vector4", this);
}

inline const Vector4i &Variant::v4i() const
{
	if (m_type == VECTOR4I)
		return v.v4i;
	api_throw("std::bad_cast", "Failed to cast Variant to Vector4i", this);
}

inline Vector4i &Variant::v4i()
{
	if (m_type == VECTOR4I)
		return v.v4i;
	api_throw("std::bad_cast", "Failed to cast Variant to Vector4i", this);
}

inline const Rect2& Variant::r2() const
{
	if (m_type == RECT2)
		return v.r2;
	api_throw("std::bad_cast", "Failed to cast Variant to Rect2", this);
}

inline Rect2 &Variant::r2()
{
	if (m_type == RECT2)
		return v.r2;
	api_throw("std::bad_cast", "Failed to cast Variant to Rect2", this);
}

inline const Rect2i &Variant::r2i() const
{
	if (m_type == RECT2I)
		return v.r2i;
	api_throw("std::bad_cast", "Failed to cast Variant to Rect2i", this);
}

inline Rect2i &Variant::r2i()
{
	if (m_type == RECT2I)
		return v.r2i;
	api_throw("std::bad_cast", "Failed to cast Variant to Rect2i", this);
}

inline const Color &Variant::color() const
{
	if (m_type == COLOR)
		return v.color;
	api_throw("std::bad_cast", "Failed to cast Variant to Color", this);
}

inline Color &Variant::color()
{
	if (m_type == COLOR)
		return v.color;
	api_throw("std::bad_cast", "Failed to cast Variant to Color", this);
}

inline PackedArray<uint8_t> Variant::as_byte_array() const {
	if (m_type == PACKED_BYTE_ARRAY) {
		return PackedArray<uint8_t>::from_index(v.i);
	}
	api_throw("std::bad_cast", "Failed to cast Variant to PackedByteArray", this);
}

inline PackedArray<float> Variant::as_float32_array() const {
	if (m_type == PACKED_FLOAT32_ARRAY) {
		return PackedArray<float>::from_index(v.i);
	}
	api_throw("std::bad_cast", "Failed to cast Variant to PackedFloat32Array", this);
}

inline PackedArray<double> Variant::as_float64_array() const {
	if (m_type == PACKED_FLOAT64_ARRAY) {
		return PackedArray<double>::from_index(v.i);
	}
	api_throw("std::bad_cast", "Failed to cast Variant to PackedFloat64Array", this);
}

inline PackedArray<int32_t> Variant::as_int32_array() const {
	if (m_type == PACKED_INT32_ARRAY) {
		return PackedArray<int32_t>::from_index(v.i);
	}
	api_throw("std::bad_cast", "Failed to cast Variant to PackedInt32Array", this);
}

inline PackedArray<int64_t> Variant::as_int64_array() const {
	if (m_type == PACKED_INT64_ARRAY) {
		return PackedArray<int64_t>::from_index(v.i);
	}
	api_throw("std::bad_cast", "Failed to cast Variant to PackedInt64Array", this);
}

inline Variant::Variant(const Variant &other)
{
	m_type = other.m_type;
	if (m_type == STRING || m_type == PACKED_BYTE_ARRAY || m_type == NODE_PATH || m_type == STRING_NAME)
		this->internal_clone(other);
	else
		v = other.v;
}
inline Variant::Variant(Variant &&other)
{
	m_type = other.m_type;
	v = other.v;

	other.m_type = NIL;
}

inline Variant &Variant::operator=(const Variant &other) {
	m_type = other.m_type;
	if (m_type == STRING || m_type == PACKED_BYTE_ARRAY || m_type == NODE_PATH || m_type == STRING_NAME)
		this->internal_clone(other);
	else
		v = other.v;

	return *this;
}
inline Variant &Variant::operator=(Variant &&other) {
	m_type = other.m_type;
	v = other.v;

	other.m_type = NIL;
	return *this;
}

inline bool Variant::operator==(const Variant &other) const {
	bool valid = false;
	Variant result;
	evaluate(OP_EQUAL, *this, other, result, valid);
	return result.operator bool();
}
inline bool Variant::operator!=(const Variant &other) const {
	bool valid = false;
	Variant result;
	evaluate(OP_NOT_EQUAL, *this, other, result, valid);
	return result.operator bool();
}
inline bool Variant::operator<(const Variant &other) const {
	bool valid = false;
	Variant result;
	evaluate(OP_LESS, *this, other, result, valid);
	return result.operator bool();
}

template <typename... Args>
inline Variant Variant::method_call(std::string_view method, Args&&... args) {
	std::array<Variant, sizeof...(args)> vargs = { args... };
	Variant result;
	callp(method, vargs.data(), vargs.size(), result);
	return result;
}

template <typename... Args>
inline Variant Variant::call(Args... args) {
	std::array<Variant, sizeof...(args)> vargs = { args... };
	Variant result;
	callp("call", vargs.data(), vargs.size(), result);
	return result;
}

template <typename... Args>
inline Variant Variant::operator ()(std::string_view method, Args... args) {
	return method_call(method, args...);
}

/* Variant::callp() requires maximum performance, so implement using inline assembly */
inline void Variant::callp(std::string_view method, const Variant *args, int argcount, Variant &r_ret) {
	//sys_vcall(this, method.begin(), method.size(), args, argcount, r_ret);
	static constexpr int ECALL_VCALL = 501; // Call a method on a Variant
	register const Variant *object asm("a0") = this;
	register const char *method_ptr asm("a1") = method.begin();
	register size_t method_size asm("a2") = method.size();
	register const Variant *args_ptr asm("a3") = args;
	register size_t argcount_reg asm("a4") = argcount;
	register Variant *ret_ptr asm("a5") = &r_ret;
	register int syscall_number asm("a7") = ECALL_VCALL;

	asm volatile(
		"ecall"
		: "=m"(*ret_ptr)
		: "r"(object), "m"(*object), "r"(method_ptr), "r"(method_size), "m"(*method_ptr), "r"(args_ptr), "r"(argcount_reg), "r"(ret_ptr), "m"(*args_ptr), "r"(syscall_number)
	);
}

/* Call operators on simple wrapper objects */

template <typename... Args>
inline Variant Vector2::operator () (std::string_view method, Args&&... args) {
	return Variant(*this).method_call(method, std::forward<Args>(args)...);
}

template <typename... Args>
inline Variant Vector2i::operator () (std::string_view method, Args&&... args) {
	return Variant(*this).method_call(method, std::forward<Args>(args)...);
}

template <typename... Args>
inline Variant Vector3::operator () (std::string_view method, Args&&... args) {
	return Variant(*this).method_call(method, std::forward<Args>(args)...);
}

template <typename... Args>
inline Variant Vector3i::operator () (std::string_view method, Args&&... args) {
	return Variant(*this).method_call(method, std::forward<Args>(args)...);
}

template <typename... Args>
inline Variant Vector4::operator () (std::string_view method, Args&&... args) {
	return Variant(*this).method_call(method, std::forward<Args>(args)...);
}

template <typename... Args>
inline Variant Vector4i::operator () (std::string_view method, Args&&... args) {
	return Variant(*this).method_call(method, std::forward<Args>(args)...);
}

template <typename... Args>
inline Variant Rect2::operator () (std::string_view method, Args&&... args) {
	return Variant(*this).method_call(method, std::forward<Args>(args)...);
}

template <typename... Args>
inline Variant Rect2i::operator () (std::string_view method, Args&&... args) {
	return Variant(*this).method_call(method, std::forward<Args>(args)...);
}

template <typename... Args>
inline Variant Color::operator () (std::string_view method, Args&&... args) {
	return Variant(*this).method_call(method, std::forward<Args>(args)...);
}

/* Call operators on packed arrays */

template <typename T>
template <typename... Args>
inline Variant PackedArray<T>::operator () (std::string_view method, Args&&... args) {
	return Variant(*this).method_call(method, std::forward<Args>(args)...);
}
