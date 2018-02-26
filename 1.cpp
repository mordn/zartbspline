#include <GL/glut.h>
#include <vector>

GLsizei winWidth = 800, winHeight = 600;

typedef struct point2d { GLfloat x, y; } POINT2D;
typedef struct { GLdouble x, y, w, z; } VECTOR4;
typedef GLdouble MATRIX4[4][4];
std::vector<POINT2D> pontok;
GLint dragged = -1;

POINT2D initPoint2D(GLfloat x, GLfloat y) {
	POINT2D P;
	P.x = x;
	P.y = y;
	return P;
}

VECTOR4 initVector4(GLdouble x, GLdouble y, GLdouble w, GLdouble z) {
	VECTOR4 P;
	P.x = x;
	P.y = y;
	P.w = w;
	P.z = z;
	return P;
}

VECTOR4 mulMatrixVector(MATRIX4 A, VECTOR4 v) {
	return initVector4(
		A[0][0] * v.x + A[0][1] * v.y + A[0][2] * v.w + A[0][3] * v.z,
		A[1][0] * v.x + A[1][1] * v.y + A[1][2] * v.w + A[1][3] * v.z,
		A[2][0] * v.x + A[2][1] * v.y + A[2][2] * v.w + A[2][3] * v.z,
		A[3][0] * v.x + A[3][1] * v.y + A[3][2] * v.w + A[3][3] * v.z);
}

VECTOR4  T,temp;
GLdouble M[4][4];

void init()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f, winWidth, 0.0f, winHeight, 0.0f, 1.0f);
	glLineWidth(2.0);
	glPointSize(4.0);

	M[0][0] = -1.0;	 M[0][1] =  3.0;  M[0][2] = -3.0;  M[0][3] = 1.0;
	M[1][0] =  3.0;	 M[1][1] = -6.0;  M[1][2] =  0.0;  M[1][3] = 4.0;
	M[2][0] = -3.0;	 M[2][1] =  3.0;  M[2][2] =  3.0;  M[2][3] = 1.0;
	M[3][0] =  1.0;	 M[3][1] =  0.0;  M[3][2] =  0.0;  M[3][3] = 0.0;

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			M[i][j] /= 6.0;
}

void TopKek()
{
	glClear(GL_COLOR_BUFFER_BIT);
	int n = pontok.size();

	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POINTS);
	for (int i = 0; i < n; i++) {
		glVertex2f(pontok[i].x, pontok[i].y);
	}
	glEnd();

	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	for (int i = 0; i < n-1; i++) {
		glVertex2f(pontok[i].x, pontok[i].y);
		glVertex2f(pontok[i+1].x, pontok[i+1].y);
	}
	glEnd();

	if(n > 3){
		for (int i = 0; i < n; i++){
			glColor3f(0.0f, 1.0f, 0.0f);
			glBegin(GL_LINE_STRIP);
			for (GLfloat t = 0; t <= 1; t += 0.01){
				T.x = t * t * t;
				T.y = t * t;
				T.w = t;
				T.z = 1;
				temp = mulMatrixVector(M, T);
				glVertex2d(pontok[i%n].x*temp.x + pontok[(i+1)%n].x*temp.y + pontok[(i+2)%n].x*temp.w + pontok[(i+3)%n].x*temp.z,
						   pontok[i%n].y*temp.x + pontok[(i+1)%n].y*temp.y + pontok[(i+2)%n].y*temp.w + pontok[(i+3)%n].y*temp.z);
			}
			glEnd();
		}
	}
	glFlush();
	glutSwapBuffers();
}

GLfloat dist(POINT2D P1, POINT2D P2) {
	GLfloat t1 = P1.x - P2.x;
	GLfloat t2 = P1.y - P2.y;
	return t1 * t1 + t2 * t2;
}

GLint getActivePoint(GLint sens, GLint x, GLint y) { 
	GLint i, s = sens * sens;
	POINT2D P = initPoint2D(x, y);

	for (i = 0; i < pontok.size(); i++)
		if (dist(pontok[i], P) < s)
			return i;
	return -1;
}

void processMouse(GLint button, GLint action, GLint xMouse, GLint yMouse) {
	GLint i;
	if (button == GLUT_LEFT_BUTTON && action == GLUT_DOWN){
		if ((i = getActivePoint(8, xMouse, winHeight - yMouse)) != -1)
			dragged = i;
		else{
			pontok.push_back(initPoint2D(xMouse, winHeight - yMouse));
		}	
	}
	if (button == GLUT_LEFT_BUTTON && action == GLUT_UP)
		dragged = -1;
	if (button == GLUT_RIGHT_BUTTON && action == GLUT_DOWN) {
		pontok.clear();
	}
}

void processMouseActiveMotion(GLint xMouse, GLint yMouse) {
	if (dragged >= 0) {
		pontok[dragged].x = xMouse;
		pontok[dragged].y = winHeight - yMouse;
		glutPostRedisplay();
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);                         // Initialize GLUT.
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);   // Set display mode.
	glutInitWindowPosition(50, 100);   // Set top-left display-window position.
	glutInitWindowSize(800, 600);      // Set display-window width and height.
	glutCreateWindow("Zárt B-spline görbe"); // Create display window.
	init();                            // Execute initialization procedure.
	glutDisplayFunc(TopKek);       // Send graphics to display window.
	glutMouseFunc(processMouse);
	glutMotionFunc(processMouseActiveMotion);
	glutMainLoop();                    // Display everything and wait.
	return 0;
}