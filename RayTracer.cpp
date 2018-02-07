// ========================================================================
// COSC 363  Computer Graphics  Lab07
// A simple ray tracer
// ========================================================================

#include <iostream>
#include <cmath>
#include <vector>
#include "Vector.h"
#include "Sphere.h"
#include "Color.h"
#include "Object.h"
#include "Plane.h"
#include "cylinder.h"
#include <GL/glut.h>
#include "TextureBMP.h"
using namespace std;

const float WIDTH = 20.0;  
const float HEIGHT = 20.0;
const float EDIST = 40.0;
const int PPU = 30;     //Total 600x600 pixels
const int MAX_STEPS = 5;
const float XMIN = -WIDTH * 0.5;
const float XMAX =  WIDTH * 0.5;
const float YMIN = -HEIGHT * 0.5;
const float YMAX =  HEIGHT * 0.5;
const float reflCoeff = 0.6;
const float transparentCoeff = 0.8;

TextureBMP texture_ground;
TextureBMP texture_building;

vector<Object*> sceneObjects;

Vector light1, light2;
Color backgroundCol;

Plane *plane;

Vector ground_vector1(-30, -10, 10);
Vector ground_vector2(30, -10, 10);
Vector ground_vector3(30, -10, -90);
Vector ground_vector4(-30, -10, -90);

float pi = 3.1415926;

//A useful struct
struct PointBundle   
{
	Vector point;
	int index;
	float dist;
};

/*
* This function compares the given ray with all objects in the scene
* and computes the closest point  of intersection.
*/
PointBundle closestPt(Vector pos, Vector dir)
{
    Vector  point(0, 0, 0);
	float min = 10000.0;

	PointBundle out = {point, -1, 0.0};

    for(int i = 0;  i < sceneObjects.size();  i++)
	{
        float t = sceneObjects[i]->intersect(pos, dir);
		if(t > 0)        //Intersects the object
		{
			point = pos + dir*t;
			if(t < min)
			{
				out.point = point;
				out.index = i;
				out.dist = t;
				min = t;
			}
		}
	}

	return out;
}

/*
* Computes the colour value obtained by tracing a ray.
* If reflections and refractions are to be included, then secondary rays will 
* have to be traced from the point, by converting this method to a recursive
* procedure.
*/

