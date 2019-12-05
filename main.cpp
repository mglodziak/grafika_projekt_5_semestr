#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "GL\glew.h"
#include "GL\freeglut.h"
#include <string>
#include <iostream>

#include "shaderLoader.h" //narzŕdzie do │adowania i kompilowania shaderˇw z pliku

#include <fstream>

#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE
//#pragma warning(disable:4996)

//funkcje algebry liniowej
#include "glm/vec3.hpp" // glm::vec3
#include "glm/vec4.hpp" // glm::vec4
#include "glm/mat4x4.hpp" // glm::mat4
#include "glm/gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale, glm::perspective

using namespace std;
int element_size;
int vertices_size;

//Wymiary okna
int screen_width = 1024;
int screen_height = 768;


int pozycjaMyszyX; // na ekranie
int pozycjaMyszyY;
int mbutton; // wcisiety klawisz myszy

double kameraX = -140.0;
//double kameraY = 0.0;
double kameraZ = -65.0;
double kameraD = -5.0;
double kameraY = 0.0;
double kameraPredkosc;
double kameraKat = 0;
double kameraPredkoscObrotu;
double poprzednie_kameraX;
double poprzednie_kameraZ;
double poprzednie_kameraD;
double poprzednie_kameraY;


double rotation = 0;


//macierze
glm::mat4 MV; //modelview - macierz modelu i świata
glm::mat4 P;  //projection - macierz projekcji, czyli naszej perspektywy



float * vertices;
int * elements_triangles;
int * elements_quads;
float max_vert_global = 0;
float min_vert_global = 0;
float max_vert_x = 0;
float max_vert_y = 0;
float max_vert_z = 0;
float min_vert_x = 0;
float min_vert_y = 0;
float min_vert_z = 0;

//shaders
GLuint programID1 = 0;
GLuint programID2 = 0;

unsigned int VBO;

/*###############################################################*/
void mysz(int button, int state, int x, int y)
{
	mbutton = button;
	switch (state)
	{
	case GLUT_UP:
		break;
	case GLUT_DOWN:
		pozycjaMyszyX = x;
		pozycjaMyszyY = y;
		poprzednie_kameraX = kameraX;
		poprzednie_kameraZ = kameraZ;
		poprzednie_kameraD = kameraD;
		poprzednie_kameraY = kameraY;
		break;

	}
}
/*******************************************/
void mysz_ruch(int x, int y)
{
	if (mbutton == GLUT_LEFT_BUTTON)
	{
		kameraX = poprzednie_kameraX - (pozycjaMyszyX - x) * 0.1;
		kameraZ = poprzednie_kameraZ - (pozycjaMyszyY - y) * 0.1;
	}
	if (mbutton == GLUT_RIGHT_BUTTON)
	{
		kameraD = poprzednie_kameraD + (pozycjaMyszyY - y) * 0.1;
		kameraY = poprzednie_kameraY - (pozycjaMyszyX - x) * 0.1;
	}

}
/******************************************/

double dx = 0;
double skala = 0;

