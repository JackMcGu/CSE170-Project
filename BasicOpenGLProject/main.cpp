
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <iostream>
#include <vector>
#include "shader.h"
#include "shaderprogram.h"

/*=================================================================================================
	DOMAIN
=================================================================================================*/

// Window dimensions
const int InitWindowWidth = 800;
const int InitWindowHeight = 800;
int WindowWidth = InitWindowWidth;
int WindowHeight = InitWindowHeight;

// Last mouse cursor position
int LastMousePosX = 0;
int LastMousePosY = 0;

// Arrays that track which keys are currently pressed
bool key_states[256];
bool key_special_states[256];
bool mouse_states[8];

// Other parameters
bool draw_wireframe = false;
bool flatshading = true;
bool shownormals = false;

// torus stuff
int n = 20;
float R = 0.8f;
float r = 0.5f;
int indicessize;

//mazestuff
const int mazesize = 16;

/*=================================================================================================
	SHADERS & TRANSFORMATIONS
=================================================================================================*/

ShaderProgram PassthroughShader;
ShaderProgram PerspectiveShader;

glm::mat4 PerspProjectionMatrix(1.0f);
glm::mat4 PerspViewMatrix(1.0f);
glm::mat4 PerspModelMatrix(1.0f);

float perspZoom = 1.0f, perspSensitivity = 0.35f;
float perspRotationX = 0.0f, perspRotationY = 0.0f;

/*=================================================================================================
	OBJECTS
=================================================================================================*/

//VAO -> the object "as a whole", the collection of buffers that make up its data
//VBOs -> the individual buffers/arrays with data, for ex: one for coordinates, one for color, etc.

GLuint axis_VAO;
GLuint axis_VBO[2];

GLuint torus_VAO, torus_VBO, torus_EBO, torus_NBO;

float axis_vertices[] = {
	//x axis
	-1.0f,  0.0f,  0.0f, 1.0f,
	1.0f,  0.0f,  0.0f, 1.0f,
	//y axis
	0.0f, -1.0f,  0.0f, 1.0f,
	0.0f,  1.0f,  0.0f, 1.0f,
	//z axis
	0.0f,  0.0f, -1.0f, 1.0f,
	0.0f,  0.0f,  1.0f, 1.0f
};

float axis_colors[] = {
	//x axis
	1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,
	//y axis
	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,
	//z axis
	0.0f, 0.0f, 1.0f, 1.0f,
	0.0f, 0.0f, 1.0f, 1.0f
};


std::string maze[mazesize] =	{
							"## ####### #####",
							"##     ### #####",
							"###### ### #####",
							"#      ### #####",
							"# ######## #####",
							"#      ### #####",
							"### ## ### #####",
							"### ###### #####",
							"## ####### #####",
							"##     ### #####",
							"###### ### #####",
							"#      ### #####",
							"# ######## #####",
							"#      ### #####",
							"### ## ### #####",
							"### ###### #####"
						};

