#ifndef __MATH_UTIL_H__
#define __MATH_UTIL_H__

#include <math.h>

class Vector2
{
public:
	union
	{
		struct
		{
			float x;
			float y;
		};
		float mem[2];
	};
	Vector2(void) : x(0.0f), y(0.0f)
	{};
	Vector2(float value) : x(value), y(value)
	{};
	Vector2(float fX, float fY) : x(fX), y(fY)
	{};
	~Vector2(void)
	{}
public:
	inline Vector2 operator-() const;
	inline Vector2 operator+(const Vector2& V) const;
	inline Vector2 operator-(const Vector2& V) const;
	inline Vector2 operator/(float value) const;
	inline Vector2 operator/(const Vector2& V) const;
	inline Vector2 operator*(float value) const;
	inline Vector2 operator*(const Vector2& V) const;
	inline Vector2 operator=(const Vector2& V);
	inline Vector2 operator=(float value);
	inline void operator+=(const Vector2& V);
	inline void operator+=(float value);  
	inline void operator-=(const Vector2& V);
	inline void operator-=(float value);  
	inline void operator*=(const Vector2& V);
	inline void operator*=(float value);  
	inline void operator/=(const Vector2& V);
	inline void operator/=(float value);  
	inline bool operator==(const Vector2& V) const;
	inline bool operator!=(const Vector2& V) const;
	inline float& operator[](int index) { return mem[index]; }
	inline float operator[](int index) const { return mem[index]; }
	inline float Length() const;
	inline float LengthSquared() const;
	inline void Normalize();
	void Set(float fX, float fY);
	inline float Dot(const Vector2 &V) const;
	inline static float Vector2Dot(const Vector2 &V1, const Vector2 &V2);
};

Vector2 Vector2::operator-() const
{
	return Vector2(-x, -y);
}

Vector2 Vector2::operator+(const Vector2 &V) const
{
	return Vector2(x + V.x, y + V.y);
}

Vector2 Vector2::operator-(const Vector2 &V) const
{
	return Vector2(x - V.x, y - V.y);
}

Vector2 Vector2::operator/(float value) const
{
	return Vector2(x / value, y / value);
}

Vector2 Vector2::operator/(const Vector2 &V) const
{
	return Vector2(x / V.x, y / V.y);
}

Vector2 Vector2::operator*(float value) const
{
	return Vector2(x * value, y * value);
}

Vector2 Vector2::operator*(const Vector2 &V) const
{
	return Vector2(x * V.x, y * V.y);
}

Vector2 Vector2::operator=(const Vector2& V)
{
	x = V.x;
	y = V.y;
	return *this;
}

Vector2 Vector2::operator=(float value)
{
	x = value;
	y = value;
	return *this;
}

void Vector2::operator+=(const Vector2 &V)
{
	x += V.x;
	y += V.y;
}

void Vector2::operator+=(float value)
{
	x += value;
	y += value;
}

void Vector2::operator-=(const Vector2 &V)
{
	x -= V.x;
	y -= V.y;
}

void Vector2::operator-=(float value)
{
	x -= value;
	y -= value;
}

void Vector2::operator*=(const Vector2 &V)
{
	x *= V.x;
	y *= V.y;
}

void Vector2::operator*=(float value)
{
	x *= value;
	y *= value;
}

void Vector2::operator/=(const Vector2 &V)
{
	x /= V.x;
	y /= V.y;
}

void Vector2::operator/=(float value)
{
	x /= value;
	y /= value;
}

bool Vector2::operator==(const Vector2 &V) const
{
	return (x == V.x && y == V.y);
}

bool Vector2::operator!=(const Vector2 &V) const
{
	return !(x == V.x && y == V.y);
}

float Vector2::Length() const
{
	return sqrtf(x*x + y*y);
}

float Vector2::LengthSquared() const
{
	return x*x + y*y;
}

void Vector2::Normalize()
{
	float len = Length();
	if (len == 0.0f) return;
	x /= len;
	y /= len;
}

inline void Vector2::Set(float fX, float fY)
{
	x = fX; y = fY;
}

inline float Vector2::Vector2Dot(const Vector2 &V1, const Vector2 &V2)
{
	return V1.x * V2.x + V1.y * V2.y;
}

inline float Vector2::Dot(const Vector2 &V) const
{
	return V.x * x + V.y * y;
}

#endif