#pragma once
#include "SdfFunctions.h"
floatColor sphereSdf(GGE::vec3 pos, GGE::vec3 spherePos, float radius, GGE::color color) {
	GGE::vec3 offset = pos - spherePos;
	float distance = offset.mag() - radius;
	return { distance ,color };
}
floatColor PlaneSdf(GGE::vec3 pos, GGE::axis ax, float pPos, float orientation, GGE::color color) {
	switch (ax) {
	case GGE::axis::x: return { orientation * (pos.x - pPos), color };
	case GGE::axis::y: return { orientation * (pos.y - pPos), color };
	case GGE::axis::z: return { orientation * (pos.z - pPos), color };
	}
}
floatColor boxSdf(GGE::vec3 pos, GGE::vec3 bp, GGE::vec3 bs, GGE::color color) {
	GGE::vec3 q = abs(pos - bp) - bs;
	return { sMax(q, 0.0).mag() + min(max(q.x, max(q.y, q.z)), 0.0f),color };
}
///https://iquilezles.org/articles/distfunctions/
floatColor boxFrameSdf(GGE::vec3 p, GGE::vec3 boxPos, GGE::vec3 b, float e, GGE::color color)
{
	p = abs(p - boxPos) - b;
	GGE::vec3 q = abs(p + GGE::vec3(e, e, e)) - GGE::vec3(e, e, e);

	GGE::vec3 zero{ 0.0f, 0.0f, 0.0f };

	float edgeX = sMax(GGE::vec3(p.x, q.y, q.z), 0).mag() + min(max(p.x, max(q.y, q.z)), 0.0f);

	float edgeY = sMax(GGE::vec3(q.x, p.y, q.z), 0).mag() + min(max(q.x, max(p.y, q.z)), 0.0f);

	float edgeZ = sMax(GGE::vec3(q.x, q.y, p.z), 0).mag() + min(max(q.x, max(q.y, p.z)), 0.0f);

	return { min(min(edgeX, edgeY), edgeZ),color };
}
floatColor sdOctahedron(GGE::vec3 p, GGE::vec3 Os, float s, GGE::color col)
{
	p = abs(p - Os);
	float m = p.x + p.y + p.z - s;
	GGE::vec3 q;
	if (3.0 * p.x < m) q = p;
	else if (3.0 * p.y < m) q = { p.y,p.z,p.x };
	else if (3.0 * p.z < m) q = { p.z,p.x,p.y };
	else return { m * 0.57735027f ,col };

	float k = GGE::math::clamp(0.5 * (q.z - q.y + s), 0.0, s);
	return { (GGE::vec3(q.x, q.y - s + k, q.z - k)).mag() , col };
}