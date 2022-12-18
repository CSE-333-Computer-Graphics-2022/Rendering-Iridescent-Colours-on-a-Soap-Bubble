//triangle.cpp

#include "triangle.h"

// This functions finds the determinant of Matrix
double calculateDeterminant(double mat[3][3])
{
    double ans;
    ans = mat[0][0] * (mat[1][1] * mat[2][2] - mat[2][1] * mat[1][2])
          - mat[0][1] * (mat[1][0] * mat[2][2] - mat[1][2] * mat[2][0])
          + mat[0][2] * (mat[1][0] * mat[2][1] - mat[1][1] * mat[2][0]);
    return ans;
}

bool Triangle::intersect(Ray& r) const
{

	double A[3][3] = {
		{ positionA[0] - positionB[0], positionA[0] - positionC[0], r.getDirection()[0]},
		{ positionA[1] - positionB[1], positionA[1] - positionC[1], r.getDirection()[1]},
		{ positionA[2] - positionB[2], positionA[2] - positionC[2], r.getDirection()[2]}
	};

	double b[3][3] = {
		{ positionA[0] - r.getOrigin()[0], positionA[0] - positionC[0], r.getDirection()[0]},
		{ positionA[1] - r.getOrigin()[1], positionA[1] - positionC[1], r.getDirection()[1]},
		{ positionA[2] - r.getOrigin()[2], positionA[2] - positionC[2], r.getDirection()[2]}
	};

	double g[3][3] = {
		{ positionA[0] - positionB[0], positionA[0] - r.getOrigin()[0], r.getDirection()[0]},
		{ positionA[1] - positionB[1], positionA[1] - r.getOrigin()[1], r.getDirection()[1]},
		{ positionA[2] - positionB[2], positionA[2] - r.getOrigin()[2], r.getDirection()[2]}
	};

	double t[3][3] = {
		{ positionA[0] - positionB[0], positionA[0] - positionC[0], positionA[0] - r.getOrigin()[0]},
		{ positionA[1] - positionB[1], positionA[1] - positionC[1], positionA[1] - r.getOrigin()[1]},
		{ positionA[2] - positionB[2], positionA[2] - positionC[2], positionA[2] - r.getOrigin()[2]}
	};

	double A_det = calculateDeterminant(A);
	double Beta = calculateDeterminant(b)/A_det;
	double Gamma = calculateDeterminant(g)/A_det;
	double Parameter = calculateDeterminant(t)/A_det;

	if(Beta >= 0 && Gamma >= 0 && Beta + Gamma <= 1 && r.setParameter(Parameter, this)) {

		Vector3D s1 = positionA - positionB;
		Vector3D s2 = positionB - positionC;

		Vector3D normal = crossProduct(s1, s2);
		normal.normalize();
		r.setNormal(normal);
		return true;
	}




	return false;

}
