#define STB_IMAGE_IMPLEMENTATION
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h"

#include "ShaderClass.h"
#include "camera.h"

#include <iostream>

void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xposin, double yposin);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void setMouseCaptured(GLFWwindow* window, bool captured);
unsigned int loadTexture(const char* path);

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

	//Shaders
	Shader cubeShader(RESOURCES_PATH "CubeShader.vert", RESOURCES_PATH "CubeShader.frag");
	Shader lightShader(RESOURCES_PATH "LightShader.vert", RESOURCES_PATH "LightShader.frag");

	//Define the vertices
	float vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,	0.0f, 0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,	0.0f, 0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,	0.0f, 0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,	0.0f, 0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,	0.0f, 0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,	0.0f, 0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,	0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,	0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,	0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,	0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,	0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,	0.0f, 0.0f, 1.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,	-1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,	-1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,	-1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,	-1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,	-1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,	-1.0f, 0.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,	1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,	1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,	1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,	1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,	1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,	1.0f, 0.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,	0.0f, -1.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,	0.0f, -1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,	0.0f, -1.0f, 0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,	0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,	0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,	0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,	0.0f, 1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,	0.0f, 1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,	0.0f, 1.0f, 0.0f,
	};

	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	//setup vertex data and buffer(s), configure vertex attributes
	unsigned int VBO, CubeVAO;
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &CubeVAO);

	//SETUP TRIANGLE
	glBindVertexArray(CubeVAO);  	//bind vertex array objects first

	//Then bind and set vertex buffer object(s)
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Then set the vertex attribute(s)
	//set position attribute (location = 0)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 
		(void*)0);
	glEnableVertexAttribArray(0);

	//set texture coords attribute (location = 1)
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
		(void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//set normals attribute (location = 2)
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
		(void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);

	unsigned int LightVAO;
	glGenVertexArrays(1, &LightVAO);
	glBindVertexArray(LightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//For wireframe polygon
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	unsigned int diffuseMap = loadTexture(RESOURCES_PATH "container2.png");
	unsigned int specularMap = loadTexture(RESOURCES_PATH "container2_specular.png");

	cubeShader.use(); 	//activate shader
	cubeShader.setInt("material.diffuse", 0); //Set texture unit of diffuse map to 0
	cubeShader.setInt("material.specular", 1); //Set texture unit of specular map to 1

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");


	//Dirlight vars
	glm::vec3 dirLightDirection = { -0.2f, -1.0f, -0.3f };
	glm::vec3 dirLightambient = { 0.05f, 0.05f, 0.05f };
	glm::vec3 dirLightdiffuse = { 0.4f, 0.4f, 0.4f };
	glm::vec3 dirLightspecular = { 0.5f, 0.5f, 0.5f };

	//Spot light vars
	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};
	glm::vec3 pointLightAmbients[] = {
		glm::vec3(0.05f, 0.05f, 0.05f),
		glm::vec3(0.05f, 0.05f, 0.05f),
		glm::vec3(0.05f, 0.05f, 0.05f),
		glm::vec3(0.05f, 0.05f, 0.05f)
	};
	glm::vec3 pointLightDiffuses[] = {
		glm::vec3(0.8f, 0.8f, 0.8f),
		glm::vec3(0.8f, 0.8f, 0.8f),
		glm::vec3(0.8f, 0.8f, 0.8f),
		glm::vec3(0.8f, 0.8f, 0.8f)
	};
	glm::vec3 pointLightSpeculars[] = {
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f)
	};
	glm::vec3 pointLightSourceCubeColors[] = {
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f)
	};

	//Spot light vars
	float spotLightCutOffDeg = 12.5f;
	float spotLightOuterCutOffDeg = 15.0f;
	glm::vec3 spotLightAmbient = { 0.0f, 0.0f, 0.0f };
	glm::vec3 spotLightDiffuse = { 1.0f, 1.0f, 1.0f };
	glm::vec3 spotlightSpecular = { 1.0f, 1.0f, 1.0f };

	glm::vec3 clearColor = { 0.0f, 0.0f, 0.0f };

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

		//MAIN CUBE SHADER UNIFORMS
		cubeShader.use();
		cubeShader.setFloat("material.shininess", 32);
		cubeShader.setVec3("viewPos", camera.Position);

		//Dir light uniforms
		cubeShader.setVec3("dirLight.direction", dirLightDirection);
		cubeShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		cubeShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
		cubeShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

		//Point light uniforms
		for (int i = 0; i < 4; i++)
		{
			std::string index = std::to_string(i);

			cubeShader.setVec3("pointLights[" + index + "].position", pointLightPositions[i]);
			cubeShader.setFloat("pointLights[" + index + "].constant", 1.0f);
			cubeShader.setFloat("pointLights[" + index + "].linear", 0.09f);
			cubeShader.setFloat("pointLights[" + index + "].quadratic", 0.032f);
			cubeShader.setVec3("pointLights[" + index + "].ambient", pointLightAmbients[i]);
			cubeShader.setVec3("pointLights[" + index + "].diffuse", pointLightDiffuses[i]);
			cubeShader.setVec3("pointLights[" + index + "].specular", pointLightSpeculars[i]);
		}

		//Spot Light uniforms
		cubeShader.setVec3("spotLight.position", camera.Position);
		cubeShader.setVec3("spotLight.direction", camera.Front);
		cubeShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(spotLightCutOffDeg)));
		cubeShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(spotLightOuterCutOffDeg)));
		cubeShader.setFloat("spotLight.constant", 1.0f);
		cubeShader.setFloat("spotLight.linear", 0.09f);
		cubeShader.setFloat("spotLight.quadratic", 0.032f);
		cubeShader.setVec3("spotLight.ambient", spotLightAmbient);
		cubeShader.setVec3("spotLight.diffuse", spotLightDiffuse);
		cubeShader.setVec3("spotLight.specular", spotlightSpecular);
		
		//bind texture(s)
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);	
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);

		//projection transfromation
		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT,
			0.1f, 100.0f);

		cubeShader.setMat4("projection", projection);

		//view transformation
		glm::mat4 view = camera.GetViewMatrix();
		cubeShader.setMat4("view", view);

		glBindVertexArray(CubeVAO);

		//world space / model transformation
		for (unsigned int i = 0; i < 10; i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			cubeShader.setMat4("model", model);

			//draw cube
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		//Light source cube transformations
		lightShader.use();
		lightShader.setMat4("projection", projection);
		lightShader.setMat4("view", view);

		glBindVertexArray(LightVAO);

		for (unsigned int i = 0; i < 4; i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.2f));
			lightShader.setMat4("model", model);
			lightShader.setVec3("lightSourceCubeColor", pointLightSourceCubeColors[i]);

			//draw light source
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		ImGui::Begin("Tools");

		ImGui::ColorEdit3("clear color", &clearColor[0]);

		if (ImGui::CollapsingHeader("Directional Light"))
		{
			ImGui::DragFloat3("direction", &dirLightDirection[0], 0.1f);
			ImGui::ColorEdit3("ambient", &dirLightambient[0]);
			ImGui::ColorEdit3("diffuse", &dirLightdiffuse[0]);
			ImGui::ColorEdit3("specualar", &dirLightspecular[0]);
		}
		for (int i = 0; i < 4; i++)
		{
			std::string name = "Point Light" + std::to_string(i);
			if (ImGui::CollapsingHeader(name.c_str()))
			{
				std::string cubeColorId = "cube color##" + std::to_string(i);
				std::string positionId = "position##" + std::to_string(i);
				std::string ambientId = "ambient##" + std::to_string(i);
				std::string diffuseId = "diffuse##" + std::to_string(i);
				std::string specularId = "specular##" + std::to_string(i);

				ImGui::ColorEdit3(cubeColorId.c_str(), &pointLightSourceCubeColors[i][0]);
				ImGui::DragFloat3(positionId.c_str(), &pointLightPositions[i][0], 0.1f);
				ImGui::ColorEdit3(ambientId.c_str(), &pointLightAmbients[i][0]);
				ImGui::ColorEdit3(diffuseId.c_str(), &pointLightDiffuses[i][0]);
				ImGui::ColorEdit3(specularId.c_str(), &pointLightSpeculars[i][0]);
			}
		}
		if (ImGui::CollapsingHeader("Spot Light"))
		{
			ImGui::DragFloat("inner radius", &spotLightCutOffDeg);
			ImGui::DragFloat("outer radius", &spotLightOuterCutOffDeg);
			ImGui::ColorEdit3("ambient", &spotLightAmbient[0]);
			ImGui::ColorEdit3("diffuse", &spotLightDiffuse[0]);
			ImGui::ColorEdit3("specualar", &spotlightSpecular[0]);
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

	glDeleteVertexArrays(1, &CubeVAO);
	glDeleteVertexArrays(1, &LightVAO);
	glDeleteBuffers(1, &VBO);

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

unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);

	if (data)
	{
		GLenum format;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << "\n";
	}

	stbi_image_free(data);
	return textureID;
}