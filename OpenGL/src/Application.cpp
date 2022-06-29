#include <GLEW/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"

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
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
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
			-0.5f, -0.5f, //0
			-0.5f,  0.5f, //1
			 0.5f,  0.5f, //2
			 0.5f, -0.5f  //3
		};

		// indices need to be unsigned
		unsigned int indices[] = {
			0 , 1 , 2,
			2 , 3 , 0
		};

		// VERTEX ARRAY OBJECT
		VertexArray va;

		// creating vertex buffer using the VertexBuffer class
		VertexBuffer vb(positions, 4 * 2 * sizeof(float));

		// vertex buffer layout .. to interpret the data in vertex buffer
		VertexBufferLayout layout;
		layout.Push<float>(2); // 2 is the count of floats that are considered 1 vertex

		// adding and binding vb to va
		va.AddBuffer(vb, layout);
		
		// creating index buffer using IndexBuffer class
		IndexBuffer ib(indices, 6);

		// the actual shader in string format -- written in GLSL (OpenGL Shading Language)
		const std::string filepath = "resources/shaders/basic.shader";

		// Load Shader
		Shader shader(filepath);

		float rChannel = 0.5;
		float delta = 0.05;

		/* ----- HERE ------- clearing all GL states */
		va.Unbind();
		vb.Unbind();
		ib.Unbind();
		shader.Unbind();
		/*-------------------------------------------*/


		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			/* Render here */
			GLCall(glClear(GL_COLOR_BUFFER_BIT));

			// binding shader
			shader.Bind();	

			// set uniforms
			shader.SetUnfiorm4f("u_Color", rChannel, 0.0, 0.5, 1.0);

			// binding VAO
			va.Bind(); 

			// binding index buffer
			ib.Bind();

			// draw call
			GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));


			if (rChannel < 0)
				delta = 0.05;
			else if (rChannel > 1)
				delta = -0.05;

			rChannel += delta;

			/* Swap front and back buffers */
			GLCall(glfwSwapBuffers(window));

			/* Poll for and process events */
			GLCall(glfwPollEvents());
		}
	}
	glfwTerminate();
	return 0;
}