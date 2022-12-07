//object.h
#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "ray.h"
#include "vector3D.h"
#include "color.h"
#include "material.h"

class Object
{
protected:
	Material *material;
	bool isSolid;
public:
	Object(Material *mat): material(mat) {}	
	bool isTransformed = false;
	double T[4][4];
	double Tp[4][4];
	virtual bool intersect(Ray& ray) const = 0;
	virtual Color shade(const Ray& ray ) const
	{
		return material->shade(ray, isSolid);
	}
	void isDielectric(){isSolid = false;}
	bool getDielectric() const {return !isSolid;}
};

#endif