Color trace(Vector pos, Vector dir, int step)
{
	Color colorSum;
    PointBundle q = closestPt(pos, dir);
    
    if(q.index == -1) return backgroundCol;        //no intersection

    Color col = sceneObjects[q.index]->getColor(); //Object's colour
    
	Vector n = sceneObjects[q.index]->normal(q.point);
    
    PointBundle closest_point;
    //generate shadow caused by light1
    Vector lightVector_1 = light1 - q.point;
    float lightDist_1 = lightVector_1.length();
    lightVector_1.normalise();
    
    closest_point = closestPt(q.point, lightVector_1);
    if(closest_point.index > -1 && closest_point.dist < lightDist_1){
		colorSum = col.phongLight(backgroundCol, 0.0, 0.0);
		return colorSum;
	}
    
 
 
    //generate shadow caused by light2
    Vector lightVector_2 = light2 - q.point;
    float lightDist_2 = lightVector_2.length();
    lightVector_2.normalise();
    
    closest_point = closestPt(q.point, lightVector_2);
    if(closest_point.index > -1 && closest_point.dist < lightDist_2){
		colorSum = col.phongLight(backgroundCol, 0.0, 0.0);
		return colorSum;
	}
  
   
   
   	//texture to floor
	if (q.index == 9){
		Vector a = ground_vector1;
		Vector b = ground_vector2;
		Vector c = ground_vector3;
		Vector d = ground_vector4;
		
		float u = ((q.point - a).dot(b-a)/(b-a).length()/(b-a).length());
		float v = ((q.point - d).dot(a-d)/(a-d).length()/(a-d).length());
		col = texture_ground.getColorAt(u,v);
		colorSum.combineColor(col);
	}
	
	//texture to a sphere
	if(q.index == 0){
		float u = 1-(0.5 + (atan2(n.z, n.x))/(2*pi));
		float v = 1-(0.5 - (asin(n.y))/pi);
		col = texture_building.getColorAt(u,v);
		
	}
   
   
   	//transparent object
	if((q.index == 1 ||q.index == 2 || q.index == 3 || q.index == 4) && step < MAX_STEPS)
	{	
		PointBundle m = closestPt(q.point, dir);
		Color transparentCol = trace(m.point, dir, step + 1);
		colorSum.combineColor(transparentCol, transparentCoeff);
	}
	
	
	//Generate refraction ray
	//Vector RefractionVector;
	//float refractionCoeff = 0.8;

/*
	if(q.index == 0 && step < MAX_STEPS)
	{	
		float n1 = 1.0;
		float n2 = 1.03;
		float coff = n1/n2;	
		float costhetaT = sqrt(1 - coff*coff * (1 - dir.dot(n) * dir.dot(n)));
		Vector g = dir*coff - n*(coff * dir.dot(n) + costhetaT);
		PointBundle r = closestPt(q.point, g);
	    Object *out = sceneObjects[r.index];
	    Vector n_negative = out->normal(r.point) * -1;
	    coff = n2/n1;   
	    costhetaT = sqrt(1 - coff*coff * (1 - g.dot(n_negative) * g.dot(n_negative)));
	    RefractionVector = dir*coff - n_negative*(coff * g.dot(n_negative) + costhetaT);
		Color refractionCol = trace (r.point, RefractionVector, step+1);
    	colorSum.combineColor(refractionCol, refractionCoeff);
	}
   
 */
	
	Vector l = light1 - q.point;
	l.normalise();
	float lDotn = l.dot(n);
	float rDotv;
	float spec;
	if(lDotn <= 0){
		colorSum.combineColor(col.phongLight(backgroundCol, 0.0, 0.0));  //diffuse reflection
	}else{
		Vector r = ((n * 2) * lDotn) - l;
		r.normalise();
		Vector v(-dir.x, -dir.y, -dir.z);
		rDotv = r.dot(v);
		if(rDotv < 0){
			spec = 0.0;
		}else{
			spec = pow(rDotv, 10);
		}
		colorSum = col.phongLight(backgroundCol, lDotn, spec);  //specular reflection
		
		//Generate reflection ray
		if(q.index == 0 && step < MAX_STEPS){
			Vector reflectionVector = pos - q.point;
			float nDotv = n.dot(reflectionVector);
			reflectionVector = (n*2)*nDotv - reflectionVector;
			reflectionVector.normalise();
			
			Color reflectionCol = trace(q.point, reflectionVector, step+1);
			colorSum.combineColor(reflectionCol, reflCoeff);

		}
	}



    return colorSum;

}

//---The main display module -----------------------------------------------------------
// In a ray tracing application, it just displays the ray traced image by drawing
// each pixel as quads.
//---------------------------------------------------------------------------------------
void display()
{
	int widthInPixels = (int)(WIDTH * PPU);
	int heightInPixels = (int)(HEIGHT * PPU);
	float pixelSize = 1.0/PPU;
	float halfPixelSize = pixelSize/2.0;
	float quarterPixelSize = pixelSize/4.0;
	float x1, y1, xc, yc;
	Vector eye(0., 0., 0.);

	glClear(GL_COLOR_BUFFER_BIT);

	glBegin(GL_QUADS);  //Each pixel is a quad.

	for(int i = 0; i < widthInPixels; i++)	//Scan every "pixel"
	{
		x1 = XMIN + i*pixelSize;
		xc = x1 + halfPixelSize;
		for(int j = 0; j < heightInPixels; j++)
		{
			y1 = YMIN + j*pixelSize;
			yc = y1 + halfPixelSize;


			//Anti-Aliasing
			Vector dirA(xc - quarterPixelSize, yc + quarterPixelSize, -EDIST);
			Vector dirB(xc + quarterPixelSize, yc + quarterPixelSize, -EDIST);
			Vector dirC(xc - quarterPixelSize, yc - quarterPixelSize, -EDIST);
			Vector dirD(xc - quarterPixelSize, yc - quarterPixelSize, -EDIST);
			
			dirA.normalise();
			dirB.normalise();
			dirC.normalise();
			dirD.normalise();
			
			Color colorA = trace(eye, dirA, 1);
			Color colorB = trace(eye, dirB, 1);
			Color colorC = trace(eye, dirC, 1);
			Color colorD = trace(eye, dirD, 1);
			
			//average every color value
			float color_r = (colorA.r + colorB.r + colorC.r + colorD.r) / 4;
			float color_g = (colorA.g + colorB.g + colorC.g + colorD.g) / 4;
			float color_b = (colorA.b + colorB.b + colorC.b + colorD.b) / 4;
			
			
			glColor3f(color_r, color_g, color_b);
			

			glVertex2f(x1, y1);				//Draw each pixel with its color value
			glVertex2f(x1 + pixelSize, y1);
			glVertex2f(x1 + pixelSize, y1 + pixelSize);
			glVertex2f(x1, y1 + pixelSize);
        }
    }

    glEnd();
    glFlush();
}



