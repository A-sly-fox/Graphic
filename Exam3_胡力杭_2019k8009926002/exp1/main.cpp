// #include <bits/stdc++.h>
#include <vector>
#include <stdio.h>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <math.h>
#include <cmath>
#include <cstring>
#include <algorithm>
using namespace std;

#define MAX 15000

struct POINT3 {
	double X;
	double Y;
	double Z;
	double Q[4][4];
};
struct Face {
	int V[3];
};
class PIC
{
public:
	vector<POINT3> V;
	vector<Face> F;
};
PIC model;

typedef vector<int>link;
vector<link>linkfaces;	//每个顶点关联的面片索引
vector<link>linkpoints;

vector<bool>validpoints;
vector<bool>validfaces;

struct Normal {
	double NX;
	double NY;
	double NZ;
};
vector<Normal>normals;	// 每个面片的法向

struct optimalv3 {
	double x;
	double y;
	double z;
};

struct validpair {
	int pt1;
	int pt2;
};

struct item {
	double cost;
	struct optimalv3 optimalv3;
	struct validpair validpair;
};

vector<item> costheap;
int sizeofvalidfaces = 0;

void getF_Normal(int num)
{
	double vertex1[3];
	double vertex2[3];
	double vertex3[3];
	double vector1[3];
	double vector2[3];
	double normal[3];
	double length;

	vertex1[0] = model.V[model.F[num].V[0]].X;
	vertex1[1] = model.V[model.F[num].V[0]].Y;
	vertex1[2] = model.V[model.F[num].V[0]].Z;
	vertex2[0] = model.V[model.F[num].V[1]].X;
	vertex2[1] = model.V[model.F[num].V[1]].Y;
	vertex2[2] = model.V[model.F[num].V[1]].Z;
	vertex3[0] = model.V[model.F[num].V[2]].X;
	vertex3[1] = model.V[model.F[num].V[2]].Y;
	vertex3[2] = model.V[model.F[num].V[2]].Z;

	vector1[0] = vertex2[0] - vertex1[0];
	vector1[1] = vertex2[1] - vertex1[1];
	vector1[2] = vertex2[2] - vertex1[2];
	vector2[0] = vertex3[0] - vertex1[0];
	vector2[1] = vertex3[1] - vertex1[1];
	vector2[2] = vertex3[2] - vertex1[2];

	normal[0] = vector1[1] * vector2[2] - vector1[2] * vector2[1];
	normal[1] = vector1[2] * vector2[0] - vector1[0] * vector2[2];
	normal[2] = vector1[0] * vector2[1] - vector1[1] * vector2[0];

	length = sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);

	normals[num].NX = normal[0] / length;
	normals[num].NY = normal[1] / length;
	normals[num].NZ = normal[2] / length;

}

void ComputeQ(int p)
{
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			model.V[p].Q[i][j] = 0;
	double plane[4];
	for (int i = 0; i < linkfaces[p].size(); i++)
	{
		plane[0] = normals[linkfaces[p][i]].NX;
		plane[1] = normals[linkfaces[p][i]].NY;
		plane[2] = normals[linkfaces[p][i]].NZ;
		plane[3] = -model.V[p].X * plane[0] - model.V[p].Y * plane[1] - model.V[p].Z * plane[2];
		for (int k = 0; k < 4; k++)
			for (int j = 0; j < 4; j++)
				model.V[p].Q[k][j] += plane[k] * plane[j];
	}
}

