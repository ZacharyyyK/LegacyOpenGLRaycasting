#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <math.h>
#define PI 3.1415926535
#define DG 0.01745329


using std::cout, std::endl, std::vector;

int width = 1024;
int height = 512;




class Player{
public:

	//Player positons
	float px;
	float py;

	//Player angle
	float pa;

	float lineLength;
	float lineX;
	float lineY;




	int map[64] = {
		1,1,1,1,1,1,1,1,
		1,0,1,0,0,0,0,1,
		1,0,1,0,0,1,0,1,
		1,0,1,0,0,0,0,1,
		1,0,0,0,0,0,0,1,
		1,0,0,0,0,1,1,1,
		1,0,0,0,0,0,0,1,
		1,1,1,1,1,1,1,1,
	};

	int numOfCubesX = 8;
	int numOfCubesY = 8;
	int lengthOfCube = 64;

	

	float calcDist(float x1, float y1, float x2, float y2) {

		float xComp = pow(x2 - x1, 2);
		float yComp = pow(y2 - y1, 2);

		float D = sqrtf(xComp + yComp);
		return D;
	}



	Player():px(300.0f),py(300.0f),pa(PI / 2.0f),lineLength(5.0f), lineX(0.0f), lineY(0.0f){
		
	}
	
	void drawPlayer() {
		
		glColor3f(1.0f, 1.0f, 0.0f);
		glPointSize(8);
		glBegin(GL_POINTS);
		glVertex2f(px, py);
		glEnd();



		lineX = lineLength * cos(pa) * 3;
		lineY = lineLength * sin(pa) * 3;

		float px2 = px + lineX;
		float py2 = py + lineY;


		glLineWidth(3);
		glBegin(GL_LINES);
		glVertex2f(px, py);
		glVertex2f(px2, py2);
		glEnd();
	}

	void drawMap() {
		
		int x1 = 1;
		int x2 = 63;

		int y1 = 1;
		int y2 = 63;


		for (int y = 0; y < numOfCubesY; y++) {

			x1 = 1;
			x2 = 63;

			for (int x = 0; x < numOfCubesX; x++) {
				
				if (map[y * numOfCubesX + x] == 1) {
					glColor3f(1.0f, 1.0f, 1.0f);
				}
				else {
					glColor3f(0.0f, 0.0f, 0.0f);
				}

				glBegin(GL_QUADS);
				glVertex2f(x1, y1);
				glVertex2f(x2, y1);
				glVertex2f(x2, y2);
				glVertex2f(x1, y2);
				glEnd();

				x1 += 64;
				x2 += 64;

			}

			y1 += 64;
			y2 += 64;

		}


	}


	bool isWall(float x, float y) {
		int mapX = static_cast<int>(x / lengthOfCube);
		int mapY = static_cast<int>(y / lengthOfCube);

		if (mapX < 0 || mapX >= numOfCubesX || mapY < 0 || mapY >= numOfCubesX) { //Treats out of bounds as walls essentially
			return true;
		}

		return(map[mapY * numOfCubesX + mapX]);

	}

	void castRays() {
		float rayAngle = pa - (PI / 6.0f); //Shifts the starting ray angle back 30 degrees

		for (unsigned int i = 0; i < 60; i++) {
			
			//Ray start position equals the player position
			float rayX = px;
			float rayY = py;

			while (!isWall(rayX, rayY)) {
				rayX += cos(rayAngle) * 2;
				rayY += sin(rayAngle) * 2;
			}

			float dist = calcDist(px, py, rayX, rayY);

			glBegin(GL_LINES);
			glVertex2f(px, py);
			glVertex2f(rayX, rayY);
			glEnd();

			float correctAngle = pa - rayAngle;
			if (correctAngle < 0) correctAngle += 2 * PI;
			if (correctAngle > 2 * PI) correctAngle -= 2 * PI;

			dist = dist * cos(correctAngle);

			float lineHeight = (lengthOfCube * 320) / dist;
			
			float lineOffset = 160 - lineHeight / 2;




			glLineWidth(8);
			glBegin(GL_LINES);
			glVertex2i(i * 8 + 530, lineOffset);
			glVertex2i(i * 8 + 530, lineHeight + lineOffset);
			glEnd();
			

			rayAngle += DG / 1.5f;
			if (rayAngle < 0) rayAngle += 2 * PI;
			if (rayAngle > 2 * PI) rayAngle -= 2 * PI;

		}


	}



};


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window, Player& player) {
	
	player.lineX = player.lineLength * cos(player.pa);
	player.lineY = player.lineLength * sin(player.pa);
	
	glColor3f(1.0f, 1.0f, 0.0f);
	
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		player.pa -= 0.01f;
		if (player.pa < 0) player.pa = 2 * PI;
		//cout << player.pa << endl;
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		
		player.pa += 0.01f;
		if (player.pa >= 2 * PI) player.pa = 0;
		//cout << player.pa << endl;
	}
	
	if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
		player.px += player.lineX * 0.1f;
		player.py += player.lineY * 0.1f;
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		player.px -= player.lineX * 0.1f;
		player.py -= player.lineY * 0.1f;
	}

}







int main() {
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(width,height, "Ray-Casting", NULL, NULL);

	if (window == nullptr) {
		cout << "Failed to create window..." << endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "Failed to initialize glad..." << endl;
		return -1;
	}

	
	glOrtho(0, width, height, 0, -1.0, 1.0);

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	

	Player player;

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);
		
		player.drawMap();
		player.drawPlayer();
		player.castRays();

		processInput(window, player);


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}