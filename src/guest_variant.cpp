#include "guest_datatypes.h"

#include <godot_cpp/variant/utility_functions.hpp>
#include <libriscv/util/crc32.hpp>

Variant GuestVariant::toVariant(const Sandbox &emu) const {
	switch (type) {
		case Variant::NIL:
			return Variant();
		case Variant::BOOL:
			return v.b;
		case Variant::INT:
			return v.i;
		case Variant::FLOAT:
			return v.f;

		case Variant::VECTOR2:
			return Variant{ godot::Vector2(v.v2f[0], v.v2f[1]) };
		case Variant::VECTOR2I:
			return Variant{ godot::Vector2i(v.v2i[0], v.v2i[1]) };
		case Variant::RECT2:
			return Variant{ godot::Rect2(v.v4f[0], v.v4f[1], v.v4f[2], v.v4f[3]) };
		case Variant::RECT2I:
			return Variant{ godot::Rect2i(v.v4i[0], v.v4i[1], v.v4i[2], v.v4i[3]) };
		case Variant::VECTOR3:
			return Variant{ godot::Vector3(v.v3f[0], v.v3f[1], v.v3f[2]) };
		case Variant::VECTOR3I:
			return Variant{ godot::Vector3i(v.v3i[0], v.v3i[1], v.v3i[2]) };
		case Variant::VECTOR4:
			return Variant{ godot::Vector4(v.v4f[0], v.v4f[1], v.v4f[2], v.v4f[3]) };
		case Variant::VECTOR4I:
			return Variant{ godot::Vector4i(v.v4i[0], v.v4i[1], v.v4i[2], v.v4i[3]) };
		case Variant::COLOR:
			return Variant{ godot::Color(v.v4f[0], v.v4f[1], v.v4f[2], v.v4f[3]) };

		case Variant::OBJECT: {
			godot::Object *obj = (godot::Object *)uintptr_t(v.i);
			if (emu.is_scoped_object(obj))
				return Variant{ obj };
			else
				throw std::runtime_error("GuestVariant::toVariant(): Object is not known/scoped");
		}

		case Variant::TRANSFORM2D:
		case Variant::PLANE:
		case Variant::QUATERNION:
		case Variant::AABB:
		case Variant::BASIS:
		case Variant::TRANSFORM3D:
		case Variant::PROJECTION:

		case Variant::DICTIONARY:
		case Variant::ARRAY:
		case Variant::CALLABLE:
		case Variant::STRING:
		case Variant::STRING_NAME:
		case Variant::NODE_PATH:
		case Variant::PACKED_BYTE_ARRAY:
		case Variant::PACKED_FLOAT32_ARRAY:
		case Variant::PACKED_FLOAT64_ARRAY:
		case Variant::PACKED_INT32_ARRAY:
		case Variant::PACKED_INT64_ARRAY:
		case Variant::PACKED_VECTOR2_ARRAY:
		case Variant::PACKED_VECTOR3_ARRAY:
		case Variant::PACKED_COLOR_ARRAY:
		case Variant::PACKED_STRING_ARRAY: {
			if (std::optional<const Variant *> v = emu.get_scoped_variant(this->v.i)) {
				return *v.value();
			} else
				throw std::runtime_error("GuestVariant::toVariant(): Dictionary/Array/Callable is not known/scoped");
		}
		default:
			ERR_PRINT(("GuestVariant::toVariant(): Unsupported type: " + std::to_string(type)).c_str());
			return Variant();
	}
}