void klawisz(GLubyte key, int x, int y)
{
	switch (key) {

	case 27:    /* Esc - koniec */
		exit(1);
		break;

	case 'x':
		dx += 10;
		break;
	case '1':
		skala -= 0.1;

		break;
	case '2':
		skala += 0.1;

		break;
	}


}
/*###############################################################*/
void rysuj(void)
{



	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Kasowanie ekranu
	GLfloat color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glClearBufferfv(GL_COLOR, 0, color);

	MV = glm::mat4(1.0f);  //macierz jednostkowa
	MV = glm::translate(MV, glm::vec3(dx, 0, kameraD));
	MV = glm::rotate(MV, (float)glm::radians(kameraZ), glm::vec3(1, 0, 0));
	MV = glm::rotate(MV, (float)glm::radians(kameraX), glm::vec3(0, 0, 1));
	//MV = glm::rotate(MV, (float)glm::radians(kameraY), glm::vec3(0, 0, 1));

	MV = glm::scale(MV, glm::vec3(1 + skala, 1 + skala, 1 + skala));

	glm::mat4 MVP = P * MV;
	glm::mat4 MVP2 = P * MV;

	/*Zmienne   jednorodne   (ang. uniform variable), zwane  także  zmiennym  globalnymi,sązmiennymi,
		których  wartośc  jest  stała  w  obrębie  obiektu programu. Shadery  mogą zmienne jednorodne tylko  odczytywac */

	glUseProgram(programID2); //u┐yj programu, czyli naszego shadera	
	GLuint MVP_id2 = glGetUniformLocation(programID2, "MVP"); // pobierz lokalizację zmiennej 'uniform' "MV" w programie
	glUniformMatrix4fv(MVP_id2, 1, GL_FALSE, &(MVP2[0][0]));	   // wyślij tablicę mv do lokalizacji "MV", która jest typu mat4	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //wypełnienie
	glDrawElements(GL_TRIANGLES, element_size, GL_UNSIGNED_INT, 0);

	glUseProgram(programID1); //u┐yj programu, czyli naszego shadera	
	GLuint MVP_id = glGetUniformLocation(programID1, "MVP"); // pobierz lokalizację zmiennej 'uniform' "MV" w programie
	glUniformMatrix4fv(MVP_id, 1, GL_FALSE, &(MVP[0][0]));	   // wyślij tablicę mv do lokalizacji "MV", która jest typu mat4	
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_TRIANGLES, element_size, GL_UNSIGNED_INT, 0); // Zaczynamy od 0 i rysujemy wszystkie wierzcho│ki



	glFlush();
	glutSwapBuffers();

}
/*###############################################################*/
void rozmiar(int width, int height)
{
	screen_width = width;
	screen_height = height;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, screen_width, screen_height);

	P = glm::perspective(glm::radians(60.0f), (GLfloat)screen_width / (GLfloat)screen_height, 0.3f, 5000.0f);

	glutPostRedisplay(); // Przerysowanie sceny
}

/*###############################################################*/
void idle()
{

	glutPostRedisplay();
}

/*###############################################################*/
GLfloat k = 0.05;
GLfloat ad = 0.0;

void timer(int value) {

	ad += k;

	if (ad > 1 || ad < 0)
		k = -k;

	//GLfloat attrib[] = { ad, 0.0f,0.0f };
	// Aktualizacja wartości atrybutu wejściowego 1.
	//glVertexAttrib3fv(1, attrib);

	/*

	W vertex_shader np:
	layout (location = 1) in vec3 incolor;

	*/
	glutTimerFunc(20, timer, 0);
}

/*###############################################################*/

int id = 0;

