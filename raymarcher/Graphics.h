#pragma once


#include <windows.h>

#include <GL/gl.h>

#include <GLFW/glfw3.h>
#include <iostream>
#include <thread>
#include <chrono>
#pragma comment(lib, "opengl32.lib")

namespace GGE {
	namespace math {
		float Lerp(float a, float b, float i) {
			return (a * i) + (b * (1 - i));
		}
		float clamp(float val,float min, float max) {
			if (val > max) {
				return max;
			}
			if (val < min) {
				return min;
			}

			return val;
		}
	}
	class color {
	public:
		float R;
		float G;
		float B;
		float A;
		color(float _R = 0, float _G = 0, float _B = 0, float _A = 1) {
			R = _R; G = _G;B = _B;A = _A;
		}
		color& operator*=(float scalar) {
			R *= scalar;
			G *= scalar;
			B *= scalar;
			return *this;
		}
		color& operator+=(color other) {
			R += other.R;
			G += other.G;
			B += other.B;
			return *this;
		}
		void mix(color c, float i) {
			R = math::Lerp(R, c.R, i);
			G = math::Lerp(G, c.G, i);
			B = math::Lerp(B, c.B, i);
		}
	};
	enum class axis {
		x,
		y,
		z
	};
	class vec2 {
	public:
		vec2() {
			x = 0;
			y = 0;
		}
		vec2(float _x, float _y) {
			x = _x;
			y = _y;
		}
		vec2(const vec2& VEC2) {
			x = VEC2.x;
			y = VEC2.y;
		}
		vec2 operator/(float scalar) const {
			if (scalar == 0.0f) {
				return vec2(0.0f, 0.0f);
			}
			return vec2(x / scalar, y / scalar);
		}
		vec2 operator+(vec2 other) const {
			return vec2(x + other.x, y + other.y);
		}
		vec2 operator-(vec2 other) const {
			return vec2(x - other.x, y - other.y);
		}
		vec2 operator*(float scalar) const {
			return vec2(x * scalar, y * scalar);
		}
		vec2& operator+=(vec2 other) {
			x += other.x;
			y += other.y;
			return *this;
		}
		vec2& operator/=(float scalar) {
			x /= scalar;
			y /= scalar;
			return *this;
		}
		

