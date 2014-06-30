/*
Project: OpenGL Grid Creation
Author : Nishant Dania
Email: nishantdania@gmail.com
Website: nishantdania.github.io
Last Modified: 15 June,2014
*/ 

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>

#define DEPTH 32
#define NUM_VERTICES (DEPTH+1)*(DEPTH+1)
#define NUM_INDICES 2*3*DEPTH*DEPTH

#define BUFFER_OFFSET(i) ((char *)NULL + (i))
GLuint shaderProgramID;
GLuint vao = 0;
GLuint vbo;
GLuint positionID, colorID;
GLuint indexBufferID;
GLfloat var;
float timer= 0.01;
float diff = 0.005;

static char* readFile(const char* filename) {
	// Open the file
	FILE* fp = fopen (filename, "r");
	// Move the file pointer to the end of the file and determing the length
	fseek(fp, 0, SEEK_END);
	long file_length = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char* contents = new char[file_length+1];
	// zero out memory
	for (int i = 0; i < file_length+1; i++) {
		contents[i] = 0;
	}
	// Here's the actual read
	fread (contents, 1, file_length, fp);
	// This is how you denote the end of a string in C
	contents[file_length+1] = '\0';
	fclose(fp);
	return contents;
}

bool compiledStatus(GLint shaderID){
	GLint compiled = 0;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compiled);
	if (compiled) {
		return true;
	}
	else {
		GLint logLength;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logLength);
		char* msgBuffer = new char[logLength];
		glGetShaderInfoLog(shaderID, logLength, NULL, msgBuffer);
		printf ("%s\n", msgBuffer);
		delete (msgBuffer);
		return false;
	}
}

GLuint makeVertexShader(const char* shaderSource) {
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource (vertexShaderID, 1, (const GLchar**)&shaderSource, NULL);
	glCompileShader(vertexShaderID);
	bool compiledCorrectly = compiledStatus(vertexShaderID);
	if (compiledCorrectly) {
		return vertexShaderID;
	}
	return -1;
}

GLuint makeFragmentShader(const char* shaderSource) {
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderID, 1, (const GLchar**)&shaderSource, NULL);
	glCompileShader(fragmentShaderID);
	bool compiledCorrectly = compiledStatus(fragmentShaderID);
	if (compiledCorrectly) {
		return fragmentShaderID;
	}
	return -1;
}

GLuint makeShaderProgram (GLuint vertexShaderID, GLuint fragmentShaderID) {
	GLuint shaderID = glCreateProgram();
	glAttachShader(shaderID, vertexShaderID);
	glAttachShader(shaderID, fragmentShaderID);
	glLinkProgram(shaderID);
	return shaderID;
}

void changeViewport(int w, int h){
	glViewport(0, 0, w, h);
}

void render() {
	glUniform1f(var, timer);
	timer+=diff;
	
	if (timer>=0.8 || timer<0.01)
	{
		diff = diff * -1;
	}

	// glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glPointSize(6);
	// glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	// glLoadIdentity();
	gluLookAt(0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f);
	glDrawElements (GL_TRIANGLES, NUM_INDICES, GL_UNSIGNED_INT, NULL);
	glutSwapBuffers();
}

void Timer(int iUnused){
	glutPostRedisplay();
	glutTimerFunc(30,Timer,0);
}

int main (int argc, char** argv) {
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutCreateWindow("Grid Plane");
	glutReshapeFunc(changeViewport);
	glutDisplayFunc(render);
	glewInit();

	//loop for vertex creation
	GLfloat vertices[3*NUM_VERTICES];
	int count = 0;

	for (int y = 0; y < DEPTH+1; ++y)
	{
		for (int x = 0; x < DEPTH+1; ++x)
		{
			vertices[count++] = (x - (float)(DEPTH/2))/DEPTH;
			vertices[count++] = (y - (float)(DEPTH/2))/DEPTH;
			vertices[count++] = 0.0f;
		}
	}

	GLuint indices[NUM_INDICES];
	count = 0;

	//loop for index creation
	for (int i = 0; i < DEPTH; ++i)
	{
		for (int start = ((DEPTH*i)+i)+1; start < ((DEPTH*i)+i)+DEPTH+1; ++start)
		{
			indices[count++]=start;
			indices[count++]=start+DEPTH;
			indices[count++]=start-1;

			indices[count++]=start;
			indices[count++]=start+DEPTH+1;
			indices[count++]=start+DEPTH;
			

		}
	}

	char* vertexShaderSourceCode = readFile("vertexShader.glsl");
	char* fragmentShaderSourceCode = readFile("fragmentShader.glsl");
	GLuint vertShaderID = makeVertexShader(vertexShaderSourceCode);
	GLuint fragShaderID = makeFragmentShader(fragmentShaderSourceCode);
	shaderProgramID = makeShaderProgram(vertShaderID, fragShaderID);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 3*NUM_VERTICES*sizeof(GLfloat), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 3*NUM_VERTICES*sizeof(GLfloat), vertices);

	glGenBuffers(1, &indexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, NUM_INDICES*sizeof(GLuint), indices, GL_STATIC_DRAW);

	positionID = glGetAttribLocation(shaderProgramID, "s_vPosition");
	var = glGetUniformLocation(shaderProgramID, "var");

	glVertexAttribPointer(positionID, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glUseProgram(shaderProgramID);
	glEnableVertexAttribArray(positionID);
	Timer(0);
	glutMainLoop();

	return 0;
}