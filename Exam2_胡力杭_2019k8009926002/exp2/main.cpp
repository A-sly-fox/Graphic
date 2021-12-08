#include <vector>
#include <GL/glut.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <math.h>
#include <cmath>
using namespace std;


struct POINT3 {
	double X;
	double Y;
	double Z;
};
struct Texture {
	double TU;
	double TV;
};
struct Normal {
	double NX;
	double NY;
	double NZ;
};
struct Face {
	int V[3];
	//int T[3];
	int N[3];
};
class PIC
{
public:
	vector<POINT3> V;//V：代表顶点。格式为V X Y Z，V后面的X Y Z表示三个顶点坐标。浮点型
	//vector<Texture>  VT;//表示纹理坐标。格式为VT TU TV。浮点型
	vector<Normal> VN;//VN：法向量。每个三角形的三个顶点都要指定一个法向量。格式为VN NX NY NZ。浮点型
	vector<Face> F;//F：面。面后面跟着的整型值分别是属于这个面的顶点、纹理坐标、法向量的索引。
				   //面的格式为：f Vertex1/Texture1/Normal1 Vertex2/Texture2/Normal2 Vertex3/Texture3/Normal3
};

PIC m_pic;
float times = 1.0;
float x_rotate = 0.0;
float y_rotate = 0.0;
float z_rotate = 0.0;
int vn_count = 0;

void ReadPIC()
{
	ifstream ifs("bunny.obj");
	string s;
	Face* f;
	POINT3* v;
	Normal* vn;
	Texture* vt;
	while (getline(ifs, s))
	{
		if (s.length() < 2)
			continue;
		if (s[0] == 'v') {
			if (s[1] == 't') {
				/*istringstream in(s);
				vt = new Texture();
				string head;
				in >> head >> vt->TU >> vt->TV;
				m_pic.VT.push_back(*vt);*/
			}
			if (s[1] == 'n') {
				//istringstream in(s);
				vn = new Normal();
				//string head;
				//in >> head >> vn->NX >> vn->NY >> vn->NZ;
				vn->NX = 0;
				vn->NY = 0;
				vn->NZ = 0;
				m_pic.VN.push_back(*vn);
				vn_count++;
			}
			else {
				istringstream in(s);
				v = new POINT3();
				string head;
				in >> head >> v->X >> v->Y >> v->Z;
				m_pic.V.push_back(*v);
			}
		}
		else if (s[0] == 'f') {
			for (int k = s.size() - 1; k >= 0; k--) {
				if (s[k] == '/')
					s[k] = ' ';
			}
			istringstream in(s);
			f = new Face();
			string head;
			in >> head;
			int i = 0;
			while (i < 3)
			{
				if (m_pic.V.size() != 0)
				{
					in >> f->V[i];
					f->V[i] -= 1;
				}
				/*if (m_pic.VT.size() != 0)
				{
					in >> f->T[i];
					f->T[i] -= 1;
				}*/
				if (m_pic.VN.size() != 0)
				{
					in >> f->N[i];
					f->N[i] -= 1;
				}
				i++;
			}
			m_pic.F.push_back(*f);
		}
	}
}

void getF_Normal()
{
	float vertex1[3];
	float vertex2[3];
	float vertex3[3];
	float vector1[3];
	float vector2[3];
	float normal[3];
	float length;
	double work[3];
	for (int i = 0; i < m_pic.F.size(); i++)
	{
		vertex1[0] = m_pic.V[m_pic.F[i].V[0]].X;
		vertex1[1] = m_pic.V[m_pic.F[i].V[0]].Y;
		vertex1[2] = m_pic.V[m_pic.F[i].V[0]].Z;
		vertex2[0] = m_pic.V[m_pic.F[i].V[1]].X;
		vertex2[1] = m_pic.V[m_pic.F[i].V[1]].Y;
		vertex2[2] = m_pic.V[m_pic.F[i].V[1]].Z;
		vertex3[0] = m_pic.V[m_pic.F[i].V[2]].X;
		vertex3[1] = m_pic.V[m_pic.F[i].V[2]].Y;
		vertex3[2] = m_pic.V[m_pic.F[i].V[2]].Z;

		vector1[0] = vertex2[0] - vertex1[0];
		vector1[1] = vertex2[1] - vertex1[1];
		vector1[2] = vertex2[2] - vertex1[2];
		vector2[0] = vertex3[0] - vertex1[0];
		vector2[1] = vertex3[1] - vertex1[1];
		vector2[2] = vertex3[2] - vertex1[2];

		normal[0] = vector1[1] * vector2[2] - vector1[2] * vector2[1];
		normal[1] = vector1[2] * vector2[0] - vector1[0] * vector2[2];
		normal[2] = vector1[0] * vector2[1] - vector1[1] * vector2[0];

		m_pic.VN[m_pic.F[i].N[0]].NX += normal[0];
		m_pic.VN[m_pic.F[i].N[0]].NY += normal[1];
		m_pic.VN[m_pic.F[i].N[0]].NZ += normal[2];
		m_pic.VN[m_pic.F[i].N[1]].NX += normal[0];
		m_pic.VN[m_pic.F[i].N[1]].NY += normal[1];
		m_pic.VN[m_pic.F[i].N[1]].NZ += normal[2];
		m_pic.VN[m_pic.F[i].N[2]].NX += normal[0];
		m_pic.VN[m_pic.F[i].N[2]].NY += normal[1];
		m_pic.VN[m_pic.F[i].N[2]].NZ += normal[2];
	}
	for (int i = 0; i < vn_count; i++)
	{
		length = sqrt(pow(m_pic.VN[i].NX, 2) + pow(m_pic.VN[i].NY, 2) + pow(m_pic.VN[i].NZ, 2));
		m_pic.VN[i].NX /= length;
		m_pic.VN[i].NY /= length;
		m_pic.VN[i].NZ /= length;
	}
}

