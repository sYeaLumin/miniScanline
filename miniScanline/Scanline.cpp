#include "Scanline.h"

void SL::Scanline::setSize(int w, int h)
{
	windowHeight = h;
	windowWeight = w;
}

void SL::Scanline::getSize(int & w, int & h)
{
	h = windowHeight;
	w = windowWeight;
}

void SL::Scanline::render(const Scene & scene)
{
	if (!ifNeedUpdate)return;
	//Mat currFrame = Mat::zeros(windowHeight, windowWeight, CV_8UC3);
	vector<glm::vec3> currBuffer;
	currBuffer.resize(windowHeight*windowWeight);
	//TODO: ����ɫ
	initTable(scene);
	for (int y = windowHeight - 1; y >= 0; y--) {
		//cout << "y:" << y;
		// ���IPL
		list<Index>().swap(IPL);
		// ���»�߱�
		updateAET(y);

		if (AET.size() == 0)
			continue;
		assert(AET.size() % 2 == 0); // ...

		// ��߱�����
		//sort(AET.begin(), AET.end(), ActiveEdge::sortCompare);
		AET.sort();
		
		//Mat currFrameRow = currFrame.row(y);
		list<ActiveEdge>::iterator ae;
		list<ActiveEdge>::iterator ae2;
		for (ae = AET.begin(); ae != AET.end(); ++ae) {
			ae2 = ae;
			++ae2;
			// ���һ������
			if (ae2 == AET.end()) {
				PT[ae->id].inFlag = false;
				IPL.remove(ae->id);
				break;
			}
			// �������
			if (!PT[ae->id].inFlag) {
				PT[ae->id].inFlag = true;
				IPL.push_back(ae->id);
			}
			// �������
			else {
				PT[ae->id].inFlag = false;
				IPL.remove(ae->id);
			}
			// �ص�
			if (round(ae->x) == round(ae2->x))
				continue;

			int polygonID = -1;
			float midX = (ae->x + ae2->x) / 2;
			// ��������ϵ����������ϵ��NDC����ϵ����������ϵ
			//float minZ = FLT_MAX; 
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
					/*
					if (minZ > curZ) {
						minZ = curZ;
						polygonID = AIP.id;
					}*/
				}
			}
			// ��¼��ɫ
			if (polygonID >= 0) {
				glm::vec3 color = scene.fList[polygonID].color;
				//cv::Scalar rgb(color.b, color.g, color.r);
				//currFrameRow(Range::all(), Range(round(ae->x), round(ae2->x))) = rgb;
				for (size_t x = round(ae->x), end = round(ae2->x); x < end; ++x)
					currBuffer[y*windowWeight + x] = color;
			}
		}
	}
	//swap(currFrame, frame);
	swap(currBuffer, buffer);
	ifNeedUpdate = false;
}

void SL::Scanline::initTable(const Scene & scene)
{
	Index ID = 0;
	ET.resize(windowHeight);
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
			if ((v1.y - v2.y) <= 0.5)
				continue;

			ET[round(v1.y)].push_back(Edge(ID, v1, v2));

			minY = min(minY, v2.y);
			maxY = max(maxY, v1.y);
		}
		if (scene.ifFNIdx) {
			glm::vec3 normal = scene.vnList[f.nIdx[0]];
			PT.push_back(
				Polygon(ID, round(maxY) - round(minY), scene.vList[f.vIdx[0]].p, normal)
			);
		}
		else
			PT.push_back(
				Polygon(ID, round(maxY) - round(minY), scene.vList[f.vIdx[0]].p, f.normal)
			);
		ID++;
	}
}

void SL::Scanline::updateAET(Index y)
{
	// ɾ���Ѿ�ɨ����ı�
	if (AET.size() > 0) {
		list<ActiveEdge>::iterator aeIter;
		for (aeIter = AET.begin(); aeIter != AET.end();) {
			if (aeIter->dy <= 1) {
				aeIter = AET.erase(aeIter);
			}
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
	// ����µı�
	for (const auto &e : ET[y])
		AET.push_back(ActiveEdge(y, e, PT[e.id]));
}
