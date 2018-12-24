#include "Scene.h"



Scene::Scene()
{
}


Scene::~Scene()
{
}

bool Scene::Load(string path)
{
	ifstream objFile(path);
	if (!objFile.is_open()) {
		cout << "Open " << path << " failed!" << endl;
		return false;
	}
	string oneLine;
	string type;
	while (getline(objFile,oneLine))
	{
		stringstream mes(oneLine);
		mes >> type;
		if (type == "v") {
			Vertex v;
			mes >> v.pOri.x >> v.pOri.y >> v.pOri.z;
			vList.push_back(v);
		}
		else if (type == "vn") {
			glm::vec3 vn;
			mes >> vn.x >> vn.y >> vn.z;
			vnList.push_back(vn);
			if (!ifFNIdx)
				ifFNIdx = true;
		}
		else if (type == "f") {
			Face f;
			string fMes;
			while (mes >> fMes)
			{
				cout << "fMes : " << fMes << endl;
				if (fMes.find_first_of("/") == string::npos) {
					f.vIdx.push_back(atoi(fMes.c_str()));
				}
				else {
					size_t idx1 = fMes.find_first_of("/");
					size_t idx2 = fMes.find_last_of("/");
					f.vIdx.push_back(atoi(fMes.substr(0, idx1).c_str()) - 1);
					if(ifFNIdx)
						f.nIdx.push_back(atoi(fMes.substr(idx2 + 1).c_str()) - 1);
				}
			}
			fList.push_back(f);
			cout << endl;
		}
	}
	objFile.close();
	// 计算法向
	if (!ifFNIdx) {
		for (auto &f : fList) {
			glm::vec3 v01 = vList[f.vIdx[1]].pOri - vList[f.vIdx[0]].pOri;
			glm::vec3 v12 = vList[f.vIdx[2]].pOri - vList[f.vIdx[1]].pOri;
			f.normal = glm::normalize(glm::cross(v01, v12));
		}
	}
	return true;
}

void Scene::Resize(int width, int height)
{
	glm::vec3 min_xyz(0xfffffff, 0xfffffff, 0xfffffff),
		max_xyz(-0xfffffff, -0xfffffff, -0xfffffff);//7个f
	glm::vec3 center_xyz(0.0, 0.0, 0.0);
	int vertex_num = this->vList.size();
	// 找到最小的点
	for (int i = 0; i < vertex_num; ++i)
	{
		const glm::vec3& vertex = this->vList[i].pOri;
		min_xyz.x = min(min_xyz.x, vertex.x);
		min_xyz.y = min(min_xyz.y, vertex.y);
		min_xyz.z = min(min_xyz.z, vertex.z);
		max_xyz.x = max(max_xyz.x, vertex.x);
		max_xyz.y = max(max_xyz.y, vertex.y);
		max_xyz.z = max(max_xyz.z, vertex.z);
	}

	center_xyz = (min_xyz + max_xyz) / 2.0f;

	float model_width = max_xyz.x - min_xyz.x;
	float model_height = max_xyz.y - min_xyz.y;
	float max_model_len = max(model_width, model_height);
	//将所有顶点放置到大小为(width,height)的窗口中

	float scale = min(width, height) / max_model_len;
	scale = 0.8*scale;
	//scale = 0.4*scale;

	for (int i = 0; i < vertex_num; ++i)
	{
		glm::vec3& vNew = this->vList[i].p;
		glm::vec3& vOld = this->vList[i].pOri;
		//放置到窗口中间
		vNew.x = (vOld.x - center_xyz.x)*scale + width / 2;
		vNew.y = (vOld.y - center_xyz.y)*scale + height / 2;
		vNew.z = (vOld.z - center_xyz.z)*scale;

	}
}