const Variant *GuestVariant::toVariantPtr(const Sandbox &emu) const {
	switch (type) {
		case Variant::STRING:
		case Variant::TRANSFORM2D:
		case Variant::PLANE:
		case Variant::QUATERNION:
		case Variant::AABB:
		case Variant::BASIS:
		case Variant::TRANSFORM3D:
		case Variant::PROJECTION:
		case Variant::DICTIONARY:
		case Variant::ARRAY:
		case Variant::CALLABLE:
		case Variant::STRING_NAME:
		case Variant::NODE_PATH:
		case Variant::PACKED_BYTE_ARRAY:
		case Variant::PACKED_FLOAT32_ARRAY:
		case Variant::PACKED_FLOAT64_ARRAY:
		case Variant::PACKED_INT32_ARRAY:
		case Variant::PACKED_INT64_ARRAY:
		case Variant::PACKED_VECTOR2_ARRAY:
		case Variant::PACKED_VECTOR3_ARRAY:
		case Variant::PACKED_COLOR_ARRAY:
		case Variant::PACKED_STRING_ARRAY: {
			if (std::optional<const Variant *> v = emu.get_scoped_variant(this->v.i))
				return v.value();
			else
				throw std::runtime_error("GuestVariant::toVariantPtr(): Callable is not known/scoped");
		}
		default:
			throw std::runtime_error("Don't use toVariantPtr() on unsupported type: " + std::to_string(type));
	}
}

void GuestVariant::set_object(Sandbox &emu, godot::Object *obj) {
	emu.add_scoped_object(obj);
	this->type = Variant::OBJECT;
	this->v.i = (uintptr_t)obj;
}

void GuestVariant::set(Sandbox &emu, const Variant &value, bool implicit_trust) {
	this->type = value.get_type();

	switch (this->type) {
		case Variant::NIL:
			break;
		case Variant::BOOL:
			this->v.b = value;
			break;
		case Variant::INT:
			this->v.i = value;
			break;
		case Variant::FLOAT:
			this->v.f = value;
			break;

		case Variant::VECTOR2: {
			godot::Vector2 vec = value.operator godot::Vector2();
			this->v.v2f[0] = vec.x;
			this->v.v2f[1] = vec.y;
			break;
		}
		case Variant::VECTOR2I: {
			godot::Vector2i vec = value.operator godot::Vector2i();
			this->v.v2i[0] = vec.x;
			this->v.v2i[1] = vec.y;
			break;
		}
		case Variant::RECT2: {
			Rect2 rect = value.operator godot::Rect2();
			this->v.v4f[0] = rect.position[0];
			this->v.v4f[1] = rect.position[1];
			this->v.v4f[2] = rect.size[0];
			this->v.v4f[3] = rect.size[1];
			break;
		}
		case Variant::RECT2I: {
			Rect2i rect = value.operator godot::Rect2i();
			this->v.v4i[0] = rect.position[0];
			this->v.v4i[1] = rect.position[1];
			this->v.v4i[2] = rect.size[0];
			this->v.v4i[3] = rect.size[1];
			break;
		}
		case Variant::VECTOR3: {
			godot::Vector3 vec = value.operator godot::Vector3();
			this->v.v3f[0] = vec.x;
			this->v.v3f[1] = vec.y;
			this->v.v3f[2] = vec.z;
			break;
		}
		case Variant::VECTOR3I: {
			godot::Vector3i vec = value.operator godot::Vector3i();
			this->v.v3i[0] = vec.x;
			this->v.v3i[1] = vec.y;
			this->v.v3i[2] = vec.z;
			break;
		}
		case Variant::VECTOR4: {
			godot::Vector4 vec = value.operator godot::Vector4();
			this->v.v4f[0] = vec.x;
			this->v.v4f[1] = vec.y;
			this->v.v4f[2] = vec.z;
			this->v.v4f[3] = vec.w;
			break;
		}
		case Variant::VECTOR4I: {
			godot::Vector4i vec = value.operator godot::Vector4i();
			this->v.v4i[0] = vec.x;
			this->v.v4i[1] = vec.y;
			this->v.v4i[2] = vec.z;
			this->v.v4i[3] = vec.w;
			break;
		}
		case Variant::COLOR: {
			godot::Color color = value.operator godot::Color();
			this->v.v4f[0] = color.r;
			this->v.v4f[1] = color.g;
			this->v.v4f[2] = color.b;
			this->v.v4f[3] = color.a;
			break;
		}

		case Variant::OBJECT: { // Objects are represented as uintptr_t
			if (!implicit_trust)
				throw std::runtime_error("GuestVariant::set(): Cannot set OBJECT type without implicit trust");
			// TODO: Check if the object is already scoped?
			godot::Object *obj = value.operator godot::Object *();
			if (!emu.is_allowed(obj))
				throw std::runtime_error("GuestVariant::set(): Object is not allowed");
			emu.add_scoped_object(obj);
			this->v.i = (uintptr_t)obj;
			break;
		}

		case Variant::STRING:
		case Variant::TRANSFORM2D:
		case Variant::PLANE:
		case Variant::QUATERNION:
		case Variant::AABB:
		case Variant::BASIS:
		case Variant::TRANSFORM3D:
		case Variant::PROJECTION:
		case Variant::DICTIONARY:
		case Variant::ARRAY:
		case Variant::CALLABLE:
		case Variant::STRING_NAME:
		case Variant::NODE_PATH:
		case Variant::PACKED_BYTE_ARRAY:
		case Variant::PACKED_FLOAT32_ARRAY:
		case Variant::PACKED_FLOAT64_ARRAY:
		case Variant::PACKED_INT32_ARRAY:
		case Variant::PACKED_INT64_ARRAY:
		case Variant::PACKED_VECTOR2_ARRAY:
		case Variant::PACKED_VECTOR3_ARRAY:
		case Variant::PACKED_COLOR_ARRAY: {
			if (!implicit_trust)
				throw std::runtime_error("GuestVariant::set(): Cannot set complex type without implicit trust");
			this->v.i = emu.add_scoped_variant(&value);
			break;
		}
		default:
			ERR_PRINT(("SetVariant(): Unsupported type: " + std::to_string(value.get_type())).c_str());
	}
}

