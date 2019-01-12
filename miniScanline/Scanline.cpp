#include "Scanline.h"

void SL::Scanline::setSize(int w, int h)
{
	windowHeight = h;
	windowWeight = w;
	viewport = glm::vec4(0.0f, 0.0f, w, h);
}

void SL::Scanline::getSize(int & w, int & h)
{
	h = windowHeight;
	w = windowWeight;
}

double SL::Scanline::render(const Scene & scene)
{
	clock_t t1 = clock();
	if (!ifNeedUpdate)return 0.0;

#ifdef MINISCANLINE_DEBUG
	vector<Index> currIDBuffer;
	currIDBuffer.resize(windowHeight*windowWeight);
#endif // MINISCANLINE_DEBUG
	
	vector<glm::vec3> currBuffer;
	currBuffer.resize(windowHeight*windowWeight);
	initTable(scene);

	for (int y = windowHeight - 1; y >= 0; y--) {

		// 清空IPL
		list<Index>().swap(IPL);
		// 更新活化边表
		updateAET(y);

		if (AET.size() == 0)
			continue;

#ifdef MINISCANLINE_DEBUG
		assert(AET.size() % 2 == 0);
#endif // MINISCANLINE_DEBUG

		// 活化边表排序
		AET.sort();
		
		list<ActiveEdge>::iterator ae;
		list<ActiveEdge>::iterator ae2;
		for (ae = AET.begin(); ae != AET.end(); ++ae) {
			ae2 = ae;
			++ae2;
			// 最后一个区间
			if (ae2 == AET.end()) {
				PT[ae->id].inFlag = false;
				IPL.remove(ae->id);
				break;
			}
			// 进多边形
			if (!PT[ae->id].inFlag) {
				PT[ae->id].inFlag = true;
				IPL.push_back(ae->id);
			}
			// 出多边形
			else {
				PT[ae->id].inFlag = false;
				IPL.remove(ae->id);
			}
			// 重叠
			if (round(ae->x) == round(ae2->x))
				continue;

			int polygonID = -1;
			float midX = (ae->x + ae2->x) / 2;
			// 世界坐标系是右手坐标系，NDC坐标系是左手坐标系
			float maxZ = -FLT_MAX;
			float curZ;
			if (IPL.size() == 0)
				continue;
			if (IPL.size() == 1)
				polygonID = IPL.front();
			if (IPL.size() >= 2) {
				for (const auto &id : IPL) {
					Polygon & AIP = PT[id];
					curZ = AIP.z(midX, y);
					if (maxZ < curZ) {
						maxZ = curZ;
						polygonID = AIP.id;
					}
				}
			}
			// 记录颜色
			if (polygonID >= 0) {
				glm::vec3 color = scene.fList[polygonID].color;
				for (size_t x = round(ae->x), end = round(ae2->x); x < end; ++x) {
					currBuffer[y*windowWeight + x] = color;
#ifdef MINISCANLINE_DEBUG
					currIDBuffer[y*windowWeight + x] = polygonID;
#endif // MINISCANLINE_DEBUG
				}
			}
		}
	}

	swap(currBuffer, buffer);
#ifdef MINISCANLINE_DEBUG
	swap(currIDBuffer, idBuffer);
#endif // MINISCANLINE_DEBUG

	AET.clear();

	ifNeedUpdate = false;

	clock_t t2 = clock();

	return (double)(t2 - t1) / CLOCKS_PER_SEC;
}

void SL::Scanline::initProj(Scene & scene)
{
	calVPMat(scene);
	project(scene);
}

void SL::Scanline::rotate(Scene & scene, glm::vec3 axis, float angle)
{
	modelMat = getRotateMat(axis, angle) * modelMat;
	project(scene);
}

void SL::Scanline::trackBall(Scene & scene, glm::vec3 & lastP, glm::vec3 & currP)
{
	glm::vec3 axis = glm::cross(lastP, currP);
	float angle = 90 * glm::l2Norm(axis);
	if (angle > 0) {
		modelMat = getRotateMat(axis, angle)*modelMat;
		project(scene);
	}
}

void SL::Scanline::initTable(const Scene & scene)
{
	vector<Polygon>		currPT;
	vector<list<Edge>>	currET;
	Index ID = 0;
	currET.resize(windowHeight);
	for (const auto &f : scene.fList) {
		float minY = FLT_MAX;
		float maxY = FLT_MIN;
		for (size_t i = 0; i < f.vIdx.size(); i++) {
			glm::vec3 v1 = scene.vList[f.vIdx[i]].p;
			glm::vec3 v2 = scene.vList[f.vIdx[(i + 1) % f.vIdx.size()]].p;
			if (v1.y < v2.y)
				swap(v1, v2);

			if (v1.y < 0 || v2.y>windowHeight)
				continue;
			if (round(v1.y) == round(v2.y))
				continue;

			currET[round(v1.y)].push_back(Edge(ID, v1, v2));

			minY = min(minY, v2.y);
			maxY = max(maxY, v1.y);
		}
		glm::vec3 normal;
		if (scene.ifFNIdx) 
			normal = scene.vnList[f.nIdx[0]];
		else
			normal = f.normal;
		LineNum dy = round(maxY) - round(minY);
#ifdef MINISCANLINE_DEBUG
		if (dy > 0) 
			assert( fabs(normal.z) > EPS);
#endif // MINISCANLINE_DEBUG
		currPT.push_back(
			Polygon(ID, dy, scene.vList[f.vIdx[0]].p, normal)
		);
		ID++;
	}
	swap(currET, ET);
	swap(currPT, PT);
}

