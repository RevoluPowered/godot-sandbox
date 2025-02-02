#pragma once

#include "variant.hpp"

class ArrayIterator;

struct Array {
	constexpr Array() {} // DON'T TOUCH
	Array(unsigned size);
	Array(const std::vector<Variant> &values);
	static Array Create(unsigned size = 0) { return Array(size); }

	operator Variant() const;

	// Array operations
	void append(const Variant &value) { push_back(value); }
	void push_back(const Variant &value);
	void push_front(const Variant &value);
	void pop_at(int idx);
	void pop_back();
	void pop_front();
	void insert(int idx, const Variant &value);
	void erase(int idx);
	void resize(int size);
	void clear();
	void sort();

	// Array access
	Variant operator[](int idx) const;
	Variant at(int idx) const { return (*this)[idx]; }

	std::vector<Variant> to_vector() const;

	// Array size
	int size() const;

	CREATE_METHOD(append_array);
	CREATE_METHOD(assign);
	CREATE_METHOD(duplicate);
	CREATE_METHOD(find);

	// Call methods on the Array
	template <typename... Args>
	Variant operator () (std::string_view method, Args&&... args);

	inline auto begin();
	inline auto end();
	inline auto rbegin();
	inline auto rend();

	static Array from_variant_index(unsigned idx) { Array a; a.m_idx = idx; return a; }
	unsigned get_variant_index() const noexcept { return m_idx; }

private:
	unsigned m_idx = -1;
};

// Array operations
inline Array::operator Variant() const {
	return Variant(*this);
}

inline Array Variant::as_array() const {
	if (m_type != ARRAY) {
		api_throw("std::bad_cast", "Failed to cast Variant to Array", this);
	}
	return Array::from_variant_index(v.i);
}

inline Variant::Variant(const Array& a) {
	m_type = ARRAY;
	v.i = a.get_variant_index();
}

inline Variant::operator Array() const {
	return as_array();
}

class ArrayIterator {
public:
	ArrayIterator(const Array &array, unsigned idx) : m_array(array), m_idx(idx) {}

	bool operator!=(const ArrayIterator &other) const { return m_idx != other.m_idx; }
	ArrayIterator &operator++() { m_idx++; return *this; }
	Variant operator*() const { return m_array[m_idx]; }

private:
	const Array m_array;
	unsigned m_idx;
};

inline auto Array::begin() {
	return ArrayIterator(*this, 0);
}
inline auto Array::end() {
	return ArrayIterator(*this, size());
}
inline auto Array::rbegin() {
	return ArrayIterator(*this, size() - 1);
}
inline auto Array::rend() {
	return ArrayIterator(*this, -1);
}

template <typename... Args>
inline Variant Array::operator () (std::string_view method, Args&&... args) {
	return Variant(*this).method_call(method, std::forward<Args>(args)...);
}
