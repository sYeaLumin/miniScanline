#include <iostream>
#include "Scene.h"
#include "Scanline.h"
#include "Render.h"
#include <ctime>

using namespace std;
using namespace SL;
int main()
{
	//���ڴ�С
	int width = 400, height = 300;	
	// ��ȡģ��
	string  fileName = "obj//teapot.obj";
	Scene scene;
	scene.Load(fileName);
	scene.Resize(width, height);
	Render::shader(scene);

	Scanline scanline;
	scanline.setSize(width, height);
	scanline.render(scene);

	Render render(&scene, &scanline);
	render.display();
	
	return 0;
}