#pragma once
#include "Graphics.h"
struct sdfResult {
	GGE::vec3 normal;
	GGE::color color;
	float distance;
};
struct floatColor {
	float distance;
	GGE::color color;
};
float sign(float value) {
	if (value > 0) {
		return 1;
	}
	if (value == 0) {
		return 0;
	}
	if (value < 0) {
		return -1;
	}
}
floatColor sUnion(floatColor s1, floatColor s2) {
	if (s1.distance < s2.distance) {
		return s1;
	}
	return s2;
}
//https://iquilezles.org/articles/smin/
floatColor sSmoothUnion(floatColor a, floatColor b, float k)
{
	k *= 4.0;
	float h = max(k - abs(a.distance - b.distance), 0.0) / k;
	float dist = min(a.distance, b.distance) - h * h * k * (1.0 / 4.0);
		
	float m = h * 0.5;
	if (a.distance < b.distance) {
		m = 1.0 - m;
	}
	a.color.mix(b.color,m);
	GGE::color col = a.color;

	return { dist,col };
}
floatColor sGrow(floatColor s1, float g) {
	s1.distance -= g;
	return s1;
}
floatColor sIntersection(floatColor s1, floatColor s2) {
	if (s1.distance > s2.distance) {
		return s1;
	}
	return s2;
}
floatColor sCut(floatColor s1, floatColor s2) {
	if (s1.distance > -s2.distance) {
		return s1;
	}
	return { -s2.distance,s2.color };
}
floatColor sOnion(floatColor s1, float thick) {
	return { abs(s1.distance) - thick,s1.color };
}
GGE::vec3 sMax(GGE::vec3 o1, float o2) {
	return GGE::vec3(
		o1.x > o2 ? o1.x : o2,
		o1.y > o2 ? o1.y : o2,
		o1.z > o2 ? o1.z : o2
	);
}
GGE::vec3 sMin(GGE::vec3 o1, float o2) {
	return GGE::vec3(
		o1.x < o2 ? o1.x : o2,
		o1.y < o2 ? o1.y : o2,
		o1.z < o2 ? o1.z : o2
	);
}
GGE::vec3 abs(GGE::vec3 in) {
	return { abs(in.x),abs(in.y),abs(in.z) };
}
GGE::vec3 mod(GGE::vec3 in, float amount) {

	return { std::fmod(in.x,amount),std::fmod(in.y,amount),std::fmod(in.z,amount) };
}