#pragma once

namespace KamataEngine {

struct Vector3 final {
	float x;
	float y;
	float z;

	Vector3 operator+(const Vector3& rhs) const {
		return {
			x + rhs.x, y + rhs.y, z + rhs.z
		}; 
	}

	Vector3 operator-(const Vector3& rhs) const {
		return {
			x - rhs.x, y - rhs.y, z - rhs.z
		};
	}
};

} // namespace KamataEngine