std::vector<float> genmazeVertices()
{
	std::vector<float> vertices;
	std::vector<float> xyz_positions;
	float offset = mazesize * 0.1 / 2;
	for (int i = 0; i < mazesize; i++)
	{
		for (int j = 0; j < mazesize; j++)
		{
			if (maze[i][j] == '#')
			{

				float p0[3] = { (i * 0.1) + 0.0 - offset, (j * 0.1) + 0.0 - offset, 0.0 };
				float p1[3] = { (i * 0.1) + 0.1 - offset, (j * 0.1) + 0.0 - offset, 0.0 };
				float p2[3] = { (i * 0.1) + 0.1 - offset, (j * 0.1) + 0.1 - offset, 0.0 };
				float p3[3] = { (i * 0.1) + 0.0 - offset, (j * 0.1) + 0.1 - offset, 0.0 };
				float p4[3] = { (i * 0.1) + 0.0 - offset, (j * 0.1) + 0.0 - offset, 0.1 };
				float p5[3] = { (i * 0.1) + 0.1 - offset, (j * 0.1) + 0.0 - offset, 0.1 };
				float p6[3] = { (i * 0.1) + 0.1 - offset, (j * 0.1) + 0.1 - offset, 0.1 };
				float p7[3] = { (i * 0.1) + 0.0 - offset, (j * 0.1) + 0.1 - offset, 0.1 };

				//ABC
				xyz_positions.push_back(p0[0]);
				xyz_positions.push_back(p0[1]);
				xyz_positions.push_back(p0[2]);
				xyz_positions.push_back(p1[0]);
				xyz_positions.push_back(p1[1]);
				xyz_positions.push_back(p1[2]);
				xyz_positions.push_back(p3[0]);
				xyz_positions.push_back(p3[1]);
				xyz_positions.push_back(p3[2]);
				//ACD
				xyz_positions.push_back(p3[0]);
				xyz_positions.push_back(p3[1]);
				xyz_positions.push_back(p3[2]);
				xyz_positions.push_back(p1[0]);
				xyz_positions.push_back(p1[1]);
				xyz_positions.push_back(p1[2]);
				xyz_positions.push_back(p2[0]);
				xyz_positions.push_back(p2[1]);
				xyz_positions.push_back(p2[2]);
				//AEF
				xyz_positions.push_back(p1[0]);
				xyz_positions.push_back(p1[1]);
				xyz_positions.push_back(p1[2]);
				xyz_positions.push_back(p5[0]);
				xyz_positions.push_back(p5[1]);
				xyz_positions.push_back(p5[2]);
				xyz_positions.push_back(p2[0]);
				xyz_positions.push_back(p2[1]);
				xyz_positions.push_back(p2[2]);
				//ADE
				xyz_positions.push_back(p2[0]);
				xyz_positions.push_back(p2[1]);
				xyz_positions.push_back(p2[2]);
				xyz_positions.push_back(p5[0]);
				xyz_positions.push_back(p5[1]);
				xyz_positions.push_back(p5[2]);
				xyz_positions.push_back(p6[0]);
				xyz_positions.push_back(p6[1]);
				xyz_positions.push_back(p6[2]);
				//BCG
				xyz_positions.push_back(p5[0]);
				xyz_positions.push_back(p5[1]);
				xyz_positions.push_back(p5[2]);
				xyz_positions.push_back(p4[0]);
				xyz_positions.push_back(p4[1]);
				xyz_positions.push_back(p4[2]);
				xyz_positions.push_back(p6[0]);
				xyz_positions.push_back(p6[1]);
				xyz_positions.push_back(p6[2]);
				//BHD
				xyz_positions.push_back(p6[0]);
				xyz_positions.push_back(p6[1]);
				xyz_positions.push_back(p6[2]);
				xyz_positions.push_back(p4[0]);
				xyz_positions.push_back(p4[1]);
				xyz_positions.push_back(p4[2]);
				xyz_positions.push_back(p7[0]);
				xyz_positions.push_back(p7[1]);
				xyz_positions.push_back(p7[2]);
				//BEH
				xyz_positions.push_back(p4[0]);
				xyz_positions.push_back(p4[1]);
				xyz_positions.push_back(p4[2]);
				xyz_positions.push_back(p0[0]);
				xyz_positions.push_back(p0[1]);
				xyz_positions.push_back(p0[2]);
				xyz_positions.push_back(p7[0]);
				xyz_positions.push_back(p7[1]);
				xyz_positions.push_back(p7[2]);
				//BCF
				xyz_positions.push_back(p7[0]);
				xyz_positions.push_back(p7[1]);
				xyz_positions.push_back(p7[2]);
				xyz_positions.push_back(p0[0]);
				xyz_positions.push_back(p0[1]);
				xyz_positions.push_back(p0[2]);
				xyz_positions.push_back(p3[0]);
				xyz_positions.push_back(p3[1]);
				xyz_positions.push_back(p3[2]);
				//EFG
				xyz_positions.push_back(p3[0]);
				xyz_positions.push_back(p3[1]);
				xyz_positions.push_back(p3[2]);
				xyz_positions.push_back(p2[0]);
				xyz_positions.push_back(p2[1]);
				xyz_positions.push_back(p2[2]);
				xyz_positions.push_back(p7[0]);
				xyz_positions.push_back(p7[1]);
				xyz_positions.push_back(p7[2]);
				//EHG
				xyz_positions.push_back(p7[0]);
				xyz_positions.push_back(p7[1]);
				xyz_positions.push_back(p7[2]);
				xyz_positions.push_back(p2[0]);
				xyz_positions.push_back(p2[1]);
				xyz_positions.push_back(p2[2]);
				xyz_positions.push_back(p6[0]);
				xyz_positions.push_back(p6[1]);
				xyz_positions.push_back(p6[2]);
				//DCG
				xyz_positions.push_back(p4[0]);
				xyz_positions.push_back(p4[1]);
				xyz_positions.push_back(p4[2]);
				xyz_positions.push_back(p5[0]);
				xyz_positions.push_back(p5[1]);
				xyz_positions.push_back(p5[2]);
				xyz_positions.push_back(p0[0]);
				xyz_positions.push_back(p0[1]);
				xyz_positions.push_back(p0[2]);
				//DHG
				xyz_positions.push_back(p0[0]);
				xyz_positions.push_back(p0[1]);
				xyz_positions.push_back(p0[2]);
				xyz_positions.push_back(p5[0]);
				xyz_positions.push_back(p5[1]);
				xyz_positions.push_back(p5[2]);
				xyz_positions.push_back(p1[0]);
				xyz_positions.push_back(p1[1]);
				xyz_positions.push_back(p1[2]);

			}
			if (maze[i][j] == ' ')
			{

			}
		}
	}
	indicessize = xyz_positions.size() / 3;
	for (int h = 0; h < xyz_positions.size(); h += 3)
	{
		vertices.push_back(xyz_positions[h]);
		vertices.push_back(xyz_positions[h + 2]);
		vertices.push_back(xyz_positions[h + 1]);
		//Alpha Position
		vertices.push_back(1.0f);
		//Color
		vertices.push_back(1.0f);
		vertices.push_back(0.0f);
		vertices.push_back(0.0f);
		//Alpha Color
		vertices.push_back(1.0f);
	}
	return vertices;
}
/*
std::vector<glm::vec3> genTorusNormals(std::vector<float> vertices, std::vector<unsigned int> indices)
{
	
	std::vector<glm::vec3> normals;
	float offset = mazesize * 0.1 / 2;
	for (int i = 0; i < mazesize; i++)
	{
		for (int j = 0; j < mazesize; j++)
		{
			if (maze[i][j] == '#')
			{

				glm::vec3 p0 = {0,0,1};
				glm::vec3 p1 = { (i * 0.1) + 0.1 - offset, (j * 0.1) + 0.0 - offset, 0.0 };
				glm::vec3 p2 = { (i * 0.1) + 0.1 - offset, (j * 0.1) + 0.1 - offset, 0.0 };
				glm::vec3 p3 = { (i * 0.1) + 0.0 - offset, (j * 0.1) + 0.1 - offset, 0.0 };
				glm::vec3 p4 = { (i * 0.1) + 0.0 - offset, (j * 0.1) + 0.0 - offset, 0.1 };
				glm::vec3 p5 = { (i * 0.1) + 0.1 - offset, (j * 0.1) + 0.0 - offset, 0.1 };
				glm::vec3 p6 = { (i * 0.1) + 0.1 - offset, (j * 0.1) + 0.1 - offset, 0.1 };
				glm::vec3 p7 = { (i * 0.1) + 0.0 - offset, (j * 0.1) + 0.1 - offset, 0.1 };

				//ABC
				normals.push_back(p0[0]);
				normals.push_back(p0[1]);
				normals.push_back(p0[2]);
				//ACD
				normals.push_back(p3[0]);
				normals.push_back(p3[1]);
				normals.push_back(p3[2]);
				//AEF
				normals.push_back(p1[0]);
				normals.push_back(p1[1]);
				normals.push_back(p1[2]);
				//ADE
				normals.push_back(p2[0]);
				normals.push_back(p2[1]);
				normals.push_back(p2[2]);
				//BCG
				normals.push_back(p5[0]);
				normals.push_back(p5[1]);
				normals.push_back(p5[2]);
				//BHD
				normals.push_back(p6[0]);
				normals.push_back(p6[1]);
				normals.push_back(p6[2]);
				//BEH
				normals.push_back(p4[0]);
				normals.push_back(p4[1]);
				normals.push_back(p4[2]);
				//BCF
				normals.push_back(p7[0]);
				normals.push_back(p7[1]);
				normals.push_back(p7[2]);
				//EFG
				normals.push_back(p3[0]);
				normals.push_back(p3[1]);
				normals.push_back(p3[2]);
				//EHG
				normals.push_back(p7[0]);
				normals.push_back(p7[1]);
				normals.push_back(p7[2]);
				//DCG
				normals.push_back(p4[0]);
				normals.push_back(p4[1]);
				normals.push_back(p4[2]);
				//DHG
				normals.push_back(p0[0]);
				normals.push_back(p0[1]);
				normals.push_back(p0[2]);

			}
			if (maze[i][j] == ' ')
			{

			}
		}
	}

	return normals;
}
*/
/*
//Position
verticestemp.push_back(x);
verticestemp.push_back(y);
verticestemp.push_back(z);
//Alpha Position
verticestemp.push_back(1.0f);
//Color
verticestemp.push_back(1.0f);
verticestemp.push_back(0.0f);
verticestemp.push_back(0.0f);
//Alpha Color
verticestemp.push_back(1.0f);
*/



