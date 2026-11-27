#include "Graphics.h"

#include <omp.h>
#include <math.h>
#include "SdfFunctions.h"
#include "SdfPrimitives.h"
#pragma region defines
#define PI 3.14159f
#define cas(x,y) (((x-1)/x)*y)
#define SHADOW
#define SPEC
#define NORMALLIGHT
#define NO_DITHER
#define NO_FOG
#define fogStart 20.0f
#define fogEnd 5000.0f

#define WINDOW_SIZE 600
#define DRAW_SCALE 2
#define DRAW_SIZE WINDOW_SIZE / DRAW_SCALE

const float epsilon = 0.01f;
const float MAX_DIST = 100000.0f;
const int MAX_STEPS = 10000;

#pragma endregion



GGE::vec3 Light{ -1,1,1 };
float Time = 0;

floatColor sceneSdf(GGE::vec3 pos) {
	floatColor BodySphere1 = sphereSdf(pos, { 0,0,0 }, 100, { 1,1,1,1 });
	floatColor BodySphere2 = sphereSdf(pos, { 0,-150,0 }, 75, { 1,1,1,1 });
	floatColor BodySphere3 = sphereSdf(pos, { 0,-250,0 }, 50, { 1,1,1,1 });
	floatColor EyeSphere1 = sphereSdf(pos, { 40,-270,-20 }, 10, { 0.2,0.2,0.2,1 });
	floatColor EyeSphere2 = sphereSdf(pos, { 40,-270,20 }, 10, { 0.2,0.2,0.2,1 });
	floatColor MouthSphere1 = sphereSdf(pos, { 45,-240,20 }, 5, { 0.2,0.2,0.2,1 });
	floatColor MouthSphere2 = sphereSdf(pos, { 45,-240,-20 }, 5, { 0.2,0.2,0.2,1 });
	floatColor MouthSphere3 = sphereSdf(pos, { 45,-230,0 }, 5, { 0.2,0.2,0.2,1 });
	
	floatColor Body = sSmoothUnion(sSmoothUnion(BodySphere1, BodySphere2, 2), BodySphere3, 2);
	floatColor Eyes = sUnion(EyeSphere1, EyeSphere2);
	floatColor Mouth = sUnion(MouthSphere1, sUnion(MouthSphere2, MouthSphere3));

	floatColor snowMan = sSmoothUnion(Body, sUnion(Mouth, Eyes),1);

	return snowMan;
}

GGE::vec3 calculateNormals(GGE::vec3 pos,float fS) {
	
	GGE::vec3 x{ 0.1,0,0 };
	GGE::vec3 y{ 0,0.1,0 };
	GGE::vec3 z{ 0,0,0.1 };

	float fX = fS - sceneSdf(pos + x).distance;
	float fY = fS - sceneSdf(pos + y).distance;
	float fZ = fS - sceneSdf(pos + z).distance;

	GGE::vec3 normal{ -fX,-fY,-fZ };

	normal.normalize();

	return normal;
}

