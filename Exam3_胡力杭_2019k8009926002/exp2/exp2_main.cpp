#include <vector>
#include <GL/glut.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <math.h>
#include <cmath>
#include <cstring>
using namespace std;

char input_name[256];
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
	int N[3];
};
class PIC
{
public:
	vector<POINT3> V;
	vector<Normal> VN;
	vector<Face> F;
};

PIC m_pic;
float times = 1.0;
float x_rotate = 0.0;
float y_rotate = 0.0;
float z_rotate = 0.0;

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
	for (int i = 0; i < m_pic.VN.size(); i++)
	{
		length = sqrt(pow(m_pic.VN[i].NX, 2) + pow(m_pic.VN[i].NY, 2) + pow(m_pic.VN[i].NZ, 2));
		m_pic.VN[i].NX /= length;
		m_pic.VN[i].NY /= length;
		m_pic.VN[i].NZ /= length;
	}
}

void ReadPIC()
{
	ifstream ifs(input_name);
	string s;
	Face* f;
	POINT3* v;
	Normal* vn;
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
			v->X *= 1000;
			v->Y *= 1000;
			v->Z *= 1000;
			m_pic.V.push_back(*v);
		}
		else if (s[0] == 'f') {
			istringstream in(s);
			vn = new Normal();
			vn->NX = 0;
			vn->NY = 0;
			vn->NZ = 0;
			m_pic.VN.push_back(*vn);
			f = new Face();
			string head;
			in >> head >> f->V[0] >> f->V[1] >> f->V[2];
			f->V[0] -= 1;
			f->V[1] -= 1;
			f->V[2] -= 1;
			f->N[0] = f->V[0];
			f->N[1] = f->V[1];
			f->N[2] = f->V[2];
			m_pic.F.push_back(*f);
		}
	}
	getF_Normal();
}

void GLCube()
{
	double a, b, c;
	for (int i = 0; i < m_pic.F.size(); i++)
	{
		glBegin(GL_TRIANGLES);

		a = (m_pic.VN[m_pic.F[i].N[0]].NX + m_pic.VN[m_pic.F[i].N[1]].NX + m_pic.VN[m_pic.F[i].N[2]].NX) / 3;
		b = (m_pic.VN[m_pic.F[i].N[0]].NY + m_pic.VN[m_pic.F[i].N[1]].NY + m_pic.VN[m_pic.F[i].N[2]].NY) / 3;
		c = (m_pic.VN[m_pic.F[i].N[0]].NZ + m_pic.VN[m_pic.F[i].N[1]].NZ + m_pic.VN[m_pic.F[i].N[2]].NZ) / 3;

		glNormal3f(a, b, c);//������
		glVertex3f(m_pic.V[m_pic.F[i].V[0]].X, m_pic.V[m_pic.F[i].V[0]].Y, m_pic.V[m_pic.F[i].V[0]].Z);		// �϶���

		glNormal3f(a, b, c);//������
		glVertex3f(m_pic.V[m_pic.F[i].V[1]].X, m_pic.V[m_pic.F[i].V[1]].Y, m_pic.V[m_pic.F[i].V[1]].Z);		// ����

		glNormal3f(a, b, c);//������
		glVertex3f(m_pic.V[m_pic.F[i].V[2]].X, m_pic.V[m_pic.F[i].V[2]].Y, m_pic.V[m_pic.F[i].V[2]].Z);		// ����

		glEnd();
	}
}

void DrawScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);// �����Ļ����Ȼ���
	glLoadIdentity();// ����ģ�͹۲����

	//���ù�Դ
	GLfloat glfLight[] = { -4.0f, 4.0f, 4.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, glfLight);

	// �������λ�ú���ת�Ƕȣ������ͼ��̿���
	glMatrixMode(GL_MODELVIEW);
	glTranslatef(0.0f, 0.0f, -5.0f);
	glRotated(x_rotate, 1.0f, 0.0f, 0.0f);
	glRotated(y_rotate, 0.0f, 1.0f, 0.0f);
	glRotated(z_rotate, 0.0f, 0.0f, 1.0f);

	glMatrixMode(GL_PROJECTION);
	glScalef(times, times, times);
	glOrtho(-1500, 1500, -1500, 1500, -2000, 2000);

	//��������
	GLCube();
	//ˢ����Ļ
	glFlush();
	glutSwapBuffers();
}

void InitScene()
{
	ReadPIC();
	getF_Normal();
	glClearColor(0.000f, 0.000f, 0.000f, 1.0f); //Background color

	glEnable(GL_LIGHT0);//����0�ŵ�
	glEnable(GL_LIGHTING);//���õ�Դ
	glEnable(GL_DEPTH_TEST);//������Ȳ��ԡ��������������Զ���Զ����ر���ס��ͼ�Σ����ϣ�
	glEnable(GL_TEXTURE_2D);   // ���ö�ά����

   //�����ͭ����
	static GLfloat glfMatAmbient[] = { 0.135f,0.2225f,0.1575f,0.95f };
	static GLfloat glfMatDiffuse[] = { 0.54f,0.89f,0.63f,0.95f };
	static GLfloat glfMatSpecular[] = { 0.316228f,0.316228f,0.316228f,0.95f };
	static GLfloat glfMatEmission[] = { 0.000f, 0.000f, 0.000f, 1.0f };
	static GLfloat fShininess = 12.800f;

	//ָ�����ڹ��ռ���ĵ�ǰ�������ԡ�����face��ȡֵ������GL_FRONT��GL_BACK��GL_FRONT_AND_BACK��ָ���������Խ�Ӧ������������档
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
	strcpy(input_name, argv[1]);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_SINGLE | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Dragon");

	InitScene();
	glutMouseFunc(&mouseclick);
	glutKeyboardFunc(&KeyBoards);
	glutDisplayFunc(&DrawScene);
	glutMainLoop();
	return 0;
}