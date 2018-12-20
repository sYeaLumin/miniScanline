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
			objFile >> v.pOri.x >> v.pOri.y >> v.pOri.z;
			vList.push_back(v);
		}
		else if (type == "vn") {
			glm::vec3 vn;
			objFile >> vn.x >> vn.y >> vn.z;
			vnList.push_back(vn);
		}
		else if (type == "f") {
			Face f;
			string fMes;
			while (mes >> fMes)
			{
				if (fMes.find_first_of("/") == string::npos) {
					f.vIdx.push_back(atoi(fMes.c_str()));
				}
				else {
					size_t idx1 = fMes.find_first_of("/");
					size_t idx2 = fMes.find_last_of("/");
					f.vIdx.push_back(atoi(fMes.substr(0, idx1).c_str()) - 1);
					f.nIdx.push_back(atoi(fMes.substr(idx2 + 1).c_str()) - 1);
				}
			}
			fList.push_back(f);
		}
	}
	objFile.close();
	// º∆À„∑®œÚ
	if (fList[0].nIdx.size() == 0) {
		for (auto &f : fList) {
			glm::vec3 v01 = vList[f.vIdx[1]].pOri - vList[f.vIdx[0]].pOri;
			glm::vec3 v12 = vList[f.vIdx[2]].pOri - vList[f.vIdx[1]].pOri;
			f.normal = glm::normalize(glm::cross(v01, v12));
		}
	}
	return false;
}
