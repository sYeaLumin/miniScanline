#include "Render.h"

using namespace std;

Scanline* Render::engine = NULL;
Scene* Render::scene = NULL;
float Render::rotateAngle = 0;

Render::Render(Scene* scene, Scanline* slzBuffer)
{
	this->scene = scene;
	engine = slzBuffer;
	// 确定各点的颜色
	//shader(*scene);

}

Render::~Render()
{
}

void Render::shader(Scene& scene) {
	const float kd = 0.8;//albedo ,散射反照率
	// 光线颜色
	glm::vec3 light_color = glm::vec3(0.3, 0.3, 0.3);
	// 外界颜色
	glm::vec3 ambient_color = glm::vec3(0.3, 0.3, 0.3);
	// 光源
	glm::vec3 light_position = glm::vec3(400.0f, 400.0f, 1000.0f);
	int face_num = scene.fList.size();

	// 遍历面
	for (int i = 0; i < face_num; ++i)
	{
		Face& face = scene.fList[i];
		int face_vertex_num = face.vIdx.size();
		// 遍历面上的点
		for (int j = 0; j < face_vertex_num; ++j)
		{
			Vertex face_vertex = scene.vList[face.vIdx[j]];
			glm::vec3 ray_direction = glm::normalize(light_position - face_vertex.pOri);//光线入射反方向
			glm::vec3 normal = scene.ifFNIdx ?
				scene.vnList[face.nIdx[j]] : face.normal;
			float cosine = dot(ray_direction, normal);//求光线入射反方向与面法线/顶点法线的夹角cos
			if (cosine > 0.0)face.color += kd*cosine*light_color;//点光源散射颜色
			face.color += ambient_color;//增加环境颜色
		}
		//多边形小块的颜色取顶点的平均颜色
		face.color /= face.vIdx.size();

		//控制颜色取值范围在0.0～1.0之间
		if (face.color.r > 1.0f)face.color.r = 1.0f;
		if (face.color.r < 0.0f)face.color.r = 0.0f;
		if (face.color.g > 1.0f)face.color.g = 1.0f;
		if (face.color.g < 0.0f)face.color.g = 0.0f;
		if (face.color.b > 1.0f)face.color.b = 1.0f;
		if (face.color.b < 0.0f)face.color.b = 0.0f;
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

	engine->render(*scene);

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
	int width = 0, height = 0;
	engine->getSize(width, height);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(50, 80);
	glutCreateWindow(name.c_str());
	glutDisplayFunc(loop);
	glutReshapeFunc(reshape);
	glutSpecialFunc(keyboard);
	glutMouseFunc(mouse);

	glutMainLoop();
}

void Render::keyboard(int key, int x, int y)
{
	int width = 0, height = 0;
	engine->getSize(width, height);
	switch (key)
	{
	case GLUT_KEY_LEFT:
		rotateAngle -= 10;
		engine->rotate2(*scene, glm::vec3(0.0f, 1.0f, 0.0f), rotateAngle);
		cout << "rotateAngle: " << rotateAngle << endl;
		break;
	case GLUT_KEY_RIGHT:
		rotateAngle += 10;
		engine->rotate2(*scene, glm::vec3(0.0f, 1.0f, 0.0f), rotateAngle);
		cout << "rotateAngle: " << rotateAngle << endl;
		break;
	default:
		break;
	}
	engine->ifNeedUpdate = true;
	engine->render(*scene);
	glutPostRedisplay();
}

void Render::mouse(int button, int state, int x, int y)
{
	int width = 0, height = 0;
	engine->getSize(width, height);
	size_t id;
	if (button == GLUT_LEFT_BUTTON) {
		switch (state) {
		case GLUT_DOWN:
			cout << "x:" << x << " y:" << (height - y);
			id = engine->idBuffer[(height - y)*width + x];
			cout << " ID:" << id;
			for (const auto &vi : scene->fList[id].vIdx) {
				cout << " " << vi;
			}/**/
			cout << endl;
			break;
		case GLUT_UP:
			break;
		}
	}
}