/*=================================================================================================
	HELPER FUNCTIONS
=================================================================================================*/

void window_to_scene(int wx, int wy, float& sx, float& sy)
{
	sx = (2.0f * (float)wx / WindowWidth) - 1.0f;
	sy = 1.0f - (2.0f * (float)wy / WindowHeight);
}

/*=================================================================================================
	SHADERS
=================================================================================================*/

void CreateTransformationMatrices(void)
{
	// PROJECTION MATRIX
	PerspProjectionMatrix = glm::perspective<float>(glm::radians(60.0f), (float)WindowWidth / (float)WindowHeight, 0.01f, 1000.0f);

	// VIEW MATRIX
	glm::vec3 eye(0.0, 0.0, 2.0);
	glm::vec3 center(0.0, 0.0, 0.0);
	glm::vec3 up(0.0, 1.0, 0.0);

	PerspViewMatrix = glm::lookAt(eye, center, up);

	// MODEL MATRIX
	PerspModelMatrix = glm::mat4(1.0);
	PerspModelMatrix = glm::rotate(PerspModelMatrix, glm::radians(perspRotationX), glm::vec3(1.0, 0.0, 0.0));
	PerspModelMatrix = glm::rotate(PerspModelMatrix, glm::radians(perspRotationY), glm::vec3(0.0, 1.0, 0.0));
	PerspModelMatrix = glm::scale(PerspModelMatrix, glm::vec3(perspZoom));
}

