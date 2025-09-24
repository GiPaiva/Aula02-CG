#include "Window.h"

Window::Window() {}

Window::Window(GLint windowWidth, GLint windowHeight) {
	width = windowWidth;
	height = windowHeight;
}

int Window::initialize() {
	//INICIALIZAR O GLFW
	if (!glfwInit()) {
		printf("GLFW Nao foi inicializado");
		glfwTerminate();
		return 1;
	};

	//GLFW OpenGL Version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	//Core Profile
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//Forward Functions
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	mainWindow = glfwCreateWindow(width, height, "Nova Janela", NULL, NULL);
	if (!mainWindow) {
		printf("GLWF nao criou a janela");
		glfwTerminate();
		return 1;
	}

	//Pegar o buffer size da largura e altura
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	//Configurando a janela principal
	glfwMakeContextCurrent(mainWindow);

	//GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		printf("Glew nao foi iniciado");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	};

	glEnable(GL_DEPTH_TEST);

	//Configurando viewport
	glViewport(0, 0, bufferWidth, bufferHeight);
}

void Window::terminate() {
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
}

Window::~Window() {
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
}
