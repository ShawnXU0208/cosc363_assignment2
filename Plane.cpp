/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The Plane class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Plane.h"
#include "Vector.h"
#include <math.h>

//Function to test if an input point is within the quad.
bool Plane::isInside(Vector q)
{
	Vector n = normal(q);
	Vector ua = b-a,  ub = c-b, uc = d-c, ud = a-d;
	Vector va = q-a,  vb = q-b, vc = q-c, vd = q-d;
	//Complete this function
	bool result = false;
	
	Vector norm1 = ua.cross(va);
	Vector norm2 = ub.cross(vb);
	Vector norm3 = uc.cross(vc);
	Vector norm4 = ud.cross(vd);
	
	if(norm1.dot(n) > 0 && norm2.dot(n) > 0 && norm3.dot(n) > 0 && norm4.dot(n) > 0){
		result = true;
	}
	
	return result;
}

//Function to compute the paramter t at the point of intersection.
float Plane::intersect(Vector pos, Vector dir)
{
	Vector n = normal(pos);
	Vector vdif = a-pos;
	float vdotn = dir.dot(n);
	if(fabs(vdotn) < 1.e-4) return -1;
    float t = vdif.dot(n)/vdotn;
	if(fabs(t) < 0.0001) return -1;
	Vector q = pos + dir*t;
	if(isInside(q)) return t;
    else return -1;
}

// Function to compute the unit normal vector
// Remember to output a normalised vector!
Vector Plane::normal(Vector pos)
{
	Vector AB = b - a;
	Vector AC = c - a;
	Vector norm = AB.cross(AC);
	norm.normalise();
	return norm;

	//Implement this function.
}



