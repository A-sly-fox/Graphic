#include <GL/glut.h>
#include <math.h>
#define PI 3.1415926536

void SetupRC(void)
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

// �����Σ�����������½�XY��������Ͻ�XY���� 
void glRect(double leftX, double leftY, double rightX, double rightY, int MODE)
{ 
	glBegin(MODE); 
	//���½� 
	glVertex2f(leftX, leftY); 
	//���½� 
	glVertex2f(rightX, leftY); 
	//���Ͻ� 
	glVertex2f(rightX, rightY); 
	//���Ͻ� 
	glVertex2f(leftX, rightY); 
	glEnd(); 
}

//�������Σ����������������
void glTri(double x1, double y1, double x2, double y2, double x3, double y3, int MODE) {
	glBegin(MODE);
	//һ��
	glVertex2f(x1, y1);
	//����
	glVertex2f(x2, y2);
	//����
	glVertex2f(x3, y3);
	glEnd();
}

//�����ߣ����ƫ����XY����ʼ�Ļ��ȣ������Ļ��ȣ��뾶
void glArc(double x, double y, double start_angle, double end_angle, double radius, int MODE) {
	glBegin(MODE);
	//ÿ�λ����ӵĻ���
	double delta_angle = PI / 180;
	//��Բ��
	for (double i = start_angle; i <= end_angle; i += delta_angle) {
		//���Զ�λ�����Ǻ���ֵ
		double vx = x + radius * cos(i);
		double vy = y + radius * sin(i);
		glVertex2d(vx, vy);
	}
	glEnd();
}

void glscreen(int MODE){
	glBegin(MODE);
		glVertex2f(-0.4f, -0.25f);
		double delta_angle = PI / 180;
		for (double i = PI * 3 / 2; i >= PI ; i -= delta_angle) {
			double vx = -0.4 + 0.05 * cos(i);
			double vy = -0.2 + 0.05 * sin(i);
			glVertex2f(vx, vy);
		}
		glVertex2f(-0.45f, -0.2f);
		glVertex2f(-0.45f, 0.2f);
		for (double i = PI ; i >= PI / 2; i -= delta_angle) {
			double vx = -0.4 + 0.05 * cos(i);
			double vy = 0.2 + 0.05 * sin(i);
			glVertex2f(vx, vy);
		}
		glVertex2f(-0.4f, 0.25f);
		glVertex2f(0.4f, 0.25f);
		for (double i = PI / 2; i >= 0; i -= delta_angle) {
			double vx = 0.4 + 0.05 * cos(i);
			double vy = 0.2 + 0.05 * sin(i);
			glVertex2f(vx, vy);
		}
		glVertex2f(0.45f, 0.2f);
		glVertex2f(0.45f, -0.2f);
		for (double i = 0; i >= -PI / 2; i -= delta_angle) {
			double vx = 0.4 + 0.05 * cos(i);
			double vy = -0.2 + 0.05 * sin(i);
			glVertex2f(vx, vy);
		}
		glVertex2f(0.4f, -0.25f);
	glEnd();
}

void gldown(int MODE) {
	glBegin(MODE);
		double delta_angle = PI / 180;
		glVertex2f(-0.3f, -0.4f);
		for (double i = PI / 2; i <= PI; i += delta_angle) {
			double vx = -0.3 + 0.05 * cos(i);
			double vy = -0.45 + 0.05 * sin(i);
			glVertex2f(vx, vy);
		}
		glVertex2f(-0.35f, -0.45f);
		glVertex2f(-0.35f, -0.5f);
		for (double i = PI ; i <= PI * 3 / 2; i += delta_angle) {
			double vx = -0.3 + 0.05 * cos(i);
			double vy = -0.5 + 0.05 * sin(i);
			glVertex2f(vx, vy);
		}
		glVertex2f(-0.3f, -0.55f);
		glVertex2f(0.3f, -0.55f);
		for (double i = PI * 3 / 2; i <= PI * 2; i += delta_angle) {
			double vx = 0.3 + 0.05 * cos(i);
			double vy = -0.5 + 0.05 * sin(i);
			glVertex2f(vx, vy);
		}
		glVertex2f(0.35f, -0.5f);
		glVertex2f(0.35f, -0.45f);
		for (double i = 0; i <= PI / 2; i += delta_angle) {
			double vx = 0.3 + 0.05 * cos(i);
			double vy = -0.45 + 0.05 * sin(i);
			glVertex2f(vx, vy);
		}
		glVertex2f(0.3f, -0.4f);
	glEnd();
}


void myDisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0.7f, 0.7f, 0.7f);
	glRect(-0.5, -0.3, 0.5, 0.3, GL_POLYGON);
	glColor3f(0.0f, 0.0f, 0.0f);
	glRect(-0.5, -0.3, 0.5, 0.3, GL_LINE_LOOP);

	glColor3f(0.0f, 0.0f, 0.8f);
	glscreen(GL_POLYGON);
	glColor3f(0.0f, 0.0f, 0.0f);
	glscreen(GL_LINE_LOOP);

	glColor3f(0.5f, 0.5f, 0.5f);
	gldown(GL_POLYGON);
	glColor3f(0.0f, 0.0f, 0.0f);
	gldown(GL_LINE_LOOP);

	glColor3f(0.5f, 0.5f, 0.5f);
	glRect(-0.2, -0.45, 0.2, -0.3, GL_POLYGON);
	glColor3f(0.0f, 0.0f, 0.0f);
	glRect(-0.2, -0.45, 0.2, -0.3, GL_LINE_LOOP);
	glColor3f(0.0f, 0.0f, 0.0f);
	glRect(-0.2, -0.45, 0.2, -0.3, GL_LINE_LOOP);
	glColor3f(1.0f, 1.0f, 0.0f);
	glTri(-0.15, -0.15, 0.15, -0.15, 0, 0.15, GL_TRIANGLES);
	glColor3f(0.0f, 0.0f, 0.0f);
	glTri(-0.15, -0.15, 0.15, -0.15, 0, 0.15, GL_LINE_LOOP);
	glColor3f(1.0f, 0.0f, 0.0f);
	glArc(0.0, -0.05, 0.0, 2 * PI, 0.05, GL_POLYGON);
	glColor3f(0.0f, 0.0f, 0.0f);
	glArc(0.0, -0.05, 0.0, 2 * PI, 0.05, GL_LINE_LOOP);
	glFlush();
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowPosition(400, 0);
	glutInitWindowSize(800, 800);
	glutCreateWindow("OpenGL");
	glEnable(GL_POINT_SMOOTH);
	SetupRC();
	glutDisplayFunc(&myDisplay);
	glutMainLoop();
	return 0;
}