struct item GetCost(int pti, int ptj)
{
	double optimalv4[4];
	double** A = new double* [4];
	for (int i = 0; i < 4; i++) {
		A[i] = new double[4];
		for (int j = 0; j < 4; j++)
			A[i][j] = model.V[pti].Q[i][j] + model.V[ptj].Q[i][j];
	}
	A[3][0] = A[3][1] = A[3][2] = 0; A[3][3] = 1;
	double* b = new double[4];
	b[0] = b[1] = b[2] = 0; b[3] = 1;
	double* x = new double[4];
	x[0] = x[1] = x[2] = x[3] = 0;
	bool flag = true;
	for (int i = 0; i < 4; i++) {
		int maxLine = i;
		for (int j = i + 1; j < 4; j++)
			if (fabs(A[j][i]) > fabs(A[maxLine][i]))
				maxLine = j;
		for (int j = i; j < 4; j++)
			swap(A[i][j], A[maxLine][j]);
		swap(b[i], b[maxLine]);
		double t = A[i][i];
		if (fabs(t) < 1e-10) {
			x[0] = (model.V[pti].X + model.V[ptj].X) / 2;
			x[1] = (model.V[pti].Y + model.V[ptj].Y) / 2;
			x[2] = (model.V[pti].Z + model.V[ptj].Z) / 2;
			x[3] = 1.0;
			flag = false;
			break;
		}
		for (int j = i; j < 4; j++)
			A[i][j] /= t;
		b[i] /= t;
		for (int j = i + 1; j < 4; j++)
			if (fabs(A[j][i]) > 1e-8) {
				t = A[j][i];
				for (int k = i; k < 4; k++)
					A[j][k] -= A[i][k] * t;
				b[j] -= b[i] * t;
			}
	}
	if (flag) {
		for (int i = 3; i >= 0; i--) {
			x[i] = b[i];
			for (int k = i + 1; k < 4; k++)
				x[i] -= A[i][k] * x[k];
		}
	}
	optimalv4[0] = x[0];
	optimalv4[1] = x[1];
	optimalv4[2] = x[2];
	optimalv4[3] = 1.0;
	double cost = 0;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++)
			A[i][j] = model.V[pti].Q[i][j] + model.V[ptj].Q[i][j];
	}
	double temp[4];
	for (int i = 0; i < 4; i++) {
		temp[i] = optimalv4[0] * A[0][i] + optimalv4[1] * A[1][i] + optimalv4[2] * A[2][i] + optimalv4[3] * A[3][i];
	}
	cost = optimalv4[0] * temp[0] + optimalv4[1] * temp[1] + optimalv4[2] * temp[2] + optimalv4[3] * temp[3];

	struct item best;
	best.cost = fabs(cost);
	best.optimalv3.x = optimalv4[0];
	best.optimalv3.y = optimalv4[1];
	best.optimalv3.z = optimalv4[2];
	best.validpair.pt1 = pti;
	best.validpair.pt2 = ptj;
	return best;
}

void HeapPush(struct item temp)
{
	costheap.push_back(temp);
}

struct item HeapPop()
{
	double min_cost;
	int num;
	struct item min_item;
	min_item = costheap[0];
	min_cost = costheap[0].cost;
	num = 0;
	for (int i = 1; i < costheap.size(); i++)
		if (costheap[i].cost < min_cost) {
			min_item = costheap[i];
			min_cost = costheap[i].cost;
			num = i;
		}
	costheap.erase(costheap.begin() + num);
	return min_item;
}

void MakeHeap()
{
	cout << "MakeHeap......" << endl;
	for (int i = 0; i < model.V.size(); i++)
		for (int j = 0; j < linkpoints[i].size(); j++)
			if (i < linkpoints[i][j]) {
				struct item temp;
				temp = GetCost(i, linkpoints[i][j]);
				HeapPush(temp);
			}
}

void InitializeAll(char* input_name)
{
	ifstream ifs(input_name);
	string s;
	Face* f;
	POINT3* v;
	Normal* vn;
	link face;
	link points;
	bool boolean = true;
	int a, b;
	while (getline(ifs, s)) {
		if (s.length() <= 2)
			continue;
		if (s[0] == 'v') {
			istringstream in(s);
			v = new POINT3();
			string head;
			in >> head >> v->X >> v->Y >> v->Z;
			model.V.push_back(*v);
			linkfaces.push_back(face);
			linkpoints.push_back(points);
			validpoints.push_back(true);
		}
		else if (s[0] == 'f') {
			istringstream in(s);
			vn = new Normal();
			vn->NX = 0;
			vn->NY = 0;
			vn->NZ = 0;
			normals.push_back(*vn);
			f = new Face();
			string head;
			in >> head >> f->V[0] >> f->V[1] >> f->V[2];
			f->V[0] -= 1;
			f->V[1] -= 1;
			f->V[2] -= 1;
			model.F.push_back(*f);
			validfaces.push_back(true);
			for (int i = 0; i < 3; i++) {
				linkfaces[f->V[i]].push_back(sizeofvalidfaces);
				if (find(linkpoints[f->V[i]].begin(), linkpoints[f->V[i]].end(), f->V[(i + 1) % 3]) == linkpoints[f->V[i]].end())
					linkpoints[f->V[i]].push_back(f->V[(i + 1) % 3]);
				if (find(linkpoints[f->V[i]].begin(), linkpoints[f->V[i]].end(), f->V[(i + 2) % 3]) == linkpoints[f->V[i]].end())
					linkpoints[f->V[i]].push_back(f->V[(i + 2) % 3]);
			}
			sizeofvalidfaces++;
		}
	}
	for (int i = 0; i < model.F.size(); i++)
		getF_Normal(i);
	for (int i = 0; i < model.V.size(); i++)
		ComputeQ(i);
	MakeHeap();
}

