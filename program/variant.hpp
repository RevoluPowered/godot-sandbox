#pragma once
#include <array>
#include <cstdint>
#include <type_traits>
#include <stdexcept>

template<typename T>
struct is_string
	: public std::disjunction<
		std::is_same<char *, typename std::decay<T>::type>,
		std::is_same<const char *, typename std::decay<T>::type>
> {};

template<class T>
struct is_stdstring : public std::is_same<T, std::basic_string<char>> {};

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

	Variant() = default;
	~Variant();

	// Constructor for integers and floats
	template <typename T>
	Variant(T value)
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
		else if constexpr (is_string<T>::value || is_stdstring<T>::value) {
			m_type = STRING;
			v.s = new std::string(value);
		}
		else
			static_assert(!std::is_same_v<T, T>, "Unsupported type");
	}

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
	operator std::string() const;

	Variant &operator=(const Variant &other);
	Variant &operator=(Variant &&other);
	bool operator==(const Variant &other) const;
	bool operator!=(const Variant &other) const;
	bool operator<(const Variant &other) const;

	Type get_type() const noexcept { return m_type; }

private:
	Type m_type = NIL;
	union {
		bool    b;
		int64_t i;
		double  f;
		std::string *s;
	} v;
};

inline Variant::operator bool() const
{
	if (m_type == BOOL)
		return v.b;
	throw std::bad_cast();
}

inline Variant::operator int64_t() const
{
	if (m_type == INT)
		return v.i;
	throw std::bad_cast();
}

inline Variant::operator int32_t() const
{
	if (m_type == INT)
		return static_cast<int32_t>(v.i);
	throw std::bad_cast();
}

inline Variant::operator int16_t() const
{
	if (m_type == INT)
		return static_cast<int16_t>(v.i);
	throw std::bad_cast();
}

inline Variant::operator int8_t() const
{
	if (m_type == INT)
		return static_cast<int8_t>(v.i);
	throw std::bad_cast();
}

inline Variant::operator uint64_t() const
{
	if (m_type == INT)
		return static_cast<uint64_t>(v.i);
	throw std::bad_cast();
}

inline Variant::operator uint32_t() const
{
	if (m_type == INT)
		return static_cast<uint32_t>(v.i);
	throw std::bad_cast();
}

inline Variant::operator uint16_t() const
{
	if (m_type == INT)
		return static_cast<uint16_t>(v.i);
	throw std::bad_cast();
}

inline Variant::operator uint8_t() const
{
	if (m_type == INT)
		return static_cast<uint8_t>(v.i);
	throw std::bad_cast();
}

inline Variant::operator double() const
{
	if (m_type == FLOAT)
		return v.f;
	throw std::bad_cast();
}

inline Variant::operator float() const
{
	if (m_type == FLOAT)
		return static_cast<float>(v.f);
	throw std::bad_cast();
}

inline Variant::operator std::string() const
{
	if (m_type == STRING)
		return *v.s;
	throw std::bad_cast();
}

inline Variant::~Variant()
{
	if (m_type == STRING)
		delete v.s;
}

inline Variant &Variant::operator=(const Variant &other) {
	m_type = other.m_type;
	if (m_type == STRING)
		v.s = new std::string(*other.v.s);
	else
		v = other.v;

	return *this;
}

inline Variant &Variant::operator=(Variant &&other) {
	m_type = other.m_type;
	if (m_type == STRING)
		v.s = other.v.s;
	else
		v = other.v;

	other.m_type = NIL;
	return *this;
}