int main(int argc, char *argv[])
{
	cout << "Podaj nazwe pliku: ";
	string name;
	cin >> name;
	ifstream file(name);
	if (!file.is_open())
	{
		perror("error file");
		getchar();
		getchar();
		return -1;
	}
	cout << "Instrukcja\n\nKlawisz '1' - oddalanie\nKlawisz '2' - przyblizanie\nLewy przycisk myszy - obracanie\nPrawy przycisk myszy - przyblizanie, oddalanie";
	string line;
	string word;
	string ilosc_danych;
	string ilosc_elementow;
	int num = 0;

	while (word != "end_header") //czytanie do end headera
	{
		if (word == "vertex")
		{
			file >> word;
			ilosc_danych = word;
			id = stoi(ilosc_danych);
		}
		else if (word == "face")
		{
			file >> word;
			ilosc_elementow = word;
		}
		else
		{
			file >> word;
		}
	}

	int sizexx = stoi(ilosc_danych) * 3;
	vertices_size = sizexx;
	//cout << sizexx << endl;
	vertices = new float[sizexx];
	int sizeyy = stoi(ilosc_elementow) * 3;
	//cout << sizeyy << endl;
	element_size = sizeyy;
	elements_triangles = new int[sizeyy];


	int i = 0;
	while (i < stoi(ilosc_danych) * 3)
	{
		string tmp;
		file >> tmp;
		float tmp2 = stof(tmp);
		vertices[i] = tmp2;
		if (i % 3 == 0)
		{
			if (min_vert_x > tmp2)
			{
				min_vert_x = tmp2;
			}
			if (max_vert_x < tmp2)
			{
				max_vert_x = tmp2;
			}
		}
		if (i % 3 == 1)
		{
			if (min_vert_y > tmp2)
			{
				min_vert_y = tmp2;
			}
			if (max_vert_y < tmp2)
			{
				max_vert_y = tmp2;
			}
		}
		if (i % 3 == 2)
		{
			if (min_vert_z > tmp2)
			{
				min_vert_z = tmp2;
			}
			if (max_vert_z < tmp2)
			{
				max_vert_z = tmp2;
			}
		}
		if (min_vert_global > tmp2)
		{
			min_vert_global = tmp2;
		}
		if (max_vert_global < tmp2)
		{
			max_vert_global = tmp2;
		}
		//cout << vertices[i] << '\n';
		i++;
	}

	int j = 0;
	int ix = 0;

	while (j < stoi(ilosc_elementow) * 4)
	{
		if (j % 4 == 0)
		{
			string tmp;
			file >> tmp;
			j++;
		}
		else
		{
			string tmp;
			file >> tmp;
			int tmp2 = stoi(tmp);
			//cout << tmp2 << endl;
			elements_triangles[ix] = tmp2;
			//cout << elements_triangles[ix] << '\n';
			j++;
			ix++;
		}
	}

	//		cout << vertices[976*3-1] << endl;
			//cout << elements_triangles[4580] << endl;
	/*
	cout << min_vert_x << endl;
	cout << min_vert_y << endl;
	cout << min_vert_z << endl;
	cout << max_vert_x << endl;
	cout << max_vert_y << endl;
	cout << max_vert_z << endl;
	cout << max_vert_global << endl;
	cout << min_vert_global << endl;
	cout << endl << "TESTY" << endl;
	*/
	file.close();


	for (int i = 0; i < vertices_size; ++i)
	{
		if (i % 3 == 0)
		{
			vertices[i] = (vertices[i] - min_vert_x) / (max_vert_z - min_vert_z);
		}
		if (i % 3 == 1)
		{
			vertices[i] = (vertices[i] - min_vert_y) / (max_vert_z - min_vert_z);
		}
		if (i % 3 == 2)
		{
			vertices[i] = (vertices[i] - min_vert_z) / (max_vert_z - min_vert_z);
		}

	}






	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(screen_width, screen_height);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Przyklad 3");

	glewInit(); //init rozszerzeszeń OpenGL z biblioteki GLEW

	glutDisplayFunc(rysuj);			// def. funkcji rysuj¦cej
	glutIdleFunc(idle);			// def. funkcji rysuj¦cej w czasie wolnym procesoora (w efekcie: ci¦gle wykonywanej)
	glutTimerFunc(20, timer, 0);
	glutReshapeFunc(rozmiar); // def. obs-ugi zdarzenia resize (GLUT)

	glutKeyboardFunc(klawisz);		// def. obsługi klawiatury
	glutMouseFunc(mysz); 		// def. obsługi zdarzenia przycisku myszy (GLUT)
	glutMotionFunc(mysz_ruch); // def. obsługi zdarzenia ruchu myszy (GLUT)


	glEnable(GL_DEPTH_TEST);

	//glPointSize(3.0f);

	//tworzenie bufora wierzcholków
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices_size * 3 * sizeof(float), vertices, GL_STATIC_DRAW);

	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, element_size * 3 * sizeof(int), elements_triangles, GL_STATIC_DRAW);

	/* úadowanie shadera, tworzenie programu *************************/
	/* i linkowanie go oraz sprawdzanie b│ŕdˇw! **********************/

	glEnableVertexAttribArray(0); // pierwszy buform atrybuˇw: wierzcho│ki
	glVertexAttribPointer(
		0,                  // atrybut 0. musi odpowiadaŠ atrybutowi w programie shader
		3,                  // wielkoťŠ (x,y,z)
		GL_FLOAT,           // typ
		GL_FALSE,           // czy znormalizowany [0-1]?
		0,                  // stride
		(void*)0            // array buffer offset
		);
	programID1 = loadShaders("vertex_shader.glsl", "fragment_shader1.glsl");
	programID2 = loadShaders("vertex_shader.glsl", "fragment_shader2.glsl");

	//glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glEnableVertexAttribArray(0); // pierwszy buform atrybuˇw: wierzcho│ki
	glVertexAttribPointer(
		0,                  // atrybut 0. musi odpowiadaŠ atrybutowi w programie shader
		3,                  // wielkoťŠ (x,y,z)
		GL_FLOAT,           // typ
		GL_FALSE,           // czy znormalizowany [0-1]?
		0,                  // stride
		(void*)0            // array buffer offset
		);

	glutMainLoop();					// start

	glDeleteBuffers(1, &VBO);

	delete (vertices);
	delete (elements_triangles);
	return(0);
}