void CreateShaders(void)
{
	// Renders without any transformations
	PassthroughShader.Create("./shaders/simple.vert", "./shaders/simple.frag");

	// Renders using perspective projection
	PerspectiveShader.Create("./shaders/persp.vert", "./shaders/persp.frag");

	//
	// Additional shaders would be defined here
	//

	PerspectiveShader.Create("./shaders/persplight.vert", "./shaders/persplight.frag");
}

/*=================================================================================================
	BUFFERS
=================================================================================================*/

void CreateAxisBuffers(void)
{
	glGenVertexArrays(1, &axis_VAO); //generate 1 new VAO, its ID is returned in axis_VAO
	glBindVertexArray(axis_VAO); //bind the VAO so the subsequent commands modify it

	glGenBuffers(2, &axis_VBO[0]); //generate 2 buffers for data, their IDs are returned to the axis_VBO array

	// first buffer: vertex coordinates
	glBindBuffer(GL_ARRAY_BUFFER, axis_VBO[0]); //bind the first buffer using its ID
	glBufferData(GL_ARRAY_BUFFER, sizeof(axis_vertices), axis_vertices, GL_STATIC_DRAW); //send coordinate array to the GPU
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0); //let GPU know this is attribute 0, made up of 4 floats
	glEnableVertexAttribArray(0);

	// second buffer: colors
	glBindBuffer(GL_ARRAY_BUFFER, axis_VBO[1]); //bind the second buffer using its ID
	glBufferData(GL_ARRAY_BUFFER, sizeof(axis_colors), axis_colors, GL_STATIC_DRAW); //send color array to the GPU
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0); //let GPU know this is attribute 1, made up of 4 floats
	glEnableVertexAttribArray(1);

	glBindVertexArray(0); //unbind when done

	//NOTE: You will probably not use an array for your own objects, as you will need to be
	//      able to dynamically resize the number of vertices. Remember that the sizeof()
	//      operator will not give an accurate answer on an entire vector. Instead, you will
	//      have to do a calculation such as sizeof(v[0]) * v.size().
}

