//sphere.cpp

#include "sphere.h"
Vector3D Sphere::matMulT(Vector3D v) const{
	Vector3D result(
	T[0][0]*v.X() + T[0][1]*v.Y() + T[0][2]*v.Z() + T[0][3]*1.0,
	T[1][0]*v.X() + T[1][1]*v.Y() + T[1][2]*v.Z() + T[1][3]*1.0,
	T[2][0]*v.X() + T[2][1]*v.Y() + T[2][2]*v.Z() + T[2][3]*1.0);
	double d = T[3][0]*v.X() + T[3][1]*v.Y() + T[3][2]*v.Z() + T[3][3]*1.0;
	return result/d;
}
Vector3D Sphere::matMulTp(Vector3D v) const{
	Vector3D result(
	Tp[0][0]*v.X() + Tp[0][1]*v.Y() + Tp[0][2]*v.Z() + Tp[0][3]*1.0,
	Tp[1][0]*v.X() + Tp[1][1]*v.Y() + Tp[1][2]*v.Z() + Tp[1][3]*1.0,
	Tp[2][0]*v.X() + Tp[2][1]*v.Y() + Tp[2][2]*v.Z() + Tp[2][3]*1.0);
	double d = Tp[3][0]*v.X() + Tp[3][1]*v.Y() + Tp[3][2]*v.Z() + Tp[3][3]*1.0;
	return result/d;
}
Vector3D Sphere::matMulTpT(Vector3D v) const{
	Vector3D result(
	Tp[0][0]*v.X() + Tp[1][0]*v.Y() + Tp[2][0]*v.Z() + Tp[3][0]*1.0,
	Tp[0][1]*v.X() + Tp[1][1]*v.Y() + Tp[2][1]*v.Z() + Tp[3][1]*1.0,
	Tp[0][2]*v.X() + Tp[1][2]*v.Y() + Tp[2][2]*v.Z() + Tp[3][2]*1.0);
	double d = Tp[0][3]*v.X() + Tp[1][3]*v.Y() + Tp[2][3]*v.Z() + Tp[3][3]*1.0;
	return result/d;
}


bool Sphere::intersect(Ray& r) const
{
	if(isTransformed) {
		Ray spaceRay(matMulTpT(r.getOrigin()), matMulTpT(r.getDirection()));
		Vector3D newPosition = matMulTpT(position);
		Vector3D centerVector = spaceRay.getOrigin() - newPosition;
		double a = 1.0;
		double b = 2*dotProduct(spaceRay.getDirection(), centerVector);
		double c = dotProduct(centerVector, centerVector) - radius*radius;
		double discriminant = b*b - 4.0*a*c;

		//now check if discr. is posivtive or zero, then only we have an intersection!
		if(discriminant >=0.0)
		{

			if(discriminant == 0)
			{
				double t;
				t = -b/(2.0*a);
				if(r.setParameter(t, this)){
					spaceRay.setParameter(t, this);
					Vector3D normal = spaceRay.getPosition() - newPosition;
					normal = matMulTp(normal);
					normal.normalize();
					r.setNormal(normal);
				}
				return true;
			}
			else
			{
				//Calculate both roots
				double D = sqrt(discriminant);
				double t1 = (-b +D)/(2.0*a);
				double t2 = (-b -D)/(2.0*a);

				bool b1 = r.setParameter(t1, this);
				bool b2 = r.setParameter(t2, this);
				spaceRay.setParameter(t1, this);
				spaceRay.setParameter(t2, this);
				if(b1||b2) {
					Vector3D normal = spaceRay.getPosition() - newPosition;
					normal.normalize();
					normal = matMulTp(normal);
					r.setNormal(normal);
				}
				


				return b1||b2;    
			}
		}
		return false;
	}
	Vector3D centerVector = r.getOrigin() - position;
	double a = 1.0;
	double b = 2*dotProduct(r.getDirection(), centerVector);
	double c = dotProduct(centerVector, centerVector) - radius*radius;
	double discriminant = b*b - 4.0*a*c;

	//now check if discr. is posivtive or zero, then only we have an intersection!
	if(discriminant >=0.0)
	{

		if(discriminant == 0)
		{
			double t;
			t = -b/(2.0*a);
			if(r.setParameter(t, this)){
				Vector3D normal = r.getPosition() - position;
				normal.normalize();
				r.setNormal(normal);
			}
			return true;
		}
		else
		{
			//Calculate both roots
			double D = sqrt(discriminant);
			double t1 = (-b +D)/(2.0*a);
			double t2 = (-b -D)/(2.0*a);

			bool b1 = r.setParameter(t1, this);
			bool b2 = r.setParameter(t2, this);
			if(b1||b2) {
				Vector3D normal = r.getPosition() - position;
				normal.normalize();
				r.setNormal(normal);
			}
			


			return b1||b2;    
		}
	}
	return false;

}
