#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

using namespace std;
typedef size_t Index;

class Vertex 
{
public:
	glm::vec3 pOri; // ԭʼobj�д洢�Ķ�������
	glm::vec3 p; // ͶӰ��Ķ�������
	glm::vec3 color;
	glm::vec3 normal;
};

class Face 
{
public:
	vector<Index> vIdx;
	vector<Index> nIdx;
	glm::vec3 normal;
	glm::vec3 color;
};

class Scene
{
public:
	Scene();
	~Scene();
	bool Load(string path);
public:
	vector<Vertex> vList;
	vector<Face> fList;
	vector<glm::vec3> vnList;
};
