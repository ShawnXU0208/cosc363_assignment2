#include "cylinder.h"
#include <math.h>


/**
 * Cylinder's intersection method. The input is a ray (pos, dir).
 */
float Cylinder::intersect(Vector pos, Vector dir)
{
    float x = (pos.x - center.x); //x0 - xc
    float z = (pos.z - center.z); // z0 - zc

    float a = dir.x*dir.x + dir.z*dir.z; //dx ^2 - dz ^2
    float b = 2*dir.x*(pos.x - center.x) + 2*dir.z*(pos.z - center.z); //dx * x + dz * z
    float c = x*x + z*z - radius*radius;

    float delta = b*b - 4*a*c;

    if (delta < 0) 
    {
		return -1.0;
	}

    float t0 = (-b + sqrt(delta)) / (2*a);
    float t1 = (-b - sqrt(delta)) / (2*a);
    if (t0>t1) {
		float tmp = t0;
		t0=t1;
		t1=tmp;
	}
    if (t1 < 0 )
    {
		return -1.0;
	}
    float y0 = pos.y + t0*dir.y;
    float y1 = pos.y + t1*dir.y;
	if (y0<center.y){
		if (y1<center.y)
		{ 
			return -1.0;
		}
		else{
			float t = (center.y - pos.y ) / dir.y;
			if (t<=0) 
			{
				return -1.0;
			}
			return t;
		}
	}else if (y0 > center.y + height){
		if (y1 > center.y + height) 
		{
			return -1.0;
		}
		else{
			float t = (center.y + height - pos.y ) / dir.y;
			if (t<=0) 
			{	
				return -1.0;
			}
			return t;
			}
   } else if (y0 > center.y && y0 < center.y + height){
	   if (t0<=0) return -1.0;
	   return t0;
   }
	return -1.0;
}





Vector Cylinder::normal(Vector p)
{

    Vector n;
	if(p.y > center.y && p.y < (center.y + height)) {
			n = Vector(p.x - center.x,0,p.z - center.z);
		}
		else if (fabs(p.y - center.y)<= 0.001) {
			n = Vector(0, -1, 0);
		}
		else if (fabs(p.y - center.y - height) <= 0.001) {
			n = Vector(0, 1, 0);
		}
		
		n.normalise();
		return n;
}
