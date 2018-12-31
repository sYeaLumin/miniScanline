#include <iostream>
#include "Scene.h"
#include "Scanline.h"
#include "Render.h"
#include <ctime>

using namespace std;
using namespace SL;
int main()
{
	//窗口大小
	int width = 800, height = 600;	
	// 读取模型
	string  fileName = "obj//teapot0.obj";
	Scene scene;
	scene.Load(fileName);
	//scene.fitWindow(width, height);
	Render::shader(scene);

	Scanline scanline;
	scanline.setSize(width, height);
	scanline.initProj(scene);
	scanline.render(scene);

	Render render(&scene, &scanline);
	render.display();
	
	return 0;
}