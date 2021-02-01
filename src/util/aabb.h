#ifndef AABB_H
#define AABB_H

template<typename T> class AABB {
public:
	AABB()
	: x0(0), y0(0), z0(0),
		x1(0), y1(0), z1(0) {
	}
	AABB(T x0, T y0, T z0, T x1, T y1, T z1)
	: x0(x0), y0(y0), z0(z0),
		x1(x1), y1(y1), z1(z1) {
	}
	
	T width() const { return x1 - x0; }
	T height() const { return y1 - y1; }
	T depth() const { return z1 - z0; }
	
	T x0, y0, z0;
	T x1, y1, z1;
};


#endif //AABB_H