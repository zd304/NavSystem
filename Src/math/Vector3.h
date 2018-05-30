#ifndef __VECTOR3_H__
#define __VECTOR3_H__

#include "Vector2.h"

class Vector3;

Vector3 operator*(float value, const Vector3 & V);

class Vector3
{
public:
	union
	{
		struct
		{
			float x;
			float y;
			float z;
		};
		float mem[3];
	};

	Vector3(void) : x(0.0f), y(0.0f), z(0.0f)
	{};
	Vector3(float value) : x(value), y(value), z(value)
	{};
	Vector3(const Vector2& Vec2, float fZ = 0.0f) : x(Vec2.x), y(Vec2.y), z(fZ)
	{}
	Vector3(float fX, float fY, float fZ) : x(fX), y(fY), z(fZ)
	{}
	~Vector3(void)
	{}
public:
	static const Vector3 ZERO;
public:
	inline Vector3 operator-() const;
	inline Vector3 operator+(const Vector3 &V) const;
	inline Vector3 operator-(const Vector3 &V) const;
	inline Vector3 operator/(float value) const;
	inline Vector3 operator/(const Vector3 &V) const;
	inline Vector3 operator*(float value) const;
	inline Vector3 operator*(const Vector3 &V) const;
	inline Vector3& operator=(const Vector3 &V);
	inline Vector3& operator=(float value);
	inline Vector3& operator+=(const Vector3 &V);
	inline Vector3& operator+=(float value);
	inline Vector3& operator-=(const Vector3 &V);
	inline Vector3& operator-=(float value);
	inline Vector3& operator*=(const Vector3 &V);
	inline Vector3& operator*=(float value);
	inline Vector3& operator/=(const Vector3 &V);
	inline Vector3& operator/=(float value);
	inline bool operator==(const Vector3 &V) const;
	inline bool operator!=(const Vector3 &V) const;
	inline float& operator[](int index) { return mem[index]; }
	inline float operator[] (int index) const { return mem[index]; }
	inline float Length() const;
	inline float LengthSquared() const;
	inline void Normalize();
	inline void Set(float fX, float fY, float fZ);
	inline Vector2& GetVec2(void) const;
	inline static float Vector3Dot(const Vector3 &V1, const Vector3 &V2);
	inline static void Vector3Cross(Vector3& Out, const Vector3 &V1, const Vector3 &V2);
	inline static float Vector3Length(const Vector3 &V);
	inline static void Vector3Normalize(Vector3 &V);
};

inline Vector3 Vector3::operator-() const
{
	return Vector3(-x, -y, -z);
}

inline Vector3 Vector3::operator+(const Vector3 &V) const
{
	return Vector3(x + V.x, y + V.y, z + V.z);
}

inline Vector3 Vector3::operator-(const Vector3 &V) const
{
	return Vector3(x - V.x, y - V.y, z - V.z);
}

inline Vector3 Vector3::operator/(float value) const
{
	return Vector3(x / value, y / value, z / value);
}

inline Vector3 Vector3::operator/(const Vector3 &V) const
{
	return Vector3(x / V.x, y / V.y, z / V.z);
}

inline Vector3 Vector3::operator*(float value) const
{
	return Vector3(x * value, y * value, z * value);
}

inline Vector3 Vector3::operator*(const Vector3 &V) const
{
	return Vector3(x * V.x, y * V.y, z * V.z);
}

inline Vector3& Vector3::operator=(const Vector3& V)
{
	x = V.x;
	y = V.y;
	z = V.z;
	return *this;
}

inline Vector3& Vector3::operator=(float value)
{
	x = value;
	y = value;
	z = value;
	return *this;
}

inline Vector3& Vector3::operator+=(const Vector3& V)
{
	x += V.x;
	y += V.y;
	z += V.z;
	return *this;
}

inline Vector3& Vector3::operator+=(float value)
{
	x += value;
	y += value;
	z += value;
	return *this;
}

inline Vector3& Vector3::operator-=(const Vector3 &V)
{
	x -= V.x;
	y -= V.y;
	z -= V.z;
	return *this;
}

inline Vector3& Vector3::operator-=(float value)
{
	x -= value;
	y -= value;
	z -= value;
	return *this;
}

inline Vector3& Vector3::operator*=(const Vector3 &V)
{
	x *= V.x;
	y *= V.y;
	z *= V.z;
	return *this;
}

inline Vector3& Vector3::operator*=(float value)
{
	x *= value;
	y *= value;
	z *= value;
	return *this;
}

inline Vector3& Vector3::operator/=(const Vector3 &V)
{
	x /= V.x;
	y /= V.y;
	z /= V.z;
	return *this;
}

inline Vector3& Vector3::operator/=(float value)
{
	x /= value;
	y /= value;
	z /= value;
	return *this;
}

inline bool Vector3::operator==(const Vector3 &V) const
{
	return (x == V.x && y == V.y && z == V.z);
}

inline bool Vector3::operator!=(const Vector3 &V) const
{
	return !(x == V.x && y == V.y && z == V.z);
}

inline float Vector3::Length() const
{
	return sqrtf(x*x + y*y + z*z);
}

inline float Vector3::LengthSquared() const
{
	return x*x + y*y + z*z;
}

inline void Vector3::Normalize()
{
	float len = Length();
	if (len == 0.0f)
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
		return;
	}
	x /= len;
	y /= len;
	z /= len;
}

inline void Vector3::Set(float fX, float fY, float fZ)
{
	x = fX; y = fY; z = fZ;
}

inline Vector2& Vector3::GetVec2(void) const
{
	return *(Vector2*)mem;
}

inline float Vector3::Vector3Dot(const Vector3 &V1, const Vector3 &V2)
{
	return V1.x * V2.x + V1.y * V2.y + V1.z * V2.z;
}

inline void Vector3::Vector3Cross(Vector3 &Out, const Vector3 &V1, const Vector3 &V2)
{
	Out.x = V1.y * V2.z - V1.z * V2.y;
	Out.y = V1.z * V2.x - V1.x * V2.z;
	Out.z = V1.x * V2.y - V1.y * V2.x;
}

inline float Vector3::Vector3Length(const Vector3 &V)
{
	return V.Length();
}

inline void Vector3::Vector3Normalize(Vector3 &V)
{
	V.Normalize();
}

inline Vector3 operator*(float value, const Vector3 &V)
{
	return Vector3(value * V.x, value * V.y, value * V.z);
}

#endif