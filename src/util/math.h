#ifndef MATHUTIL_H
#define MATHUTIL_H

#include <cmath>
#include "aabb.h"

class point2i
{
public:
	point2i() : x(0), y(0) {}
	point2i(int x, int y) : x(x), y(y) {}
	
	point2i operator+(const point2i& that) const {
		return point2i(x + that.x, y + that.y);
	}
	
	point2i operator-(const point2i& that) const {
		return point2i(x - that.x, y - that.y);
	}
	
	bool operator==(const point2i& that) const {
		return x == that.x && y == that.y;
	}
	
	int manhattanLength() const {
		return std::abs(x) + std::abs(y);
	}
	
	int x, y;
};

class vec2 {
public:
	vec2() : x(0), y(0) { } 
	vec2(double x, double y) : x(x), y(y) { } 
	
	double length() const { return sqrt(x*x + y*y); }
	double length2() const { return x*x + y*y; }
	
	vec2 operator-(const vec2& v) const {
		return vec2(x - v.x, y - v.y);
	}
	
	vec2& operator-=(const vec2& v) {
		x -= v.x; y -= v.y;
		return *this;
	}
	
	vec2 operator+(const vec2& v) const {
		return vec2(x + v.x, y + v.y);
	}
	
	vec2& operator+=(const vec2& v) {
		x += v.x; y += v.y;
		return *this;
	}
	
	vec2 operator-() const {
		return inverted();
	}
	
	vec2 operator*(double k) const {
		return vec2(x * k, y * k);
	}
	
	vec2& operator*=(double k) {
		x *= k; y *= k;
		return *this;
	}
	
	vec2 operator/(double k) const {
		return vec2(x / k, y / k);
	}
	
	vec2& operator/=(double k) {
		x /= k; y /= k;
		return *this;
	}
	
	vec2& invert() {
		x = -x; y = -y;
		return *this;
	}
	
	vec2 inverted() const {
		return vec2(-x, -y);
	}
	
	double dot(const vec2& v) const {
		return x * v.x + y * v.y;
	}
	
	double slope() const {
		return y/x;
	}
	
	double cross(const vec2& v) const {
        return y * v.x - x * v.y;
    }
	
	vec2& normalize() {
		double len = length();
		x /= len; y /= len;
		return *this;
	}
	
	vec2 normalized() const {
		double len = length();
		return vec2(x/len, y/len);
	}
	
	double x, y;
};

class vec3 {
public:
	vec3() : x(0), y(0), z(0) {}
	vec3(double x, double y, double z) : x(x), y(y), z(z) { } 
	
	double length() const { return sqrt(x*x + y*y + z*z); }
	
	vec3 operator-(const vec3& v) const {
		return vec3(x - v.x, y - v.y, z - v.z);
	}
	
	vec3& operator-=(const vec3& v) {
		x -= v.x; y -= v.y; z -= v.z;
		return *this;
	}
	
	vec3 operator+(const vec3& v) const {
		return vec3(x + v.x, y + v.y, z + v.z);
	}
	
	vec3& operator+=(const vec3& v) {
		x += v.x; y += v.y; z += v.z;
		return *this;
	}
	
	vec3 operator-() const {
		return inverted();
	}
	
	vec3 operator*(double k) const {
		return vec3(x * k, y * k, z * k);
	}
	
	vec3& operator*=(double k) {
		x *= k; y *= k; z *= k;
		return *this;
	}
	
	vec3 operator/(double k) const {
		return vec3(x / k, y / k, z / k);
	}
	
	vec3& operator/=(double k) {
		x /= k; y /= k; z /= k;
		return *this;
	}
	
	vec3& invert() {
		x = -x; y = -y; z = -z;
		return *this;
	}
	
	vec3 inverted() const {
		return vec3(-x, -y, -z);
	}
	
	double dot(const vec3& v) const {
		return x * v.x + y * v.y + z * v.z;
	}
	
	vec3& normalize() {
		float len = length();
		x /= len; y /= len; z /= len;
		return *this;
	}
	
	vec3 normalized() const {
		float len = length();
		return vec3(x/len, y/len, z/len);
	}
	
	vec2 xy() const {
		return vec2(x, y);
	}
	
	double x, y, z;
};

bool lineIntersect(double line1X1, double line1Y1, double line1X2, double line1Y2,
									 double line2X1, double line2Y1, double line2X2, double line2Y2,
									 double& pointX, double& pointY);

bool rayAABBIntersect2D(double rayX, double rayY, double rayDirX, double rayDirY, const AABB<float>& aabb);
bool rayAABBIntersect3D(double rayX, double rayY, double rayZ,
												double rayDirX, double rayDirY, double rayDirZ, const AABB<float>& aabb, float* t = nullptr); 
float rayCylinderIntersect(vec3 rayDir, vec3 rayPos, vec3 cylPos, vec2 cylSize);

bool circleTriangleIntersect(vec2 c0, float cr, vec2 v1, vec2 v2, vec2 v3);

#endif // MATHUTIL_H
