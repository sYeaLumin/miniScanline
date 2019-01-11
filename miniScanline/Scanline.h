#pragma once
#include <vector>
#include <list>
#include <algorithm>
#include <time.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include "Scene.h"
//#define MINISCANLINE_DEBUG
using namespace std;

namespace SL {
	const float EPS = 10e-6;
	typedef size_t Index;
	typedef size_t LineNum;

	class Edge {
	public:
		float x;
		float dx;
		LineNum	dy;
		Index			id;
	public:
		Edge(Index _id, glm::vec3 v1, glm::vec3 v2) :
			id(_id) {
			x = v1.x;
			dx = -(v1.x - v2.x) / (v1.y - v2.y);
			dy = round(v1.y) - round(v2.y); // can not use round(v1.y-v2.y)
		}
	};

	class Polygon {
	public:
		float a, b, c, d;
		LineNum	dy;
		Index			id;
		bool inFlag = false;
	public:
		Polygon(Index _id, LineNum _num, glm::vec3 _p, glm::vec3 _n) :
			dy(_num), id(_id) {
			a = _n.x;
			b = _n.y;
			c = _n.z;
			d = -(a*_p.x + b*_p.y + c*_p.z);
		}
		float z(float x, float y) {
			if (fabs(c) > EPS) {
				return -(a*x + b*y + d) / c;
			}
			else
				//return FLT_MAX; // 左手坐标系
				return -FLT_MAX; // 右手坐标系
		}
	};

	class ActiveEdge {
	public:
		float x;
		float dx;
		float z;
		float zdx;
		float zdy;
		LineNum	dy;
		Index			id;
	public:
		ActiveEdge(Index y, const Edge & e, const Polygon & ap) :
			x(e.x), dx(e.dx), dy(e.dy), id(e.id) {
			if (fabs(ap.c) < EPS) {
				z = zdx = zdy = 0;
			}
			else {
				z = -(ap.a*e.x + ap.b*y + ap.d) / ap.c;
				zdx = -ap.a / ap.c;
				zdy = ap.b / ap.c;
			}
		}
		bool operator<(const ActiveEdge& ae) {
			if (fabs(x - ae.x) < EPS) {
				if (z < ae.z)
					return true;
				else
					return false;
			}
			else if (x < ae.x)
				return true;
			else
				return false;
		}
	};

	class Scanline
	{
	public:
		Scanline() {}
		~Scanline() {}
		void setSize(int w, int h);
		void getSize(int& w, int& h);
		double render(const Scene& scene);
		void initProj(Scene& scene);
		void rotate(Scene& scene, glm::vec3 axis, float angle);
		void trackBall(Scene & scene, glm::vec3& lastP, glm::vec3& currP);
	private:
		void initTable(const Scene& scene);
		void updateAET(Index y);
		/************* Function for project ***********/
		void calVPMat(const Scene& scene);
		void project(Scene& scene);
		glm::mat4 getRotateMat(glm::vec3 axis, float angle);
		/************* Function for debug ***********/
		void printET();
		void printAET();
		void printInFlag();
		void traceET(Index id);
		void traceAET(Index id);

	public:
		//Mat frame;
		bool ifNeedUpdate = true;
		vector<glm::vec3> buffer;
		vector<Index> idBuffer; // for debug
	private:
		int windowHeight;
		int windowWeight;
		float zNear;
		float zFar;
		glm::mat4 modelMat = glm::mat4(1.0f);
		glm::mat4 viewMat;
		glm::mat4 persMat;
		glm::mat4 projMat;
		glm::vec4 viewport;
		vector<Polygon>		PT;		// polygon table
		vector<list<Edge>>	ET;		// edge table
		list<ActiveEdge>			AET;	// active edge table
		list<Index>					IPL;		// active In-Polygon List
	};

}

