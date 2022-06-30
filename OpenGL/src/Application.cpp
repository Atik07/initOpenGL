#include <GLEW/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "ErrorHandling.h"

#include "Renderer.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	/* setting OpenGL profile to be CORE*/
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 640, "quack", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK)
		std::cout << "glewInit error!" << std::endl;

	std::cout << glGetString(GL_VERSION) << std::endl;

	{

		float positions[] = {
			-0.5f, -0.5f, 0.0f, 0.0f, //0
			-0.5f,  0.5f, 0.0f, 1.0f, //1
			 0.5f,  0.5f, 1.0f, 1.0f, //2
			 0.5f, -0.5f, 1.0f, 0.0f  //3
		};

		// indices need to be unsigned
		unsigned int indices[] = {
			0 , 1 , 2,
			2 , 3 , 0
		};

		// handling blending
		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		// VERTEX ARRAY OBJECT
		VertexArray va;

		// creating vertex buffer using the VertexBuffer class
		VertexBuffer vb(positions, 4 * 4 * sizeof(float));

		// vertex buffer layout .. to interpret the data in vertex buffer
		VertexBufferLayout layout;
		layout.Push<float>(2); // 2 is the count of floats that are considered 1 vertex
		layout.Push<float>(2); // this is for the texture coordinates
		// adding and binding vb to va
		va.AddBuffer(vb, layout);
		
		// creating index buffer using IndexBuffer class
		IndexBuffer ib(indices, 6);

		/* MVP matrices */
		glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);

		// create identity matrix and translate it .. or rotate or scale
		glm::mat4 view = glm::translate(glm::mat4 (1.0f),glm::vec3(-0.3f,0,0));

		glm::mat4 model = glm::translate(glm::mat4 (1.0f), glm::vec3(0.1f,0.6f,0));

		glm::mat4 mvp = projection * view * model; // multiplied in this order since OpenGL is column major

		// the actual shader in string format -- written in GLSL (OpenGL Shading Language)
		const std::string filepath = "resources/shaders/basic.shader";

		// Load Shader
		Shader shader(filepath);
		shader.Bind();
		shader.SetUniformMat4f("u_MVP",mvp);

		// Texture
		Texture texture("resources/textures/duck.png");
		texture.Bind();
		shader.SetUniform1i("u_Texture",0);

		/* ----- HERE ------- clearing all GL states */
		va.Unbind();
		vb.Unbind();
		ib.Unbind();
		shader.Unbind();
		/*-------------------------------------------*/

		Renderer renderer;

		// Setup ImGui binding
		ImGui::CreateContext();
		// setup imgui controls
		// ImGuiIO& io = ImGui::GetIO(); (void)io;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
		ImGui_ImplGlfwGL3_Init(window, true);
		// Setup style -- DARK MODE :)
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		glm::vec3 translation = glm::vec3(0.1f, 0.6f, 0);

		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			/* Render here */
			renderer.Clear();

			ImGui_ImplGlfwGL3_NewFrame();

			glm::mat4 model = glm::translate(glm::mat4(1.0f), translation);
			glm::mat4 mvp = projection * view * model;

			shader.Bind();
			shader.SetUniformMat4f("u_MVP", mvp);

			renderer.Draw(va, ib, shader);

			{
				ImGui::SliderFloat3("Model Translation", &translation.x, 0.0f, 1.0f);
				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			}


			ImGui::Render();
			ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

			/* Swap front and back buffers */
			GLCall(glfwSwapBuffers(window));

			/* Poll for and process events */
			GLCall(glfwPollEvents());
		}
	}
	// Cleanup
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
	return 0;
}