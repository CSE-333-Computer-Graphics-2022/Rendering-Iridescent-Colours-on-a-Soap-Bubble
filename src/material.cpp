#include "world.h"
#include "material.h"
#include <iostream>

Vector3D cie_colour_match[81] = { Vector3D(
0.0014,0.0000,0.0065), Vector3D(0.0022,0.0001,0.0105), Vector3D(0.0042,0.0001,0.0201), Vector3D(0.0076,0.0002,0.0362), Vector3D(0.0143,0.0004,0.0679), Vector3D(0.0232,0.0006,0.1102), Vector3D(0.0435,0.0012,0.2074), Vector3D(0.0776,0.0022,0.3713), Vector3D(0.1344,0.0040,0.6456), Vector3D(0.2148,0.0073,1.0391), Vector3D(0.2839,0.0116,1.3856), Vector3D(0.3285,0.0168,1.6230), Vector3D(0.3483,0.0230,1.7471), Vector3D(0.3481,0.0298,1.7826), Vector3D(0.3362,0.0380,1.7721), Vector3D(0.3187,0.0480,1.7441), Vector3D(0.2908,0.0600,1.6692), Vector3D(0.2511,0.0739,1.5281), Vector3D(0.1954,0.0910,1.2876), Vector3D(0.1421,0.1126,1.0419), Vector3D(0.0956,0.1390,0.8130), Vector3D(0.0580,0.1693,0.6162), Vector3D(0.0320,0.2080,0.4652), Vector3D(0.0147,0.2586,0.3533), Vector3D(0.0049,0.3230,0.2720), Vector3D(0.0024,0.4073,0.2123), Vector3D(0.0093,0.5030,0.1582), Vector3D(0.0291,0.6082,0.1117), Vector3D(0.0633,0.7100,0.0782), Vector3D(0.1096,0.7932,0.0573), Vector3D(0.1655,0.8620,0.0422), Vector3D(0.2257,0.9149,0.0298), Vector3D(0.2904,0.9540,0.0203), Vector3D(0.3597,0.9803,0.0134), Vector3D(0.4334,0.9950,0.0087), Vector3D(0.5121,1.0000,0.0057), Vector3D(0.5945,0.9950,0.0039), Vector3D(0.6784,0.9786,0.0027), Vector3D(0.7621,0.9520,0.0021), Vector3D(0.8425,0.9154,0.0018), Vector3D(0.9163,0.8700,0.0017), Vector3D(0.9786,0.8163,0.0014), Vector3D(1.0263,0.7570,0.0011), Vector3D(1.0567,0.6949,0.0010), Vector3D(1.0622,0.6310,0.0008), Vector3D(1.0456,0.5668,0.0006), Vector3D(1.0026,0.5030,0.0003), Vector3D(0.9384,0.4412,0.0002), Vector3D(0.8544,0.3810,0.0002), Vector3D(0.7514,0.3210,0.0001), Vector3D(0.6424,0.2650,0.0000), Vector3D(0.5419,0.2170,0.0000), Vector3D(0.4479,0.1750,0.0000), Vector3D(0.3608,0.1382,0.0000), Vector3D(0.2835,0.1070,0.0000), Vector3D(0.2187,0.0816,0.0000), Vector3D(0.1649,0.0610,0.0000), Vector3D(0.1212,0.0446,0.0000), Vector3D(0.0874,0.0320,0.0000), Vector3D(0.0636,0.0232,0.0000), Vector3D(0.0468,0.0170,0.0000), Vector3D(0.0329,0.0119,0.0000), Vector3D(0.0227,0.0082,0.0000), Vector3D(0.0158,0.0057,0.0000), Vector3D(0.0114,0.0041,0.0000), Vector3D(0.0081,0.0029,0.0000), Vector3D(0.0058,0.0021,0.0000), Vector3D(0.0041,0.0015,0.0000), Vector3D(0.0029,0.0010,0.0000), Vector3D(0.0020,0.0007,0.0000), Vector3D(0.0014,0.0005,0.0000), Vector3D(0.0010,0.0004,0.0000), Vector3D(0.0007,0.0002,0.0000), Vector3D(0.0005,0.0002,0.0000), Vector3D(0.0003,0.0001,0.0000), Vector3D(0.0002,0.0001,0.0000), Vector3D(0.0002,0.0001,0.0000), Vector3D(0.0001,0.0000,0.0000), Vector3D(0.0001,0.0000,0.0000), Vector3D(0.0001,0.0000,0.0000), Vector3D(0.0000,0.0000,0.0000)
};

