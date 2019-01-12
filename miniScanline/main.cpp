#include <iostream>
#include "Scene.h"
#include "Scanline.h"
#include "Render.h"
#include <ctime>
using namespace std;
using namespace SL;
string  fileName;
void help() {
	cout << "Help:" << endl;
	cout << "  cmd : miniScanline.exe [objFile]" << endl;
	cout << "  e.g. > miniScanline.exe obj/teapot.obj" << endl;
	cout << endl;
}
int main(int argc, char* argv[])
{
	if (argc == 2) {
		fileName = argv[1];
	}
	else {
		help();
		return 0;
	}

	// ���ڴ�С
	int width = 800, height = 600;	
	// ��ȡģ��
	Scene scene;
	scene.Load(fileName);
	// ��ɫ��������Ƭ��ɫ��
	Render::shader(scene);

	Scanline scanline;
	scanline.setSize(width, height);
	scanline.initProj(scene);

	Render render(&scene, &scanline);
	render.display();
	
	return 0;
}