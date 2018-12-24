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
	static glm::mat4 RotateMat;
	static int idx1, idx2;

	// ����ģ�͵���ɫ
	//const float kd = 0.8;//albedo ,ɢ�䷴����
	static void shader(Scene& model);
	
	static void loop();
	static void reshape(int w, int h);
	static void keyboard(int key, int x, int y);
};


#endif