void GLCube()
{
	for (int i = 0; i < m_pic.F.size(); i++)
	{
		glBegin(GL_TRIANGLES);

		//glTexCoord2f(m_pic.VT[m_pic.F[i].T[0]].TU, m_pic.VT[m_pic.F[i].T[0]].TV);  //纹理	
		glNormal3f(m_pic.VN[m_pic.F[i].N[0]].NX, m_pic.VN[m_pic.F[i].N[0]].NY, m_pic.VN[m_pic.F[i].N[0]].NZ);//法向量
		glVertex3f(m_pic.V[m_pic.F[i].V[0]].X, m_pic.V[m_pic.F[i].V[0]].Y, m_pic.V[m_pic.F[i].V[0]].Z);		// 上顶点

		//glTexCoord2f(m_pic.VT[m_pic.F[i].T[1]].TU, m_pic.VT[m_pic.F[i].T[1]].TV);  //纹理
		glNormal3f(m_pic.VN[m_pic.F[i].N[1]].NX, m_pic.VN[m_pic.F[i].N[1]].NY, m_pic.VN[m_pic.F[i].N[1]].NZ);//法向量
		glVertex3f(m_pic.V[m_pic.F[i].V[1]].X, m_pic.V[m_pic.F[i].V[1]].Y, m_pic.V[m_pic.F[i].V[1]].Z);		// 左下

		//glTexCoord2f(m_pic.VT[m_pic.F[i].T[2]].TU, m_pic.VT[m_pic.F[i].T[2]].TV);  //纹理
		glNormal3f(m_pic.VN[m_pic.F[i].N[2]].NX, m_pic.VN[m_pic.F[i].N[2]].NY, m_pic.VN[m_pic.F[i].N[2]].NZ);//法向量
		glVertex3f(m_pic.V[m_pic.F[i].V[2]].X, m_pic.V[m_pic.F[i].V[2]].Y, m_pic.V[m_pic.F[i].V[2]].Z);		// 右下

		glEnd();
	}
}

void DrawScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);// 清除屏幕及深度缓存
	glLoadIdentity();// 重置模型观察矩阵

	//设置光源
	GLfloat glfLight[] = { -4.0f, 4.0f, 4.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, glfLight);

	// 设置相机位置和旋转角度，由鼠标和键盘控制
	glMatrixMode(GL_MODELVIEW);
	glTranslatef(0.0f, 0.0f, -5.0f);
	glRotated(x_rotate, 1.0f, 0.0f, 0.0f);
	glRotated(y_rotate, 0.0f, 1.0f, 0.0f);
	glRotated(z_rotate, 0.0f, 0.0f, 1.0f);

	glMatrixMode(GL_PROJECTION);
	glScalef(times, times, times);
	glOrtho(-1500, 1500, -1500, 1500, -2000, 2000);

	//绘制物体
	GLCube();
	//刷新屏幕
	glFlush();
	glutSwapBuffers();
}

void InitScene()
{
	ReadPIC();
	getF_Normal();
	glClearColor(0.000f, 0.000f, 0.000f, 1.0f); //Background color

	glEnable(GL_LIGHT0);//启用0号灯
	glEnable(GL_LIGHTING);//启用灯源
	glEnable(GL_DEPTH_TEST);//启用深度测试。　　根据坐标的远近自动隐藏被遮住的图形（材料）
	glEnable(GL_TEXTURE_2D);   // 启用二维纹理

   //定义黄铜材质
	static GLfloat glfMatAmbient[] = { 0.33f, 0.22f, 0.03f, 1.0f };
	static GLfloat glfMatDiffuse[] = { 0.78f, 0.57f, 0.11f, 1.0f };
	static GLfloat glfMatSpecular[] = { 0.99f, 0.91f, 0.81f, 1.0f };
	static GLfloat glfMatEmission[] = { 0.000f, 0.000f, 0.000f, 1.0f };
	static GLfloat fShininess = 27.800f;

	//指定用于光照计算的当前材质属性。参数face的取值可以是GL_FRONT、GL_BACK或GL_FRONT_AND_BACK，指出材质属性将应用于物体的哪面。
	glMaterialfv(GL_FRONT, GL_AMBIENT, glfMatAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, glfMatDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, glfMatSpecular);
	glMaterialfv(GL_FRONT, GL_EMISSION, glfMatEmission);
	glMaterialf(GL_FRONT, GL_SHININESS, fShininess);
}

void glclear()
{
	x_rotate = 0;
	y_rotate = 0;
	z_rotate = 0;
}

void mouseclick(int button, int state, int x, int y)
{
	if (state == GLUT_UP && button == GLUT_LEFT_BUTTON)
	{
		times = 0.05f + times;
		glclear();
		DrawScene();
	}

	else if (state == GLUT_UP && button == GLUT_RIGHT_BUTTON)
	{
		times = -0.05f + times;
		glclear();
		DrawScene();
	}
}

void KeyBoards(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'x':
		x_rotate = 30;
		DrawScene();
		break;
	case 'y':
		y_rotate = 30;
		DrawScene();
		break;
	case 'z':
		z_rotate = 30;
		DrawScene();
		break;
	}
}

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_SINGLE | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(600, 600);
	glutCreateWindow("Bunny");
	
	InitScene();
	glutMouseFunc(&mouseclick);
	glutKeyboardFunc(&KeyBoards);
	glutDisplayFunc(&DrawScene);
	glutMainLoop();
	return 0;
}