sdfResult sceneFunction(GGE::vec3 pos) {
	floatColor SDF = sceneSdf(pos);
	return { calculateNormals(pos,SDF.distance),SDF.color,SDF.distance};
}
struct Ray {
	GGE::vec3 origin;
	GGE::vec3 direction;
	void step(float amount) {
		origin += direction * amount;
	}
	bool isColliding() {
		
		return (sceneSdf(origin).distance < epsilon);
	}
	Ray(GGE::vec3 _origin, GGE::vec3 _direction) {
		origin = _origin;
		direction = _direction;
		direction.normalize();
	}
};
sdfResult skyBox(GGE::vec3 esRay) {
	if (esRay.y <= 0) {
		return { {Light * -1},{ 0.1451f,0.9333f,0.9608f,1.0f },1000};
	}
	return { {Light * -1},{ 0.6f,0.6f,0.6f,1.0f},1000};
}
sdfResult marchRay(Ray& ray) {
	
	float currDist = 0.0f;
	int steps = 0;
	while (true) {
		float dist = sceneSdf(ray.origin).distance;
		currDist += dist;
		if (currDist > MAX_DIST || steps > MAX_STEPS) {

			sdfResult sdf = skyBox(ray.direction);
			return sdf;
		}
		if (ray.isColliding()) {
			sdfResult sdf = sceneFunction(ray.origin);
			return sdf;
		}
		steps++;
		ray.step(dist);
	}
	sdfResult sdf = skyBox(ray.direction);
	return sdf;
}
float softshadow(Ray& ray, float mint, float maxt, float k)
{
	float res = 1.0;
	float t = mint;
	for (int i = 0; i < 32 && t < maxt; i+=1)
	{
		float h = sceneSdf(ray.origin + ray.direction * t).distance;
		if (h < 0.005f) {
			return 0.0;
		}
			
		res = min(res, k * h / t);
		t += h;
	}
	return res;
}
int main() {
	GGE::Window wind{ WINDOW_SIZE,WINDOW_SIZE,"Ray Marching Test"};

	wind.setFrameRateGoal(500);
	float FPS  =0;
	GLuint screenTexture;
	glGenTextures(1, &screenTexture);
	glBindTexture(GL_TEXTURE_2D, screenTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	GGE::vec3 camera{ 200,200,0 };
	
	Light.normalize();

	float FOV = 1.04;
	float DT=0;
	float moveSpeed = 250;
	float XZ = 0;
	float YZ = 0;
	const GGE::vec2 screenDim = wind.get_window_dimensions();
	const GGE::vec2 screenMid = screenDim / 2;

	float focalLength = screenDim.x / (2 * tanf(FOV / 2.0f));

	
	while (!wind.shouldClose()) {

		static GGE::color buffer[DRAW_SIZE][DRAW_SIZE];

#ifndef _OPENMP
#error OPENMP_IS_OFF
#endif
#pragma omp parallel for schedule(dynamic, 4)
		for (int j = 0;j < DRAW_SIZE; j++) {
			
			for (int i = 0;i < DRAW_SIZE; i++) {

				int x = i - (DRAW_SIZE/2);
				int y = j - (DRAW_SIZE/2);
				x *= DRAW_SCALE;
				y *= DRAW_SCALE;

				GGE::vec3 Ra{(float)x,(float)y,focalLength};
				Ra.rotate(XZ,YZ);
				//Ra.normalize();

				GGE::vec3 H = Light + Ra;
				H.normalize();

				Ray r{ camera, Ra};

				sdfResult re = marchRay(r);

				float disttraveled = (r.origin - camera).mag();

				r.direction =Light;
				r.direction *= -1;
				float shadow = 1;
				
				GGE::color specColor = GGE::color(0.0f, 0.0f, 0.0f, 1.0f);
				if (re.normal != Light * -1) {
					specColor = GGE::color(1.0f, 1.0f, 1.0f, 1.0f);
#ifdef SHADOW
					shadow = softshadow(r, 2, 1000, 4);
#endif // NO_SHADOW
				}
				GGE::color res = re.color;
				float lighting = re.normal.Dot(Light * -1);
#ifndef NORMALLIGHT
				lighting = 1;
#endif // !NORMALLIGHT
				float shine = H.Dot(re.normal);
				shine = pow(shine, 8) * shadow * 0.2f;
#ifndef SPEC
				shine = 0;
#endif // !NORMALLIGHT
				lighting += 3;
				lighting /= 4;
				lighting *= lighting;// * lighting;
				res *= lighting;
				res *= (shadow+0.5)/1.5;
				
				specColor *= shine;
				res += specColor;
#ifdef DITHER
				float dither = ((float)(rand() % 100) / 100.0f - 0.5f) * 0.4f;

				res.R += dither;
				res.G += dither;
				res.B += dither;
#endif
				res.mix(skyBox({0,-1,0}).color, 0.9);
				res *= 1.1;
#ifdef FOG
				float fogFactor = (disttraveled - fogStart) / (fogEnd - fogStart);
				fogFactor = GGE::math::clamp(1 - fogFactor, 0.0f, 1.0f);
				res.mix({ 0.6,0.6,0.6 }, fogFactor);
#endif
				buffer[j][i] = res;
				//wind.drawRectangle(GGE::vec2((float)x, (float)y) + screenMid, GGE::vec2((float)x+1, (float)y +1) + screenMid,true);
			}
		}
		
		glBindTexture(GL_TEXTURE_2D, screenTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, DRAW_SIZE, DRAW_SIZE, 0, GL_RGBA, GL_FLOAT, buffer);
		
		
		glEnable(GL_TEXTURE_2D);
		glColor3f(1.0f, 1.0f, 1.0f);

		glBegin(GL_QUADS);
		
		glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, 0.0f);
		
		glTexCoord2f(1.0f, 0.0f); glVertex2f(WINDOW_SIZE, 0.0f);

		glTexCoord2f(1.0f, 1.0f); glVertex2f(WINDOW_SIZE, WINDOW_SIZE);

		glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f, WINDOW_SIZE);
		glEnd();

		glDisable(GL_TEXTURE_2D);
		DT= wind.getDT();
		#pragma region movement
		if (wind.getKeyState(GLFW_KEY_LEFT)) {
			XZ += 1 * DT;
		}
		if (wind.getKeyState(GLFW_KEY_RIGHT)) {
			XZ -= 1 * DT;
		}
		if (wind.getKeyState(GLFW_KEY_UP)) {
			YZ -= 1 * DT;
		}
		if (wind.getKeyState(GLFW_KEY_DOWN)) {
			YZ += 1 * DT;
		}
		if (wind.getKeyState(GLFW_KEY_W)) {
			GGE::vec2 dir(cos(XZ), sin(XZ));
			dir = dir * DT * moveSpeed;
			camera += {-dir.y, 0, dir.x};
		}
		if (wind.getKeyState(GLFW_KEY_S)) {
			GGE::vec2 dir(cos(XZ), sin(XZ));
			dir = dir * DT * moveSpeed;
			camera += {dir.y, 0, -dir.x};
		}
		if (wind.getKeyState(GLFW_KEY_A)) {
			GGE::vec2 dir(cos(XZ), sin(XZ));
			dir = dir * DT * moveSpeed;
			camera += {-dir.x, 0, -dir.y};
		}
		if (wind.getKeyState(GLFW_KEY_D)) {
			GGE::vec2 dir(cos(XZ), sin(XZ));
			dir = dir * DT * moveSpeed;
			camera += {dir.x, 0, dir.y};
		}
		if (wind.getKeyState(GLFW_KEY_E)) {
			camera += {0, -DT * moveSpeed, 0};
		}
		if (wind.getKeyState(GLFW_KEY_Q)) {
			camera += {0, DT * moveSpeed, 0};
		}
		YZ = GGE::math::clamp(YZ, -PI / 2.0f, PI / 2.0f);
#pragma endregion

		
		Time += DT;
		FPS = FPS + 1 / wind.getDT();
		FPS /= 2;
		std::cout << "FPS: " << FPS << "\n";
		wind.update();
	}
}