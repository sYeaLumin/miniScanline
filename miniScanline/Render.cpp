#include "Render.h"
using namespace std;

Scanline* Render::engine = NULL;
Scene* Render::scene = NULL;
bool Render::isLeftDown = false;
glm::vec3 Render::lastBallPos = glm::vec3();
glm::vec3 Render::currBallPos = glm::vec3();
double Render::costTime = 0;

Render::Render(Scene* scene, Scanline* slzBuffer)
{
	this->scene = scene;
	engine = slzBuffer;
}

Render::~Render()
{
}

void Render::shader(Scene& scene) {
	const float kd = 0.8;
	glm::vec3 lightColor = glm::vec3(0.5, 0.5, 0.5);
	glm::vec3 ambientColor = glm::vec3(0.2, 0.2, 0.2);
	glm::vec3 lightPos = glm::vec3(400.0f, 400.0f, 600.0f);
	for (auto &f : scene.fList) {
		for (int j = 0; j < f.vIdx.size(); ++j)
		{
			glm::vec3 rayDir = glm::normalize(lightPos - scene.vList[f.vIdx[j]].pOri);
			glm::vec3 normal = scene.ifFNIdx ?
				scene.vnList[f.nIdx[j]] : f.normal;
			float cosine = dot(rayDir, normal);
			if (cosine > 0.0)
				f.color += kd*cosine*lightColor;
			f.color += ambientColor;
		}
		f.color /= f.vIdx.size();

		if (f.color.r > 1.0f)  f.color.r = 1.0f;
		if (f.color.r < 0.0f)  f.color.r = 0.0f;
		if (f.color.g > 1.0f) f.color.g = 1.0f;
		if (f.color.g < 0.0f) f.color.g = 0.0f;
		if (f.color.b > 1.0f) f.color.b = 1.0f;
		if (f.color.b < 0.0f) f.color.b = 0.0f;
	}
}

void Render::loop()
{
	int width = 0, height = 0;
	engine->getSize(width, height);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, width, 0, height);

	costTime = engine->render(*scene);

	glBegin(GL_POINTS);

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			
			glm::vec3 rgb = engine->buffer[y*width + x];
			glColor3f(rgb.r, rgb.g, rgb.b);
			glVertex2i(x, y);
		}
	}
	glEnd();

	// 文字
	glColor3f(1.0f, 0.0f, 0.0f);
	glRasterPos2f(10.0f, height - 20.0f);
	drawString("Use keyboard :\n");
	glRasterPos2f(10.0f, height - 40.0f);
	drawString("KEY 'up' & 'down'  rotate around axis x\n");
	glRasterPos2f(10.0f, height - 60.0f);
	drawString("KEY 'left' & 'right'  rotate around axis y\n");
	glRasterPos2f(10.0f, height - 80.0f);
	drawString("KEY 'f1'  change the projection\n");

	glRasterPos2f(10.0f, height - 100.0f);
	drawString("Use mouse :\n");
	glRasterPos2f(10.0f, height - 120.0f);
	drawString("use left mouse button to rotate\n");

	char str[30];
	sprintf_s(str, "Cost time : %.3f seconds\n", costTime);
	glRasterPos2f(10.0f, height - 160.0f);
	drawString(str);
	glRasterPos2f(10.0f, height - 180.0f);
	drawString((engine->ifPerspective ? 
		"Projection : perspective\n" : "Projection : ortho\n"));

	glFinish();
}

void Render::reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	engine->setSize(w, h);
	engine->initProj(*scene);
	engine->ifNeedUpdate = true;
	engine->render(*scene);
}

void Render::display()
{
	string name = "ScanLineZBuffer";
	int weight = 0, height = 0;
	engine->getSize(weight, height);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(weight, height);
	glutInitWindowPosition(50, 80);
	glutCreateWindow(name.c_str());
	glutDisplayFunc(loop);
	glutReshapeFunc(reshape);
	glutSpecialFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(MotionFunc);

	glutMainLoop();
}

void Render::keyboard(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_LEFT:
		engine->rotate(*scene, glm::vec3(0.0f, 1.0f, 0.0f), -10);
		break;
	case GLUT_KEY_RIGHT:
		engine->rotate(*scene, glm::vec3(0.0f, 1.0f, 0.0f), 10);
		break;
	case GLUT_KEY_UP:
		engine->rotate(*scene, glm::vec3(1.0f, 0.0f, 0.0f), -10);
		break;
	case GLUT_KEY_DOWN:
		engine->rotate(*scene, glm::vec3(1.0f, 0.0f, 0.0f), 10);
		break;
	case GLUT_KEY_F1:
		engine->ifPerspective = !engine->ifPerspective;
		engine->initProj(*scene);
		break;
	default:
		break;
	}
	engine->ifNeedUpdate = true;
	glutPostRedisplay();
}

void Render::mouse(int button, int state, int x, int y)
{
	int weight = 0, height = 0;
	engine->getSize(weight, height);
	size_t id;
	if (button == GLUT_RIGHT_BUTTON) {
		switch (state) {
		case GLUT_DOWN:
#ifdef MINISCANLINE_DEBUG
			cout << "x:" << x << " y:" << (height - y);
			id = engine->idBuffer[(height - y)*weight + x];
			cout << " ID:" << id;
			for (const auto &vi : scene->fList[id].vIdx) {
				cout << " " << vi;
			}
			cout << endl;
#endif // MINISCANLINE_DEBUG
			break;
		case GLUT_UP:
			break;
		}
	}
	else if (button == GLUT_LEFT_BUTTON) {
		switch (state) {
		case GLUT_DOWN:
			isLeftDown = true;
			trackBallPos(x, y, lastBallPos);
			break;
		case GLUT_UP:
			isLeftDown = false;
			lastBallPos = glm::vec3();
			currBallPos = glm::vec3();
			break;
		}
	}
}

void Render::MotionFunc(int x, int y)
{
	if (isLeftDown)
	{
		trackBallPos(x, y, currBallPos);
		engine->trackBall(*scene, lastBallPos, currBallPos);
		lastBallPos = currBallPos;
		engine->ifNeedUpdate = true;
		glutPostRedisplay();
	}
}

void Render::trackBallPos(int x, int y, glm::vec3 & p)
{
	int weight = 0, height = 0;
	engine->getSize(weight, height);
	float nccX = (2.0f*x - weight) / weight;
	float nccY = (height - 2.0f*y) / height;
	p[0] = nccX;
	p[1] = nccY;
	p[2] = 0.0f;

	float sqrZ = 1 - glm::dot(p, p);
	if (sqrZ > 0)
		p[2] = sqrt(sqrZ);
	else
		p = glm::normalize(p);
}

void Render::drawString(const char * str)
{
	static bool isFirstCall = true;
	static GLuint lists;

	if (isFirstCall) { 
		// 如果是第一次调用，执行初始化
		// 为每一个ASCII字符产生一个显示列表
		isFirstCall = false;
		// 申请MAX_CHAR个连续的显示列表编号
		lists = glGenLists(MAX_CHAR);
		// 把每个字符的绘制命令都装到对应的显示列表中
		wglUseFontBitmaps(wglGetCurrentDC(), 0, MAX_CHAR, lists);
	}
	// 调用每个字符对应的显示列表，绘制每个字符
	for (; *str != '\n'; ++str)
		glCallList(lists + *str);
}