void initialize()
{
	//Iniitialize background colour and light's position
	backgroundCol = Color::GRAY;
	light1 = Vector(10.0, 60.0, -5.0);
	light2 = Vector(-10.0, 80.0, 0.0);
	
	texture_ground = TextureBMP((char*)"grass_field.bmp");
	texture_building = TextureBMP((char*)"building.bmp");

	//object0
	Sphere *bigsphere = new Sphere (Vector(0, -10, -100), 20.0, Color(0.1f, 0.1f, 0.1f));
	sceneObjects.push_back(bigsphere);
	

	//object1
	Cylinder *cylinder3 = new Cylinder(Vector(-4, -10, -60), 6, 1, Color(0.8f, 0.8f, 0.8f));
	sceneObjects.push_back(cylinder3);	
	
	//object2
	Cylinder *cylinder4 = new Cylinder(Vector(4, -10, -60), 6, 1, Color(0.8f, 0.8f, 0.8f));
	sceneObjects.push_back(cylinder4);
	
	//object3
	Cylinder *cylinder5 = new Cylinder(Vector(-6, -10, -40), 6, 1, Color(0.8f, 0.8f, 0.8f));
	sceneObjects.push_back(cylinder5);
	
	//object4
	Cylinder *cylinder6 = new Cylinder(Vector(6, -10, -40), 6, 1, Color(0.8f, 0.8f, 0.8f));
	sceneObjects.push_back(cylinder6);
	
	
	//object5
	Sphere *sphere3 = new Sphere (Vector(-4, -3, -60), 1.0, Color::RED);
	sceneObjects.push_back(sphere3);
	
	//object6
	Sphere *sphere4 = new Sphere (Vector(4, -3, -60), 1.0, Color::RED);
	sceneObjects.push_back(sphere4);
	
	//object7
	Sphere *sphere5 = new Sphere (Vector(-6, -3, -40), 1.0, Color::RED);
	sceneObjects.push_back(sphere5);
	
	//object8
	Sphere *sphere6 = new Sphere (Vector(6, -3, -40), 1.0, Color::RED);
	sceneObjects.push_back(sphere6);
	
	//object9
	plane = new Plane(ground_vector1, ground_vector2, ground_vector3, ground_vector4, Color(1,0,1));
	sceneObjects.push_back(plane);
	

	
	
	//Add a cube consists of 6 planes
	Plane *plane1_cube = new Plane(Vector(-6, 8, -50), Vector(-6, 6, -50), Vector(-2, 6, -50), Vector(-2, 8, -50), Color::GREEN);
	sceneObjects.push_back(plane1_cube);
	
	Plane *plane2_cube = new Plane(Vector(-2, 8, -50), Vector(-2, 6, -50), Vector(-2, 6, -55), Vector(-2, 8, -55), Color::GREEN);
	sceneObjects.push_back(plane2_cube);
	
	Plane *plane3_cube = new Plane(Vector(-6, 8, -55), Vector(-2, 8, -55), Vector(-2, 6, -55), Vector(-6, 6, -55), Color::GREEN);
	sceneObjects.push_back(plane3_cube);
	
	Plane *plane4_cube = new Plane(Vector(-6, 8, -55), Vector(-6, 6, -55), Vector(-6, 6, -50), Vector(-6, 8, -50), Color::GREEN);
	sceneObjects.push_back(plane4_cube);
	
	Plane *plane5_cube = new Plane(Vector(-6, 8, -55), Vector(-6, 8, -50), Vector(-2, 8, -50), Vector(-2, 8, -55), Color::GREEN);
	sceneObjects.push_back(plane5_cube);
	
	Plane *plane6_cube = new Plane(Vector(-6, 6, -55), Vector(-2, 6, -55), Vector(-2, 6, -50), Vector(-6, 6, -50), Color::GREEN);
	sceneObjects.push_back(plane6_cube);
	
	
	//The following are OpenGL functions used only for drawing the pixels
	//of the ray-traced scene.
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(XMIN, XMAX, YMIN, YMAX);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClearColor(0, 0, 0, 1);
}


int main(int argc, char *argv[]) 
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(20, 20);
    glutCreateWindow("Raytracing");

    glutDisplayFunc(display);
    initialize();

    glutMainLoop();
    return 0;
}
