#include <GL/glut.h>
#include <stdio.h>

int color, num, shape, mod, first = 0, r1, r2, r3, a[100];

void color_menu(int value) {
    color = value;
}

void shape_menu(int value) {
    shape = value;
}

void mymenu(int value) {
    if (value == 2) {
        mod = 1;
        num = 0;
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_STENCIL_BUFFER_BIT);
        glFlush();
        printf("����\n");
    }
    if (value == 3)
        mod = 0;
}

void menu() {
    r1 = glutCreateMenu(color_menu);
    glutAddMenuEntry("Green", 1);
    glutAddMenuEntry("Red", 2);
    glutAddMenuEntry("Blue", 3);
    
    r2 = glutCreateMenu(shape_menu);
    glutAddMenuEntry("Line segment", 1);
    glutAddMenuEntry("Rect", 2);
    glutAddMenuEntry("Triangle", 3);
    
    glutCreateMenu(mymenu);
    glutAddSubMenu("Choose color", r1);
    glutAddSubMenu("Choose Shape", r2);
    glutAddMenuEntry("Clear Screen", 2);
    glutAddMenuEntry("Draw Mode", 3);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void drawOBjects(GLenum mode) {
    int i;
    for (i = 0; i < num; i++) {
        switch (a[i * 6 + 4]) {
            case 1:glColor3f(0.0, 1.0, 0.0);
                break;
            case 2:glColor3f(1.0, 0.0, 0.0);
                break;
            case 3:glColor3f(0.0, 0.0, 1.0);
                break;
        }
        switch (a[i * 6 + 5]) {
            case 1:
                glBegin(GL_LINES);
                glVertex2f(a[i * 6], a[i * 6 + 1]);
                glVertex2f(a[i * 6 + 2], a[i * 6 + 3]);
                glEnd();
                break;
            case 2:
                glRectf(a[i * 6], a[i * 6 + 1], a[i * 6 + 2], a[i * 6 + 3]);
                break;
            case 3:
                glBegin(GL_TRIANGLES);
                glVertex2f(a[i * 6], a[i * 6 + 1]);
                glVertex2f(a[i * 6 + 2], a[i * 6 + 3]);
                glVertex2f(a[i * 6], a[i * 6 + 3]);
                glEnd();
                break;
        }
    }
}

void display(){
    glClear(GL_COLOR_BUFFER_BIT);
    drawOBjects(GL_RENDER);
    glFlush();
}

void mouseclick(int button, int state, int x, int y){
    if (!mod && num < 16 && state == GLUT_DOWN && button == GLUT_LEFT_BUTTON) {
        a[num * 6] = x;
        a[num * 6 + 1] = 400 - y;
        first++;
        printf("a[num * 6]:%d\n", a[num * 6]);
        printf("a[num * 6 +1]:%d\n", a[num * 6 + 1]);
    }
    if (!mod && num < 16 && state == GLUT_UP && button == GLUT_LEFT_BUTTON && first && (x != a[num * 6] || y != a[num * 6 + 1])) {
        a[num * 6 + 2] = x;
        a[num * 6 + 3] = 400 - y;
        a[num * 6 + 4] = color;
        a[num * 6 + 5] = shape;
        first--;
        printf("num:%d\n", num + 1);
        num++;
        display();
    }
}

void Init(){
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(400, 400);

    glutCreateWindow("Simple");
    menu();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(200, 200, 100, 200, 200, 0, 0, 1, 0);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-200, 200, -200, 200, 0, 200);
}

int main(int argc, char* argv[]){
    glutInit(&argc, argv);
    Init();
    glutMouseFunc(&mouseclick);
    glutDisplayFunc(&display);
    glutMainLoop();
    return 0;
}