		float x, y;
		float mag() {
			return sqrt(x * x + y * y);
		}
		void normalize() {
			float m = mag();
			x /= m;
			y /= m;
		}
		float Dot(vec2 other) {
			return x * other.x + y * other.y;
		}
	
	};
	class vec3 {
	public:
		vec3() {
			x = 0;
			y = 0;
			z = 0;
		}
		vec3(float _x, float _y, float _z) {
			x = _x;
			y = _y;
			z = _z;
		}
		vec3(const vec3& VEC3) {
			x = VEC3.x;
			y = VEC3.y;
			z = VEC3.z;
		}
		vec3 operator/(float scalar) const {
			if (scalar == 0.0f) {
				return vec3(0.0f, 0.0f, 0.0f);
			}
			return vec3(x / scalar, y / scalar, z/ scalar);
		}
		vec3 operator+(vec3 other) const {
			return vec3(x + other.x, y + other.y, z + other.z);
		}
		vec3 operator+(float other) const {
			return vec3(x + other, y + other, z + other);
		}
		vec3 operator-(float other) const {
			return vec3(x - other, y - other, z - other);
		}
		vec3 operator-(vec3 other) const {
			return vec3(x - other.x, y - other.y, z - other.z);
		}
		vec3 operator*(float scalar) const {
			return vec3(x * scalar, y * scalar, z * scalar);
		}
		vec3& operator+=(vec3 other) {
			x += other.x;
			y += other.y;
			z += other.z;
			return *this;
		}
		vec3& operator*=(float scalar) {
			x *= scalar;
			y *= scalar;
			z *= scalar;
			return *this;
		}
		bool operator==(vec3 other) {
			return (x == other.x) && (y == other.y) && (z == other.z);
		}
		bool operator!=(vec3 other) {
			return !((x == other.x) && (y == other.y) && (z == other.z));
		}
		vec3& operator/=(float scalar) {
			x /= scalar;
			y /= scalar;
			z /= scalar;
			return *this;
		}
		float x, y, z;
		float mag2() {
			return x * x + y * y + z * z;
		}
		float mag() {
			return sqrt(x * x + y * y + z * z);
		}
		void normalize() {
			float m = mag();
			x /= m;
			y /= m;
			z /= m;
		}
		float Dot(vec3 other) {
			return x * other.x + y * other.y + z * other.z;
		}
		void rotate(float xz, float yz) {
			rotYZ(yz);
			rotXZ(xz);
			
		}
	private:
		void rotXZ(float theta) {
			float i, j;
			i = x * cos(theta) - z * sin(theta);
			j = x * sin(theta) + z * cos(theta);
			x = i;z = j;
		}
		void rotYZ(float theta) {
			float i, j;
			i = z * cos(theta) - y * sin(theta);
			j = z * sin(theta) + y * cos(theta);
			z = i;y = j;
		}
	};
	class Window
	{
	private:
		double last_update_time;
		GLFWwindow* window;
		int frameRateGoal = 60;
	public:
		inline ~Window() {
			if (window) {
				glfwDestroyWindow(window);
			}
		}
		// constructor function for the window
		inline Window(int Width = 800, int Height = 600, const char* title = "window") {
			//check for glfw initialization
			if (!glfwInit()) {
				std::cerr << "Failed to initialize GLFW" << std::endl;
			}

			//create window
			window = glfwCreateWindow(Width, Height, title, NULL, NULL);

			//check for window creation
			if (!window) {
				std::cerr << "Failed to create GLFW window" << std::endl;
			}

			//make the window's context current
			glfwMakeContextCurrent(window);
			int width, height;
			glfwGetFramebufferSize(window, &width, &height); // Grab your window's actual size

			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			// Maps (0,0) to Top-Left corner, and (width, height) to Bottom-Right corner
			glOrtho(0.0, width, height, 0.0, -1.0, 1.0);

			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			//clean up
			std::atexit([]() {
				glfwTerminate();
				});
			update();
		}
		inline void clear(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
			glClearColor(r, g, b, a);
			glClear(GL_COLOR_BUFFER_BIT);
		}
		inline bool shouldClose() {
			return glfwWindowShouldClose(window);
		}
		inline void update() {
			glfwSwapBuffers(window);
			glfwPollEvents();
			float targetMs = 1000.0f / frameRateGoal;
			float sleepMs = targetMs - (getDT() * 1000.0f);
			last_update_time = glfwGetTime();
			if (sleepMs > 0.0f && frameRateGoal!=NULL) {
				std::this_thread::sleep_for(std::chrono::milliseconds((long long)sleepMs));
			}

		}
		inline void drawTriangle(vec2 v1, vec2 v2, vec2 v3) {
			glfwMakeContextCurrent(window);
			glfwMakeContextCurrent(window);
			glBegin(GL_TRIANGLES);
			glVertex2f(v1.x, v1.y);
			glVertex2f(v2.x, v2.y);
			glVertex2f(v3.x, v3.y);
			glEnd();

		}
		inline void setColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
			glColor4f(r, g, b, a);
		}
		inline void setColor(color col) {
			glColor4f(col.R, col.G, col.B, col.A);
		}
		inline void setContext() {
			glfwMakeContextCurrent(window);
		}
		inline void drawRectangle(vec2 v1, vec2 size,bool optimized = false) {
			if (!optimized) {
				glBegin(GL_QUADS);
			}
			glVertex2f(v1.x, v1.y + size.y);
			glVertex2f(v1.x + size.x, v1.y + size.y);
			glVertex2f(v1.x + size.x, v1.y);
			glVertex2f(v1.x, v1.y);
			if (!optimized) {
				glBegin(GL_QUADS);
			}
		}
		inline void drawPoint(vec2 pos) {
			glBegin(GL_POINTS);
			glVertex2f(pos.x,pos.y);
			glEnd();
		}
		inline void drawCircle(vec2 pos, float rad) {
			glBegin(GL_TRIANGLE_FAN);
			glVertex2f(pos.x, pos.y);
			for (float a = 0.0f;a <= 360;a++) {
				glVertex2f(
					rad * cos(a / 57.2957795131f) + pos.x,
					rad * sin(a / 57.2957795131f) + pos.y
				);
			}
			glEnd();
		}
		inline int getKeyState(int key) {
			return glfwGetKey(window, key);
		}
		inline void movement(float& x, float& y, float speed = 4) {
			if (getKeyState(GLFW_KEY_RIGHT)) { x += speed; }
			if (getKeyState(GLFW_KEY_LEFT)) { x -= speed; }
			if (getKeyState(GLFW_KEY_DOWN)) { y += speed; }
			if (getKeyState(GLFW_KEY_UP)) { y -= speed; }
		}
		inline GLFWwindow* getWindowContext() {
			return window;
		}
		inline void setFrameRateGoal(int FPS) { frameRateGoal = FPS; }
		inline double getDT() {
			double current_time = glfwGetTime();
			double delta_time = current_time - last_update_time;
			return delta_time;
		}
		//NOT IMPLEMENTED
		inline void drawText(char text) {
			//DO LATER
		}
		inline vec2 get_window_dimensions() {
			int x = 0;
			int y = 0;
			glfwGetWindowSize(window, &x, &y);
			return vec2((float)x, (float)y);

		}
	};
}