void output(char* output_name)
{
	freopen(output_name, "w", stdout);
	int count = 0;
	int array[MAX];
	for (int i = 0; i < model.V.size(); i++) {
		array[i] = count;
		if (validpoints[i] == true) {
			printf("v %.6lf %.6lf %.6lf\n", model.V[i].X, model.V[i].Y, model.V[i].Z);
			count++;
		}
	}
	for (int i = 0; i < model.F.size(); i++) {
		if (validfaces[i] == true)
			printf("f %d %d %d\n", array[model.F[i].V[0]] + 1, array[model.F[i].V[1]] + 1, array[model.F[i].V[2]] + 1);
	}
}

void DeleteVertex()//删除一个pair
{
	// 获得删除的pair
	struct item vbest = HeapPop();

	//获得并且添加新顶点
	POINT3* v = new POINT3();
	v->X = vbest.optimalv3.x;
	v->Y = vbest.optimalv3.y;
	v->Z = vbest.optimalv3.z;

	//将删除的顶点设为无效
	int todelete1 = vbest.validpair.pt1;
	int todelete2 = vbest.validpair.pt2;
	validpoints[todelete1] = false;
	validpoints[todelete2] = false;

	model.V.push_back(*v);
	validpoints.push_back(true);

	//将删除的面设为无效
	for (int i = 0; i < linkfaces[todelete1].size(); i++)
		for (int j = 0; j < linkfaces[todelete2].size(); j++)
			if (linkfaces[todelete1][i] == linkfaces[todelete2][j]) {
				validfaces[linkfaces[todelete1][i]] = false;
				int c = model.F[linkfaces[todelete1][i]].V[0] ^ model.F[linkfaces[todelete1][i]].V[1] ^ model.F[linkfaces[todelete1][i]].V[2] ^ todelete1 ^ todelete2;
				for (int k = 0; k < linkfaces[c].size(); k++)
					if (linkfaces[c][k] == linkfaces[todelete1][i]) {
						linkfaces[c].erase(linkfaces[c].begin() + k);
						k--;
					}
				sizeofvalidfaces--;
			}

	// 设置新的顶点相连的面
	// 设置修改的面的新顶点 
	// 设置修改的面的法向量
	link nulllinkface;
	linkfaces.push_back(nulllinkface);
	for (int i = 0; i < linkfaces[todelete1].size(); i++) {			//处理todelete1的链接关系
		if (validfaces[linkfaces[todelete1][i]]) {
			linkfaces.rbegin()->push_back(linkfaces[todelete1][i]);
			if (model.F[linkfaces[todelete1][i]].V[0] == todelete1) {
				model.F[linkfaces[todelete1][i]].V[0] = linkfaces.size() - 1;
			}
			else if (model.F[linkfaces[todelete1][i]].V[1] == todelete1) {
				model.F[linkfaces[todelete1][i]].V[1] = linkfaces.size() - 1;
			}
			else if (model.F[linkfaces[todelete1][i]].V[2] == todelete1) {
				model.F[linkfaces[todelete1][i]].V[2] = linkfaces.size() - 1;
			}
			getF_Normal(linkfaces[todelete1][i]);
			ComputeQ(model.F[linkfaces[todelete1][i]].V[0]);
			ComputeQ(model.F[linkfaces[todelete1][i]].V[1]);
			ComputeQ(model.F[linkfaces[todelete1][i]].V[2]);
		}
	}
	for (int i = 0; i < linkfaces[todelete2].size(); i++) {			//处理todelete2的链接关系
		if (validfaces[linkfaces[todelete2][i]]) {
			linkfaces.rbegin()->push_back(linkfaces[todelete2][i]);
			if (model.F[linkfaces[todelete2][i]].V[0] == todelete2) {
				model.F[linkfaces[todelete2][i]].V[0] = linkfaces.size() - 1;
			}
			else if (model.F[linkfaces[todelete2][i]].V[1] == todelete2) {
				model.F[linkfaces[todelete2][i]].V[1] = linkfaces.size() - 1;
			}
			else if (model.F[linkfaces[todelete2][i]].V[2] == todelete2) {
				model.F[linkfaces[todelete2][i]].V[2] = linkfaces.size() - 1;
			}
			getF_Normal(linkfaces[todelete2][i]);
			ComputeQ(model.F[linkfaces[todelete2][i]].V[0]);
			ComputeQ(model.F[linkfaces[todelete2][i]].V[1]);
			ComputeQ(model.F[linkfaces[todelete2][i]].V[2]);
		}
	}

	//设置新的顶点连接的pair
	//修改堆中的cost并且更新
	vector<int> ano;
	for (int i = 0; i < costheap.size(); i++) {//与delete1相连的边与新增点相连
		int anopoint = 0;
		if (costheap[i].validpair.pt1 == todelete1) {
			anopoint = costheap[i].validpair.pt2;
			if (validpoints[anopoint]) {
				ano.push_back(anopoint);
				costheap.erase(costheap.begin() + i);
				i--;
			}
		}
		else if (costheap[i].validpair.pt2 == todelete1) {
			anopoint = costheap[i].validpair.pt1;
			if (validpoints[anopoint]) {
				ano.push_back(anopoint);
				costheap.erase(costheap.begin() + i);
				i--;
			}
		}
	}
	for (int i = 0; i < costheap.size(); i++) {//与delete2相连的边与新增点相连
		int anopoint = 0;
		if (costheap[i].validpair.pt1 == todelete2) {
			anopoint = costheap[i].validpair.pt2;
			if (validpoints[anopoint]) {
				ano.push_back(anopoint);
				costheap.erase(costheap.begin() + i);
				i--;
			}
		}
		else if (costheap[i].validpair.pt2 == todelete2) {
			anopoint = costheap[i].validpair.pt1;
			if (validpoints[anopoint]) {
				ano.push_back(anopoint);
				costheap.erase(costheap.begin() + i);
				i--;
			}
		}
	}
	sort(ano.begin(), ano.end()); // 排序，以消除重边
	int p = 1;
	for (int i = 1; i < ano.size(); i++)
		if (ano[i] != ano[i - 1]) {
			ano[p] = ano[i];
			p++;
		}
	ano.erase(ano.begin() + p, ano.end());

	//构建新顶点的邻接关系
	for (int i = 0; i < ano.size(); i++) { //添加与新增节点相连的边 
		struct item temp;
		temp = GetCost(ano[i], model.V.size() - 1);
		HeapPush(temp);
	}
}

int main(int argc, char* argv[])
{
	clock_t a, b;
	float cputime;
	char input_name[256];
	char output_name[256];
	float simplification_ratio;
	if (argc == 4) {
		strcpy(input_name, argv[1]);
		strcpy(output_name, argv[2]);
		sscanf(argv[3], "%f", &simplification_ratio);
		//读取模型，计算误差，建立堆
		InitializeAll(input_name);
		int opt_faces = sizeofvalidfaces * simplification_ratio;
		//简化网络
		a = time(NULL);
		cout << "Delete......" << endl;
		while (sizeofvalidfaces > opt_faces) {
			DeleteVertex();
		}
		b = time(NULL);
		cputime = (float)(b - a);
		cout << "cputime=" << cputime << endl;
		output(output_name);
	}else {
		cout << "Unknown command!" << endl;
	}
	return 0;
}