Color spec_to_rgb(const Ray& incident, double thickness) {
	thickness *= 800 * 400;

	// spec to xyz
	float lambda = 380.0;
	float X = 0, Y = 0, Z = 0;

	for (int i = 0; lambda < 780.1; i++, lambda += 5) {
		float a = 2 * M_PI * thickness * 1.38f * 0.707f;
		float sina = sin(a/lambda);
		float Me = 4 * 1 * sina * sina;
		X += Me * cie_colour_match[i].X();
		Y += Me * cie_colour_match[i].Y();
		Z += Me * cie_colour_match[i].Z();
        }
    
    float XYZ = (X + Y + Z);
    Vector3D cie = Vector3D(X / XYZ, Y / XYZ, Z / XYZ);

    Vector3D transMat[3];
    transMat[0] = Vector3D( 3.2404542, -1.5371385, -0.4985314);
    transMat[1] = Vector3D(-0.9692660,  1.8760108,  0.0415560);
    transMat[2] = Vector3D( 0.0556434, -0.2040259,  1.0572252);

    Color srgb = Color(dotProduct(transMat[0], cie), dotProduct(transMat[1], cie), dotProduct(transMat[2], cie));
    return srgb *1.5;
}

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
	if (disc < 1e-7) {
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
	Color reflectedColor(0.0, 0.0, 0.0);
	
	// Ambient
	Ia = world->getAmbient() * ka;
	for(int i = 0; i < world->getLightSourceListSize(); i++) {
		LightSource* light = world->getLightSource(i);

		// Shadow checking
		Ray shadowFeeler(incident.getPosition() +
			((light->getPosition() - incident.getPosition())*1e-4),
			light->getPosition() - incident.getPosition());
		world->firstIntersection(shadowFeeler);

		if(!shadowFeeler.didHit() || shadowFeeler.intersected()->getDielectric()){
			//either the shadow feeler didn't hit or it hit a dielectric object

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

	reflectedColor = (lightQty + Ia) * color;
	
	if(!isSolid && incident.getLevel() < 4) {
		Vector3D newNormal = dotProduct(incident.getDirection(), incident.getNormal()) < 0 ? incident.getNormal() : -incident.getNormal();

		double newEta;
		if(dotProduct(incident.getDirection(), incident.getNormal()) < 0)
		{	newEta = eta;} //Entering an object
		else
		{	newEta = (eta <= 1.0 + 1e-4) ? 1.3 : 1.0; } //Exiting-so use outside's eta

		Vector3D refractedDirection = getRefractedDirection(incident, newEta / incident.getEta(), newNormal);

		// check TIR
		if(incident.getEta() > eta) // Denser to Rarer
		{
			if(refractedDirection.X() == 0 && refractedDirection.Y() == 0 && refractedDirection.Z() == 0) {
				// TIR yes
				return reflectedColor;
			}
		}

		Ray refractedRay(incident.getPosition() + refractedDirection*1e-6,
				refractedDirection, incident.getLevel()+1, newEta);
			
		Color refractedColor = world->shade_ray(refractedRay);
		double baseNoise = world->pn.noise(incident.getPosition().X()*2.5, incident.getPosition().Y(), incident.getPosition().Z()*2.5);
		double noise = baseNoise + pow(baseNoise,2) + pow(baseNoise,3);
		double thickness = refractedRay.getParameter() + 0.0003*noise;
		Color bubble(0.0, 0.0, 0.0);
		bubble = eta > 1 && refractedRay.didHit() ? spec_to_rgb(incident, thickness) : Color(1, 1, 1);
		reflectedColor = reflectedColor * bubble;

		Vector3D reflectedDirection = incident.getDirection() -
			incident.getNormal()*2*dotProduct(incident.getDirection(),
			incident.getNormal());
		Ray reflectedRay(incident.getPosition() + reflectedDirection*1e-4,
		reflectedDirection, incident.getLevel()+1, incident.getEta());
		reflectedColor = (reflectedColor + world->shade_ray(reflectedRay)) * km;

		double c = -dotProduct(incident.getDirection(),
			newNormal);
		double R0 = (newEta-incident.getEta()) / (newEta+incident.getEta());
		R0 *= R0;
		double R = R0 + (1.0 - R0) * pow(1.0 - c, 5);
		return (reflectedColor * R + (refractedColor) * (1.0 - R) ) ;
	}
	return reflectedColor;
}
