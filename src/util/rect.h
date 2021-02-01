#ifndef RECT_H
#define RECT_H

template <typename T>
struct Rect {
	Rect(T x1, T y1, T x2, T y2) :
		x1(x1), y1(y1), x2(x2), y2(y2) {}
	Rect() : x1(0), y1(0), x2(0), y2(0) {}
	
	bool inside(T x, T y) const {
		return x >= x1 && y >= y1 && x <= x2 && y <= y2;
	}
	
	bool intersects(const Rect<T>& other) {
		return !(
			other.x1 > x2 ||
			other.x2 < x1 ||
			other.y1 > y2 ||
			other.y2 < y1
		);
	}
	
	T x1; T y1;
	T x2; T y2;
    
    void moveTo(T x0, T y0) {
        T w = width();
        T h = height();
        
        x1 = x0;
        y1 = y0;
        
        x2 = x0 + w;
        y2 = y0 + h;
    }
    
    Rect<T> growCenter(int dx, int dy) {
        return Rect<int>(x1 - dx, y1 - dy, x2 + dx, y2 + dy);
    }
	
	T width() const {
		return x2 - x1;
	}
	
	T height() const {
		return y2 - y1;
	}
	
	T centerX() const {
		return (x2+x1)/2;
	}
	
	T centerY() const {
		return (y2+y1)/2;
	}
};

#endif
