#include "Render.h"

using namespace std;

Scanline* Render::engine = NULL;
Scene* Render::scene = NULL;
glm::mat4 Render::RotateMat = glm::mat4();

int Render::idx1 = -1, Render::idx2 = -1;


Render::Render(Scene* scene, Scanline* slzBuffer)
{
	this->scene = scene;
	engine = slzBuffer;
	// ȷ���������ɫ
	//shader(*scene);

	RotateMat = glm::mat4();
}

Render::~Render()
{
}

void Render::shader(Scene& scene) {
	const float kd = 0.8;//albedo ,ɢ�䷴����
	// ������ɫ
	glm::vec3 light_color = glm::vec3(0.3, 0.3, 0.3);
	// �����ɫ
	glm::vec3 ambient_color = glm::vec3(0.3, 0.3, 0.3);
	// ��Դ
	glm::vec3 light_position = glm::vec3(400.0f, 600.0f, 500.0f);
	int face_num = scene.fList.size();

	// ������
	for (int i = 0; i < face_num; ++i)
	{
		Face& face = scene.fList[i];
		int face_vertex_num = face.vIdx.size();
		// �������ϵĵ�
		for (int j = 0; j < face_vertex_num; ++j)
		{
			Vertex face_vertex = scene.vList[face.vIdx[j]];
			glm::vec3 ray_direction = glm::normalize(light_position - face_vertex.pOri);//�������䷴����
			glm::vec3 normal = scene.ifFNIdx ?
				scene.vnList[face.nIdx[j]] : face.normal;
			float cosine = dot(ray_direction, normal);//��������䷴�������淨��/���㷨�ߵļн�cos
			if (cosine > 0.0)face.color += kd*cosine*light_color;//���Դɢ����ɫ
			face.color += ambient_color;//���ӻ�����ɫ
		}
		//�����С�����ɫȡ�����ƽ����ɫ
		face.color /= face.vIdx.size();

		//������ɫȡֵ��Χ��0.0��1.0֮��
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
	(*scene).Resize(w, h);
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

	glutMainLoop();
}


void Render::keyboard(int key, int x, int y)
{
}
