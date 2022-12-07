#include "world.h"
#include "material.h"
#include <iostream>
Vector3D getRefractedDirection(const Ray& incident,
							   const double eta,
							   const Vector3D normal) {
	// direction of refracted ray
	double cosTheta =
	dotProduct(incident.getDirection(), normal);
	Vector3D refractedDirection =
	(incident.getDirection() -
		(normal * cosTheta)) / eta;
	double disc = 1.0 - ((1.0-pow(cosTheta,2))/pow(eta,2));
	if (disc < 1e-4) {
		return Vector3D(0.0, 0.0, 0.0);
	}
	refractedDirection = refractedDirection - (normal *
				sqrt(disc));
	refractedDirection.normalize();
	return refractedDirection;
}
Color Material::shade(const Ray& incident, const bool isSolid) const
{
	
	Color Ia(0.0, 0.0, 0.0);
	Color Id(0.0, 0.0, 0.0);
	Color Is(0.0, 0.0, 0.0);
	Color lightQty(0.0, 0.0, 0.0);
	// Ambient
	Ia = world->getAmbient() * ka;
	for( int i = 0; i < world->getLightSourceListSize(); i++) {
		LightSource* light = world->getLightSource(i);
		// Shadow checking
		Ray shadowFeeler(incident.getPosition() +
			((light->getPosition() - incident.getPosition())*1e-4),
			light->getPosition() - incident.getPosition());
		world->firstIntersection(shadowFeeler);
		if(!shadowFeeler.didHit()){
			// Diffuse
			Vector3D lightPos = light->getPosition();
			Vector3D l = light->getPosition() - incident.getPosition();
			l.normalize();
			double cos = dotProduct(l, incident.getNormal());
			if(cos < 1e-4) {
				cos = 0;
			}
			Id = light->getIntensity() * (cos * kd);
			
			// Specular
			Vector3D h = l - incident.getDirection();
			h.normalize();
			cos = dotProduct(h, incident.getNormal());
			if(cos < 1e-4) {
				cos = 0;
			}
			cos = pow(cos, n*n);
			Is = light->getIntensity() * ks * cos;
			lightQty = lightQty +  Id + Is;
		}
	}
	Color reflectedColor = (lightQty + Ia) * color;
	
	if(!isSolid && incident.getLevel() < 4) {
		double c = 0;
		Color kb(0.0, 0.0, 0.0);
		Color refractedColor(0.0, 0.0, 0.0);
		if(dotProduct(incident.getDirection(), incident.getNormal())
			< 0) {
			// Entering the Dielectric
			Vector3D refractedDirection = getRefractedDirection(incident, eta,
				incident.getNormal());
			Ray refractedRay(incident.getPosition() + refractedDirection*1e-4,
				refractedDirection);
			refractedRay.setLevel(incident.getLevel()+1);
			
			refractedColor = world->shade_ray(refractedRay);
			
			c = -dotProduct(incident.getDirection(),
				incident.getNormal());
			kb = Color(1.0, 1.0, 1.0);

		}
		else {
			// Exiting Dielectric
			kb.R(pow(2.71828, -incident.getParameter()*0.3));
			kb.G(pow(2.71828, -incident.getParameter()*0.1));
			kb.B(pow(2.71828, -incident.getParameter()*0.3));
			// check TIR
			Vector3D refractedDirection = getRefractedDirection(incident, 1.0/eta,
				-incident.getNormal());
			if(refractedDirection.X() == 0 && refractedDirection.Y() == 0 && refractedDirection.Z() == 0) {
				// TIR yes
				return reflectedColor*kb;
			}
			c = dotProduct(incident.getDirection(), incident.getNormal());
			Ray refractedRay(incident.getPosition() + refractedDirection*1e-4,
				refractedDirection);
			refractedRay.setLevel(incident.getLevel()+1);
			
			refractedColor = world->shade_ray(refractedRay);
			

		}
		Vector3D reflectedDirection = incident.getDirection() -
			incident.getNormal()*2*dotProduct(incident.getDirection(),
			incident.getNormal());
		Ray reflectedRay(incident.getPosition() + reflectedDirection*1e-4,
		reflectedDirection);
		reflectedRay.setLevel(incident.getLevel()+1);
		reflectedColor = world->shade_ray(reflectedRay);

		double R = R0 + (1.0 - R0) * pow(1.0 - c, 5);
		return reflectedColor * R + refractedColor * (1.0 - R) * kb ;
		
	}
	return reflectedColor;
}
