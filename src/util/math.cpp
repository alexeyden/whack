#include "math.h"

#include <algorithm>
#include <cmath>

bool lineIntersect(double line1X1, double line1Y1, double line1X2, double line1Y2, double line2X1, double line2Y1, double line2X2, double line2Y2, double& pointX, double& pointY)
{
	struct pnt { double x; double y; };
	pnt a1{line1X1, line1Y1}, a2{line1X2, line1Y2};
	pnt b1{line2X1, line2Y1}, b2{line2X2,line2Y2};
	double eps = 0.00001; 
	
  double d =(a1.x-a2.x)*(b2.y-b1.y) - (a1.y-a2.y)*(b2.x-b1.x);
  double da=(a1.x-b1.x)*(b2.y-b1.y) - (a1.y-b1.y)*(b2.x-b1.x);
  //double db=(a1.x-a2.x)*(a1.y-b1.y) - (a1.y-a2.y)*(a1.x-b1.x);

  if (fabs(d)<eps)
		return false;
  else {
    double ta = da/d;
    //double tb = db/d;
#warning TODO: check tb 
    
    if  ((0<=ta) && (ta<=1)) {
			pointX = a1.x + ta * (a2.x - a1.x);
			pointY = a1.y + ta * (a2.y - a1.y);
			return true;
		}
		else return false;
	}
}

bool rayAABBIntersect2D(double rayX, double rayY, double rayDirX, double rayDirY, const AABB<float>& aabb)
{
	double tx1 = (aabb.x0 - rayX)/rayDirX;
	double tx2 = (aabb.x1 - rayX)/rayDirX;
	
	double tmin = std::min(tx1, tx2);
	double tmax = std::max(tx1, tx2);
	
	double ty1 = (aabb.y0 - rayY)/rayDirY;
	double ty2 = (aabb.y1 - rayY)/rayDirY;

	tmin = std::max(tmin, std::min(ty1, ty2));
	tmax = std::min(tmax, std::max(ty1, ty2));
	
	return tmax >= tmin;
}

bool rayAABBIntersect3D(double rayX, double rayY, double rayZ, double rayDirX, double rayDirY, double rayDirZ, const AABB< float >& aabb, float* t)
{
	double invDirX = 1.0 / rayDirX, invDirY = 1.0 / rayDirY, invDirZ = 1.0 / rayDirZ;
	
	double tx1 = (aabb.x0 - rayX) * invDirX;
	double tx2 = (aabb.x1 - rayX) * invDirX;
	
	double tmin = std::min(tx1, tx2);
	double tmax = std::max(tx1, tx2);
	
	double ty1 = (aabb.y0 - rayY) * invDirY;
	double ty2 = (aabb.y1 - rayY) * invDirY;

	double tymin = std::min(ty1, ty2);
	double tymax = std::max(ty1, ty2);
	
	if((tmin > tymax) || (tymin > tmax))
		return false;
	
	if(tymin > tmin)
		tmin = tymin;
	
	if(tymax < tmax)
		tmax = tymax;
	
	double tz1 = (aabb.z0 - rayZ) * invDirZ;
	double tz2 = (aabb.z1 - rayZ) * invDirZ;
	
	double tzmin = std::min(tz1, tz2);
	double tzmax = std::max(tz1, tz2);
	
	if((tmin > tzmax) || (tzmin > tmax))
		return false;
	
	if(tzmin > tmin)
		tmin = tzmin;
	
	if(tzmax < tmax)
		tmax = tzmax;

	double final_t = tmin;
	
	if(final_t < 0) {
		final_t = tmax;
		
		if(final_t < 0)
			return false;
	}
	
	if(t != nullptr) {
		*t = final_t;
	}
	
	return true;
}

float rayCylinderIntersect(vec3 rayDir, vec3 rayPos, vec3 cylPos, vec2 cylSize)
{
	float xd = rayDir.x, yd = rayDir.y;
	float xp = rayPos.x, yp = rayPos.y;
	float xc = cylPos.x, yc = cylPos.y;
	float r = cylSize.x, h = cylSize.y;
	
	float a = yd*yd + xd*xd;
	float b = 2 * yd * yp - 2 * yc * yd + 2 * xd * xp - 2 * xc * xd;
	float c = yp*yp - 2*yc*yp + yc*yc + xp*xp - 2*xc*xp + xc*xc - r*r;
	
	float D = b*b - 4*a*c;
	
	if(D < 0) {
		return -1;
	}
	
	float t1 = (-b + sqrt(D))/(2 * a);
	float t2 = (-b - sqrt(D))/(2 * a);
	
	float z1 = rayPos.z + rayDir.z * t1; 
	float z2 = rayPos.z + rayDir.z * t2; 
	
	if(z1 < cylPos.z || z2 < cylPos.z ||
		z1 > cylPos.z + h || z2 > cylPos.z + h) {
		return -1;
	}
	
	return std::min(t1, t2);
}

bool circleTriangleIntersect(vec2 c0, float cr, vec2 v1, vec2 v2, vec2 v3)
{
    // adapded from: http://www.phatcode.net/articles.php?id=459
    
    // v1-v3 inside circle
    
    float cr2 = cr*cr;
    
    vec2 c1 = c0 - v1;
    vec2 c2 = c0 - v2;
    vec2 c3 = c0 - v3;
    
    if ((c1.length2() - cr2) <= 0)
        return true;
    if ((c2.length2() - cr2) <= 0)
        return true;
    if ((c3.length2() - cr2) <= 0)
        return true;
    
    // c0 inside triangle
    
    vec2 e1 = v2 - v1;
    vec2 e2 = v3 - v2;
    vec2 e3 = v1 - v3;
    
    if(e1.cross(c1) >= 0 && e2.cross(c2) >= 0 && e3.cross(c3) >= 0) {
        return true;
    }
    
    // circle intersects the edge
    
    float k1 = c1.dot(e1);
    float k2 = c2.dot(e2);
    float k3 = c3.dot(e3);
    
    if ((k1 > 0) && (k1 < e1.length2()) && ((c1.length2() - cr2) * e1.length2() <= k1*k1)) {
        return true;
    }
    
    if ((k2 > 0) && (k2 < e2.length2()) && ((c2.length2() - cr2) * e2.length2() <= k2*k2)) {
        return true;
    }
    
    if ((k3 > 0) && (k3 < e3.length2()) && ((c3.length2() - cr2) * e3.length2() <= k3*k3)) {
        return true;
    }
    
    return false;
}
