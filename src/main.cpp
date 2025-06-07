#define STB_IMAGE_IMPLEMENTATION
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ShaderClass.h"
#include "camera.h"
#include "model.h"

#include <iostream>

void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xposin, double yposin);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void setMouseCaptured(GLFWwindow* window, bool captured);

//settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool first_mouse = true;
bool mouseCaptured = true;
bool spacePressedLastFrame = false;

float lastx = 400;
float lasty = 300;

glm::vec3 lightPos(1.2f, 0.1f, 1.8f);

int main()
{
	//GLFW initialization options
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Create GLFW window
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Learn GL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); //setting glViewport for Resizing window
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);


	//Initializing GLAD
	if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	stbi_set_flip_vertically_on_load(true);

	Shader ourShader(RESOURCES_PATH"modelLoading.vert", RESOURCES_PATH"modelLoading.frag");

	Model ourModel(RESOURCES_PATH"objects/backpack/backpack.obj");

	// draw in wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//Render Loop
	while (!glfwWindowShouldClose(window))
	{
		//set delta Time
		float currentframe = glfwGetTime();
		deltaTime = currentframe - lastFrame;
		lastFrame = currentframe;

		//Input
		processInput(window);

		//set backround color
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		ourShader.use();

		//projection transfromation
		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT,
			0.1f, 100.0f);
		ourShader.setMat4("projection", projection);

		//view transformation
		glm::mat4 view = camera.GetViewMatrix();
		ourShader.setMat4("view", view);

		//model transformation
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		ourShader.setMat4("model", model);
		ourModel.Draw(ourShader); //render the loaded model

		//Check and call events and swap the buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window) 
{
	if (mouseCaptured)
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			camera.ProcessKeyboard(FORWARD, deltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			camera.ProcessKeyboard(BACKWARD, deltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			camera.ProcessKeyboard(LEFT, deltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			camera.ProcessKeyboard(RIGHT, deltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		{
			camera.ProcessKeyboard(UP, deltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		{
			camera.ProcessKeyboard(DOWN, deltaTime);
		}
	}
	
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !spacePressedLastFrame)
	{
		setMouseCaptured(window, !mouseCaptured);
	}
	spacePressedLastFrame = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}

void setMouseCaptured(GLFWwindow* window, bool captured)
{
	if (captured)
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	else
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	mouseCaptured = captured;
}

void mouse_callback(GLFWwindow* window, double xPosin, double yPosin)
{
	float xPos = static_cast<float>(xPosin);
	float yPos = static_cast<float>(yPosin);

	if (first_mouse)
	{
		lastx = xPos;
		lasty = yPos;
		std::cout << "ye\n";
		first_mouse = false;
	}

	float xoffset = xPos - lastx;
	float yoffset = lasty - yPos;
	lastx = xPos;
	lasty = yPos;
	
	if (mouseCaptured)
		camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (mouseCaptured)
		camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
