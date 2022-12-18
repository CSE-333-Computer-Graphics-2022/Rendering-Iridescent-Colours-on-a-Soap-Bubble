//sphere.h
#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_

#include "object.h"
#include "ray.h"
#include "vector3D.h"
#include "color.h"

class Triangle : public Object
{
private:
	Vector3D positionA, positionB, positionC;

public:
	Triangle(const Vector3D& _positionA, const Vector3D& _positionB, const Vector3D& _positionC, Material* mat):
		Object(mat), positionA(_positionA), positionB(_positionB), positionC(_positionC)
	{
		isSolid = true;
	}
	virtual bool intersect(Ray& r) const;
};
#endif
