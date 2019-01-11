#ifndef __DISPLAY_H
#define __DISPLAY_H

#include <iostream>
#include <iostream>
#include <cmath>
#include "glut.h"
#include "Scene.h"
#include "Scanline.h"
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

	// 生成模型的颜色
	static void shader(Scene& model);
	// render
	static void loop();
	static void reshape(int w, int h);
	static void keyboard(int key, int x, int y);
	static void mouse(int button, int state, int x, int y);
	static void MotionFunc(int x, int y);
	static void trackBallPos(int x, int y, glm::vec3 & p);
};


#endif