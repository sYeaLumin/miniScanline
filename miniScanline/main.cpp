#include <iostream>
#include "Scene.h"
#include "Scanline.h"
#include "Render.h"
#include <ctime>
using namespace std;
using namespace SL;
string  fileName = "obj//bunny.obj";
void help() {

}
int main(int argc, char* argv[])
{
	if (argc == 2) {
		fileName = argv[1];
	}
	else if (argc > 2) {

	}
	// 窗口大小
	int width = 800, height = 600;	
	// 读取模型
	Scene scene;
	scene.Load(fileName);
	// 着色（分配面片颜色）
	Render::shader(scene);

	Scanline scanline;
	scanline.setSize(width, height);
	scanline.initProj(scene);

	Render render(&scene, &scanline);
	render.display();
	
	return 0;
}