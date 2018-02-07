
#ifndef H_CYLINDER
#define H_CYLINDER

#include "Object.h"


class Cylinder : public Object {
private:
    Vector center;
    float radius;
    float height;
  
public:  
    Cylinder() 
              	: center(Vector()),height(1),radius(1)  //Default constructor creates a unit cylinder
	{
	    	color = Color::WHITE;
	};
    
    Cylinder(Vector c,float h,float r,Color col)
		: center(c),height(h),radius(r)
	{
	    	color = col;
	};

    	float intersect(Vector pos, Vector dir);
    
    	Vector normal(Vector p);
    
};

#endif //!H_CYLINDER
