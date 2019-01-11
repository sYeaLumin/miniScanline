#pragma once
#include <iostream>
#include <iostream>
#include <cmath>
#include <time.h>
#include <Windows.h>
#include "glut.h"
#include "Scene.h"
#include "Scanline.h"
#define MAX_CHAR        128
using namespace SL;
class Render
{
public:
	Render(Scene* model, Scanline* slzBuffer);
	~Render();
	void display();

	static Scene* scene;
	static Scanline* engine;
	static bool isLeftDown;
	static glm::vec3 lastBallPos;
	static glm::vec3 currBallPos;
	static double costTime;

	// 生成模型的颜色
	static void shader(Scene& model);
	// render
	static void loop();
	static void reshape(int w, int h);
	static void keyboard(int key, int x, int y);
	static void mouse(int button, int state, int x, int y);
	static void MotionFunc(int x, int y);
	static void trackBallPos(int x, int y, glm::vec3 & p);
	static void drawString(const char* str);
};
