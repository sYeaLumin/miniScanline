#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

using namespace std;
typedef size_t Index;

class Vertex 
{
public:
	glm::vec3 pOri; // 原始obj中存储的顶点数据
	glm::vec3 p; // 投影后的顶点数据
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
	void Resize(int width, int height);
public:
	vector<Vertex> vList;
	vector<Face> fList;
	vector<glm::vec3> vnList;
	bool ifFNIdx = false;
};