void SL::Scanline::updateAET(Index y)
{
	// 更新已在活化边表中的活化边
	if (AET.size() > 0) {
		list<ActiveEdge>::iterator aeIter;
		for (aeIter = AET.begin(); aeIter != AET.end();) {
			// 如果活化边跨越的剩余扫描线数目小于等于1，删除
			if (aeIter->dy <= 1) {
				aeIter = AET.erase(aeIter);
			}
			// 否则更新活化边的x,z,dy值
			else {
				aeIter->x += aeIter->dx;
				aeIter->z += aeIter->zdx*aeIter->dx + aeIter->zdy;
				aeIter->dy--;
				aeIter++;
			}
		}
	}
	if (ET[y].size() == 0)
		return;
	// 添加新的边
	for (const auto &e : ET[y])
		// if (fabs(PT[e.id].c) > EPS)  // no back culling
		if (PT[e.id].c > EPS) // back culling
			AET.push_back(ActiveEdge(y, e, PT[e.id]));
}

void SL::Scanline::calVPMat(const Scene & scene)
{
	float fov = glm::radians(45.0f);
	float radius = glm::length(scene.maxCoord - scene.minCoord);
	zNear = 0.2 * radius / glm::sin(0.5*fov);
	zFar = 10 * (zNear + 2.0*radius);
	float dis = zNear + radius * 0.8;

	// 视变换矩阵
	glm::vec3 center = (scene.maxCoord + scene.minCoord) / 2.0f;
	glm::vec3 eye = center + glm::vec3(0.0f, 0.0f, 1.0f)*dis;
	viewMat = glm::lookAt(eye, center, glm::vec3(0.0f, 1.0f, 0.0f));

	// 透视投影矩阵
	perspMat = glm::perspective(fov,
		(float)windowWeight / (float)windowHeight, zNear, zFar);
	projMatP = perspMat*viewMat;

	// 正交投影矩阵
	float h = radius * 0.5;
	float w = h * (float)windowWeight / (float)windowHeight;
	orthoMat = glm::ortho(-w, w, -h, h, zNear, zFar);
	projMatO = orthoMat*viewMat;
}

void SL::Scanline::project(Scene & scene)
{
	glm::mat4 currProjMat;
	if (ifPerspective)
		currProjMat = projMatP;
	else
		currProjMat = projMatO;

	// 顶点投影
	for (auto &v : scene.vList) {
		v.p = glm::project(v.pOri, modelMat, currProjMat, viewport);
		v.p.z = -v.p.z*(zFar - zNear) - zNear; // 左右手坐标系变换
	}
	// 根据顶点新坐标重新计算法线
	for (auto &f : scene.fList) {
		glm::vec3 v01 = scene.vList[f.vIdx[1]].p - scene.vList[f.vIdx[0]].p;
		glm::vec3 v12 = scene.vList[f.vIdx[2]].p - scene.vList[f.vIdx[1]].p;
		f.normal = glm::normalize(glm::cross(v01, v12));
	}
	scene.ifFNIdx = false;
}

glm::mat4 SL::Scanline::getRotateMat(glm::vec3 axis, float angle)
{
	glm::vec3 rotateAxis = glm::normalize(axis);
	float radians = glm::radians(angle);
	glm::quat rotateQuat = glm::angleAxis(radians, rotateAxis);
	return glm::toMat4(rotateQuat);
}

void SL::Scanline::printET()
{
	for (int y = windowHeight - 1; y >= 0; y--) {
		if (ET[y].size() == 0)
			continue;
		else {
			if (y < 296 || y > 305)
				continue;
			cout << "ET" << y << "\t:" << ET[y].size() << "\t:";
			for (const auto &e : ET[y]) {
				cout << e.id << "y" << e.dy << " ";
			}
			cout << endl;
		}
	}
}

void SL::Scanline::printAET()
{
	for (const auto &ae : AET) {
		cout << ae.id << (PT[ae.id].inFlag ? "+" : "-") << ae.dy << " ";
	}
	cout << endl;
}

void SL::Scanline::printInFlag()
{
	for (const auto &ae : AET) {
		cout <<(PT[ae.id].inFlag ? "+" : "-")  << " ";
	}
	cout << endl;
}

void SL::Scanline::traceET(Index id)
{
	for (int y = windowHeight - 1; y >= 0; y--) {
		if (ET[y].size() == 0)
			continue;
		else {
			cout << "ET" << y << "\t:" << ET[y].size() << "\t:";
			for (const auto &e : ET[y]) {
				if(e.id==id)
					cout << e.id << "y" << e.dy << " ";
			}
			cout << endl;
		}
	}
}

void SL::Scanline::traceAET(Index id)
{
	size_t num = 0;
	for (const auto &ae : AET) {
		if (ae.id == id) {
			cout << "No" << num << ".";
			cout << ae.id << (PT[ae.id].inFlag ? "+" : "-") << ae.dy << " ";
		}
		num++;
	}
	cout << endl;
}
