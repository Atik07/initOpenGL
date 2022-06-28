#include <GLEW/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#define ASSERT(x) if (!(x)) __debugbreak(); //when
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x,__FILE__,__LINE__))

static void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line)
{
	while (GLenum error = glGetError())
	{
		std::cout << "[OpenGL error]: " << error << " "
			<< function << " " << file << ":" << line << std::endl;
		return false;
	}
	return true;
}

struct ShaderProgramSource {
	std::string vertexSource;
	std::string fragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filepath) {
	std::ifstream stream(filepath);
	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;

	while (getline(stream, line)) {
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos) {
				//set mode to vertex
				type = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos) {
				//set mode to fragment
				type = ShaderType::FRAGMENT;
			}
		}
		else
		{
			ss[(int)type] << line << "\n";
		}
	}
	return { ss[0].str(), ss[1].str() };
}


static std::string ParseFile(const std::string& filepath) {
	std::ifstream stream(filepath);
	std::string line;
	std::stringstream ss;
	while (getline(stream, line)) {
		ss << line << '\n';
	};
	return ss.str();
}

static unsigned int CompileShader(unsigned int type, const std::string& source) 
{
	// ~TODO:~ Error Handling

	GLCall(unsigned int shader_id = glCreateShader(type));
	const char* src = source.c_str(); //TODO: handle if source is not provided 

	GLCall(glShaderSource(shader_id, 1, &src, NULL));
	GLCall(glCompileShader(shader_id));

	//error handling in shader compilation
	int result;
	GLCall(glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result));

	if (result == GL_FALSE) {
		int length;
		GLCall(glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &length));
		char* err_message = (char *)_malloca(length*sizeof(char));
		GLCall(glGetShaderInfoLog(shader_id, length, &length, err_message));
		std::cout<<"Failed to compile "<<type<<" shader!"<< std::endl;
		std::cout << err_message << std::endl;
		GLCall(glDeleteShader(shader_id));
		return 0;
	}

	return shader_id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) 
{
	GLCall(unsigned int program = glCreateProgram());
	GLCall(unsigned int vshader = CompileShader(GL_VERTEX_SHADER, vertexShader));
	GLCall(unsigned int fshader = CompileShader(GL_FRAGMENT_SHADER, fragmentShader));
	
	GLCall(glAttachShader(program, vshader));
	GLCall(glAttachShader(program, fshader));
	GLCall(glLinkProgram(program));
	GLCall(glValidateProgram(program));

	// once the program is linked the intermediates can be deleted ... this deletes the .obj files
	GLCall(glDeleteShader(vshader));
	GLCall(glDeleteShader(fshader));

	return program;
}

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
	unsigned int vao;
	GLCall(glGenVertexArrays(1,&vao));
	GLCall(glBindVertexArray(vao));

	unsigned int buffer;
	// generates VERTEX BUFFER 
	GLCall(glGenBuffers(1, &buffer));
	// selecting the BUFFER
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
	// declare size of buffer and fill buffer with data
	GLCall(glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), positions, GL_STATIC_DRAW));

	// parsing the buffer data into vertex attributes 
	// WE NEED TO CALL These the no of attributes present in a vertex
	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*2, (const void *)0));

	//binding the index buffer
	unsigned int ibo;
	GLCall(glGenBuffers(1, &ibo));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW));

	// the actual shader in string format -- written in GLSL (OpenGL Shading Language)
	const std::string filepath = "resources/shaders/basic.shader";
	GLCall(ShaderProgramSource shaderSource = ParseShader(filepath));

	//const std::string vertexShaderFile = "resources/shaders/vertex.shader";
	//const std::string fragmentShaderFile = "resources/shaders/fragment.shader";

	//const std::string vertexSource = ParseFile(vertexShaderFile);
	//const std::string fragmentSource = ParseFile(fragmentShaderFile);

	// shader
	GLCall(unsigned int shader = CreateShader(shaderSource.vertexSource, shaderSource.fragmentSource));
	GLCall(glUseProgram(shader));

	//UNIFORMS
	GLCall(int location = glGetUniformLocation(shader,"u_Color"));
	ASSERT(location != -1);

	float rChannel = 0.5;
	float delta = 0.05;

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		GLCall(glClear(GL_COLOR_BUFFER_BIT));

		/* using legacy OpenGL*/
		/*
			glBegin(GL_TRIANGLES);
			glVertex2d(-0.5f, -0.5f);
			glVertex2d(0, 0.5f);
			glVertex2d(0.5f, -0.5f);
			glEnd();
		*/

		/* using Modern OpenGL */

		if (rChannel < 0)
			delta = 0.05;
		else if (rChannel > 1)
			delta = -0.05;

		rChannel += delta;

		GLCall(glUniform4f(location, rChannel, 0.0,0.5,1.0));


		//this is the draw call
		GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0)); 

		/* Swap front and back buffers */
		GLCall(glfwSwapBuffers(window));

		/* Poll for and process events */
		GLCall(glfwPollEvents());
	}

	GLCall(glDeleteProgram(shader));

	glfwTerminate();
	return 0;
}