//
//void CreateMyOwnObject( void ) ...
//
void createTorusBuffers(void)
{
	std::vector<float> vertices = genmazeVertices();


	glGenVertexArrays(1, &torus_VAO);
	glGenBuffers(1, &torus_VBO);
	glGenBuffers(1, &torus_NBO);

	glBindVertexArray(torus_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, torus_VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, torus_EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//glBindBuffer(GL_ARRAY_BUFFER, torus_NBO);
	//glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
	//glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	//glEnableVertexAttribArray(2);

	glBindVertexArray(0);

}

/*=================================================================================================
	CALLBACKS
=================================================================================================*/

//-----------------------------------------------------------------------------
// CALLBACK DOCUMENTATION
// https://www.opengl.org/resources/libraries/glut/spec3/node45.html
// http://freeglut.sourceforge.net/docs/api.php#WindowCallback
//-----------------------------------------------------------------------------

void idle_func()
{
	//uncomment below to repeatedly draw new frames
	glutPostRedisplay();
}

void reshape_func(int width, int height)
{
	WindowWidth = width;
	WindowHeight = height;

	glViewport(0, 0, width, height);
	glutPostRedisplay();
}

void keyboard_func(unsigned char key, int x, int y)
{
	key_states[key] = true;

	switch (key)
	{
	case 'm':
	{
		draw_wireframe = !draw_wireframe;
		if (draw_wireframe == true)
			std::cout << "Wireframes on.\n";
		else
			std::cout << "Wireframes off.\n";
		break;
	}
	case 'q':
		n++;
		break;
	case 'a':
		n--;
		break;
	case 'w':
		r += 0.01f;
		break;
	case 's':
		r -= 0.01f;
		break;
	case 'e':
		R += 0.01f;
		break;
	case 'd':
		R -= 0.01f;
		break;
	case 'z':
		flatshading = true;
		break;
	case 'x':
		flatshading = false;
		break;
	case 'c':
		shownormals = !shownormals;
		break;

		// Exit on escape key press
	case '\x1B':
	{
		exit(EXIT_SUCCESS);
		break;
	}
	}
}

void key_released(unsigned char key, int x, int y)
{
	key_states[key] = false;
}

void key_special_pressed(int key, int x, int y)
{
	key_special_states[key] = true;
}

void key_special_released(int key, int x, int y)
{
	key_special_states[key] = false;
}

void mouse_func(int button, int state, int x, int y)
{
	// Key 0: left button
	// Key 1: middle button
	// Key 2: right button
	// Key 3: scroll up
	// Key 4: scroll down

	if (x < 0 || x > WindowWidth || y < 0 || y > WindowHeight)
		return;

	float px, py;
	window_to_scene(x, y, px, py);

	if (button == 3)
	{
		perspZoom += 0.03f;
	}
	else if (button == 4)
	{
		if (perspZoom - 0.03f > 0.0f)
			perspZoom -= 0.03f;
	}

	mouse_states[button] = (state == GLUT_DOWN);

	LastMousePosX = x;
	LastMousePosY = y;
}

void passive_motion_func(int x, int y)
{
	if (x < 0 || x > WindowWidth || y < 0 || y > WindowHeight)
		return;

	float px, py;
	window_to_scene(x, y, px, py);

	LastMousePosX = x;
	LastMousePosY = y;
}

void active_motion_func(int x, int y)
{
	if (x < 0 || x > WindowWidth || y < 0 || y > WindowHeight)
		return;

	float px, py;
	window_to_scene(x, y, px, py);

	if (mouse_states[0] == true)
	{
		perspRotationY += (x - LastMousePosX) * perspSensitivity;
		perspRotationX += (y - LastMousePosY) * perspSensitivity;
	}

	LastMousePosX = x;
	LastMousePosY = y;
}

/*=================================================================================================
	RENDERING
=================================================================================================*/

void display_func(void)
{
	// Clear the contents of the back buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Update transformation matrices
	CreateTransformationMatrices();

	// Choose which shader to user, and send the transformation matrix information to it
	PerspectiveShader.Use();
	PerspectiveShader.SetUniform("projectionMatrix", glm::value_ptr(PerspProjectionMatrix), 4, GL_FALSE, 1);
	PerspectiveShader.SetUniform("viewMatrix", glm::value_ptr(PerspViewMatrix), 4, GL_FALSE, 1);
	PerspectiveShader.SetUniform("modelMatrix", glm::value_ptr(PerspModelMatrix), 4, GL_FALSE, 1);

	// Drawing in wireframe?
	if (draw_wireframe == true)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Bind the axis Vertex Array Object created earlier, and draw it
	glBindVertexArray(axis_VAO);
	glDrawArrays(GL_LINES, 0, 6); // 6 = number of vertices in the object

	//
	// Bind and draw your object here
	//

	// Create buffer cause i have to change it
	createTorusBuffers();

	glBindVertexArray(torus_VAO);
	glDrawArrays(GL_TRIANGLES, 0, indicessize);



	// Unbind when done
	glBindVertexArray(0);

	// Swap the front and back buffers
	glutSwapBuffers();
}

/*=================================================================================================
	INIT
=================================================================================================*/

void init(void)
{
	// Print some info
	std::cout << "Vendor:         " << glGetString(GL_VENDOR) << "\n";
	std::cout << "Renderer:       " << glGetString(GL_RENDERER) << "\n";
	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << "\n";
	std::cout << "GLSL Version:   " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n\n";

	// Set OpenGL settings
	glClearColor(50.0f, 50.0f, 50.0f, 0.0f); // background color
	glEnable(GL_DEPTH_TEST); // enable depth test
	glEnable(GL_CULL_FACE); // enable back-face culling

	// Create shaders
	CreateShaders();

	// Create axis buffers
	CreateAxisBuffers();

	//
	// Consider calling a function to create your object here
	//

	std::cout << "Finished initializing...\n\n";
}

/*=================================================================================================
	MAIN
=================================================================================================*/

int main(int argc, char** argv)
{
	// Create and initialize the OpenGL context
	glutInit(&argc, argv);

	glutInitWindowPosition(100, 100);
	glutInitWindowSize(InitWindowWidth, InitWindowHeight);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

	glutCreateWindow("CSE-170 Computer Graphics");

	// Initialize GLEW
	GLenum ret = glewInit();
	if (ret != GLEW_OK) {
		std::cerr << "GLEW initialization error." << std::endl;
		glewGetErrorString(ret);
		return -1;
	}

	// Register callback functions
	glutDisplayFunc(display_func);
	glutIdleFunc(idle_func);
	glutReshapeFunc(reshape_func);
	glutKeyboardFunc(keyboard_func);
	glutKeyboardUpFunc(key_released);
	glutSpecialFunc(key_special_pressed);
	glutSpecialUpFunc(key_special_released);
	glutMouseFunc(mouse_func);
	glutMotionFunc(active_motion_func);
	glutPassiveMotionFunc(passive_motion_func);

	// Do program initialization
	init();

	// Enter the main loop
	glutMainLoop();

	return EXIT_SUCCESS;
}