void GuestVariant::create(Sandbox &emu, Variant &&value) {
	this->type = value.get_type();

	switch (this->type) {
		case Variant::NIL:
		case Variant::BOOL:
		case Variant::INT:
		case Variant::FLOAT:

		case Variant::VECTOR2:
		case Variant::VECTOR2I:
		case Variant::RECT2:
		case Variant::RECT2I:
		case Variant::VECTOR3:
		case Variant::VECTOR3I:
		case Variant::VECTOR4:
		case Variant::VECTOR4I:
		case Variant::COLOR:
			this->set(emu, value, true); // Trust the value
			break;

		case Variant::OBJECT: {
			godot::Object *obj = value.operator godot::Object *();
			if (!emu.is_allowed(obj))
				throw std::runtime_error("GuestVariant::create(): Object is not allowed");
			emu.add_scoped_object(obj);
			this->v.i = (uintptr_t)obj;
			break;
		}

		case Variant::STRING:
		case Variant::TRANSFORM2D:
		case Variant::PLANE:
		case Variant::QUATERNION:
		case Variant::AABB:
		case Variant::BASIS:
		case Variant::TRANSFORM3D:
		case Variant::PROJECTION:
		case Variant::DICTIONARY:
		case Variant::ARRAY:
		case Variant::CALLABLE:
		case Variant::STRING_NAME:
		case Variant::NODE_PATH:
		case Variant::PACKED_BYTE_ARRAY:
		case Variant::PACKED_FLOAT32_ARRAY:
		case Variant::PACKED_FLOAT64_ARRAY:
		case Variant::PACKED_INT32_ARRAY:
		case Variant::PACKED_INT64_ARRAY:
		case Variant::PACKED_VECTOR2_ARRAY:
		case Variant::PACKED_VECTOR3_ARRAY:
		case Variant::PACKED_COLOR_ARRAY:
		case Variant::PACKED_STRING_ARRAY: {
			// Store the variant in the current state
			unsigned int idx = emu.create_scoped_variant(std::move(value));
			this->v.i = idx;
			break;
		}
		default:
			ERR_PRINT(("CreateVariant(): Unsupported type: " + std::to_string(value.get_type())).c_str());
	}
}

void GuestVariant::free(Sandbox &emu) {
	throw std::runtime_error("GuestVariant::free(): Not implemented");
}
