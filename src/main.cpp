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

#pragma region Setting up point light source cube
	float vertices[] = {
		// positions          
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,

		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,

		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,

		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
	};

	unsigned int lightCubeVBO, lightCubeVAO;
	glGenVertexArrays(1, &lightCubeVAO);
	glGenBuffers(1, &lightCubeVBO);

	glBindBuffer(GL_ARRAY_BUFFER, lightCubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(lightCubeVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
#pragma endregion

	stbi_set_flip_vertically_on_load(true);

	Shader ourShader(RESOURCES_PATH"modelLoading.vert", RESOURCES_PATH"modelLoading.frag");
	Shader lightCubeShader(RESOURCES_PATH"LightCubeShader.vert", RESOURCES_PATH"LightCubeShader.frag");

	Model ourModel(RESOURCES_PATH"objects/backpack/backpack.obj");

	// draw in wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//IMGUI SETUP
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

#pragma region Lighting variables

	glm::vec3 clearColor = { 0.0f, 0.0f, 0.0f };

	//Dirlight vars
	bool dirLightActive = true;
	glm::vec3 dirLightDirection = { -0.2f, -1.0f, -0.3f };
	glm::vec3 dirLightambient = { 0.1f, 0.1f, 0.1f };
	glm::vec3 dirLightdiffuse = { 0.4f, 0.4f, 0.4f };
	glm::vec3 dirLightspecular = { 0.5f, 0.5f, 0.5f };

	//PointLight vars
	int NR_POINT_LIGHTS = 2;
	bool pointLightsActive[] = {
		true,
		true
	};
	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f)
	};
	glm::vec3 pointLightAmbients[] = {
		glm::vec3(0.1f, 0.1f, 0.1f),
		glm::vec3(0.1f, 0.1f, 0.1f)
	};
	glm::vec3 pointLightDiffuses[] = {
		glm::vec3(0.8f, 0.8f, 0.8f),
		glm::vec3(0.8f, 0.8f, 0.8f)
	};
	glm::vec3 pointLightSpeculars[] = {
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f)
	};
	glm::vec3 pointLightSourceCubeColors[] = {
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f)
	};
#pragma endregion


	
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
		glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		
		ourShader.use();

		ourShader.setVec3("viewPos", camera.Position);

		ourShader.setVec3("dirLight.direction", dirLightDirection);
		if (dirLightActive)
		{
			ourShader.setVec3("dirLight.ambient", dirLightambient);
			ourShader.setVec3("dirLight.diffuse", dirLightdiffuse);
			ourShader.setVec3("dirLight.specular", dirLightspecular);
		}
		else
		{
			ourShader.setVec3("dirLight.ambient", 0.0f, 0.0f, 0.0f);
			ourShader.setVec3("dirLight.diffuse", 0.0f, 0.0f, 0.0f);
			ourShader.setVec3("dirLight.specular", 0.0f, 0.0f, 0.0f);
		}


		for (int i = 0; i < NR_POINT_LIGHTS; i++)
		{
			std::string index = std::to_string(i);

			ourShader.setVec3("pointLights[" + index + "].position", pointLightPositions[i]);
			ourShader.setFloat("pointLights[" + index + "].constant", 1.0f);
			ourShader.setFloat("pointLights[" + index + "].linear", 0.09f);
			ourShader.setFloat("pointLights[" + index + "].quadratic", 0.032f);
			if (pointLightsActive[i])
			{
				ourShader.setVec3("pointLights[" + index + "].ambient", pointLightAmbients[i]);
				ourShader.setVec3("pointLights[" + index + "].diffuse", pointLightDiffuses[i]);
				ourShader.setVec3("pointLights[" + index + "].specular", pointLightSpeculars[i]);
			}
			else
			{
				ourShader.setVec3("pointLights[" + index + "].ambient", 0.0f, 0.0f, 0.0f);
				ourShader.setVec3("pointLights[" + index + "].diffuse", 0.0f, 0.0f, 0.0f);
				ourShader.setVec3("pointLights[" + index + "].specular", 0.0f, 0.0f, 0.0f);
			}
		}

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

		//set shader values for light source cube
		lightCubeShader.use();
		lightCubeShader.setMat4("projection", projection);
		lightCubeShader.setMat4("view", view);
		
		glBindVertexArray(lightCubeVAO);
		for (unsigned int i = 0; i < NR_POINT_LIGHTS; i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.2f));
			lightCubeShader.setMat4("model", model);
			if (pointLightsActive[i])
				lightCubeShader.setVec3("lightSourceCubeColor", pointLightSourceCubeColors[i]);
			else
				lightCubeShader.setVec3("lightSourceCubeColor", clearColor);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		//ImGui
		ImGui::Begin("Tools");

		ImGui::ColorEdit3("clear color", &clearColor[0]);

		if (ImGui::CollapsingHeader("Directional Light"))
		{
			ImGui::Checkbox("active", &dirLightActive);
			ImGui::DragFloat3("direction", &dirLightDirection[0], 0.1f);
			ImGui::ColorEdit3("ambient", &dirLightambient[0]);
			ImGui::ColorEdit3("diffuse", &dirLightdiffuse[0]);
			ImGui::ColorEdit3("specualar", &dirLightspecular[0]);
		}
		for (int i = 0; i < NR_POINT_LIGHTS; i++)
		{
			std::string name = "Point Light" + std::to_string(i);
			if (ImGui::CollapsingHeader(name.c_str()))
			{
				std::string activeId = "active##" + std::to_string(i);
				std::string cubeColorId = "cube color##" + std::to_string(i);
				std::string positionId = "position##" + std::to_string(i);
				std::string ambientId = "ambient##" + std::to_string(i);
				std::string diffuseId = "diffuse##" + std::to_string(i);
				std::string specularId = "specular##" + std::to_string(i);

				ImGui::Checkbox(activeId.c_str(), &pointLightsActive[i]);
				ImGui::ColorEdit3(cubeColorId.c_str(), &pointLightSourceCubeColors[i][0]);
				ImGui::DragFloat3(positionId.c_str(), &pointLightPositions[i][0], 0.1f);
				ImGui::ColorEdit3(ambientId.c_str(), &pointLightAmbients[i][0]);
				ImGui::ColorEdit3(diffuseId.c_str(), &pointLightDiffuses[i][0]);
				ImGui::ColorEdit3(specularId.c_str(), &pointLightSpeculars[i][0]);
			}
		}

		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		//Check and call events and swap the buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

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
