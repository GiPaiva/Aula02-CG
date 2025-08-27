#include <iostream>
#include <GL/glew.h>
#include <GLFW//glfw3.h>

using namespace std;

const GLint WIDTH = 800, HEIGHT = 600;
// VAO � o ponteiro do programa que estou rodando, espa�o de mem�ria que representa aquele objeto
// VBO � o ponteiro para os meus dados brutos, s�o os dados do objeto a ser representado pelo VAO 
// shaderProgram � qual programa estou rodando 
// todo programa pode ser chamado de shader
GLuint VAO, VBO, shaderProgram;

// aqui estamos fazendo um programa (shader) em GLSL

// shader para renderizar pontos na tela
static const char* vertexShader = "                                                        \n\
#version 330                                                                               \n\
                                                                                           \n\
                                                                                           \n\
// passando um argumento para o inicio do programa (args do C//                            \n\
// estou passando um argumento de entrada na primeira posi��o                              \n\
// esse argumento deve ser um vetor de duas posi��es                                       \n\
layout(location=0) in vec2 pos;                                                            \n\
                                                                                           \n\
void main() {                                                                              \n\
	gl_Position = vec4(pos.x, pos.y, 0.0, 1.0);                                            \n\
}                                                                                          \n\
";

// fragment pode ser lido como "a partir desse ponto o que fa�o com ele?"
// shader para atribuir cores aos pontos

static const char* fragmentShader = "                                                      \n\
#version 330                                                                               \n\
                                                                                           \n\
// diferente da entrada por layout, uniform � uma entrada em tempo de execu��o             \n\
uniform vec3 triColor;																       \n\
// sa�da do fragment shader, a cor final do pixel na tela                                  \n\
out vec4 color;                                                                            \n\
                                                                                           \n\
void main() {                                                                              \n\
	color = vec4(triColor, 1.0);                                                           \n\
}                                                                                          \n\
";

void create_triangle() {
	GLfloat vertices[] = { // nosso buffer de vert�ces
		 0.0f ,  1.0f, // vertice 1 
		-1.0f , -1.0f, // vertice 2
		 1.0f , -1.0f  // vertice 3
	};

	// iniciar um VAO
	// vamos abrir um espa�o de mem�ria na placa de v�deo
	// pegamos o endere�o virutal da placa de video e guardamos ele na vari�vel l� em cima
	glGenVertexArrays(1, &VAO);

	// estamos dizendo que a partir desse momento todas as altera��es ser�o feitas nesse espa�o de mem�ria
	// agora tudo o que vamos criar ser� nesse espa�o de mem�ria como os pontos da nossa tela
	glBindVertexArray(VAO);

		// iniciar um VBO
		glGenBuffers(1, &VBO); // alocar um buffer
		// agora tudo o que vamos criar ser� nesse buffer
		glBindBuffer(GL_ARRAY_BUFFER, VBO); // a partir desse momento estamos modificando esse buffer
			// agora vamos colocar os dados dentro desse buffer
			// GL_array_buffer � o tipo do buffer, 
			// sizeof � o tamanho do buffer, vertices � o ponteiro para os dados que queremos colocar no buffer, 
			// GL_STATIC_DRAW � a forma como vamos usar esse buffer (static porque n�o vamos mudar esses dados)
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); 
			// agora vamos explicar como esses dados est�o organizados
			// 0 � o layout(location=0) que criamos l� em cima no vertex shader, 2 � porque s�o 2 posi��es por v�rtice (x e y), 
			// gl_FLOAT � o tipo de dado, GL_FALSE � se os dados est�o normalizados ou n�o (n�o est�o), 
			// 0 � o espa�amento entre os dados (0 porque s�o dados cont�nuos), 0 � o offset (0 porque come�am do in�cio)
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0); // explicando como interpretar esses dados
			// habilitar o atributo que criamos l� em cima
			glEnableVertexAttribArray(0);  // location

		// deixar de apontar para o VAO e VBO
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

// fun��o para executar um buffer na tela
void add_shader(GLuint program, const char* shaderCode, GLenum type) {
	GLuint _shader = glCreateShader(type);

	// converte char para glchar
	const GLchar* code[1];
	// coloca o c�digo do shader na primeira posi��o do array
	code[0] = shaderCode;


	glShaderSource(_shader, 1, code, NULL);
	glCompileShader(_shader);

	// tratar os erros

	// pegando o status da compila��o
	glAttachShader(program, _shader);
}

// fun��o para criar o programa (shader)
void add_program() {
	shaderProgram = glCreateProgram();
	if (!shaderProgram) {
		cout << "Erro: falha ao criar o programa!";
	}

	// adicionando os shaders ao programa
	add_shader(shaderProgram, vertexShader, GL_VERTEX_SHADER);
	add_shader(shaderProgram, fragmentShader, GL_FRAGMENT_SHADER);

	// linkando o programa, a execu��o
	glLinkProgram(shaderProgram);
}





int main() {
	// inicando GLFW
	if (!glfwInit()) {
		cout << "Erro: GLFW n�o foi iniciado";
		glfwTerminate();
		return -1;
	}

	// set nas coisas obrigat�rias para subir uma janela
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // vers�o m�xima permitida 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // vers�o m�nima
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // somente as fun��es core
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // qual a precis�o de ponto flutuante que vamos usar, precis�o da placa, deixando compativel com vers�es futuras e anteriores

	// cria��o de janela 
	// NULL, NULL s�o para monitor, se usa o monitor principal e share, que n�o vamos usar precisar usar agora
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Window", NULL, NULL);
	if (!window) {
		cout << "Erro: N�o foi poss�vel criar a janela";
		glfwTerminate();
		return -1;
	}

	// guardando o tamanho real da minha tela 
	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(window, &bufferWidth, &bufferHeight);

	glfwMakeContextCurrent(window); // tornando essa janela como principal

	//iniciando o Glew
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		cout << "Erro: n�o foi poss�vel iniciar o glew";
		glfwDestroyWindow(window);
		glfwTerminate();
		return -1;
	}

	// 0, 0 � onde a janela ser� renderizada, ou seja apartir do centro da tela, passando a largura e altura da tela
	glViewport(0, 0, bufferWidth, bufferHeight);

	create_triangle(); // criando o tri�ngulo
	add_program(); // criando o programa
	
	while (!glfwWindowShouldClose(window)) {
		// cor de fundo
		glClearColor(0.29f, 0.0f, 0.51f, 1.0f); // RGBA indigo
		// glfwPollEvents() pega os eventos do teclado e mouse
		glfwPollEvents();
		// limpar a tela com a cor de fundo
		glClear(GL_COLOR_BUFFER_BIT);


		//Alterando a cor do tri�ngulo
		// Guardando a entrada do programa de cor, na memoria RAM
		GLint uniformColor = glGetUniformLocation(shaderProgram, "triColor"); // pegando o endere�o da vari�vel triColor do fragment shader
		// 3f � porque s�o 3 floats (R, G, B)
		// como colocando um vec tres l� em cima a aqui tambem tem que ser 3
		float r = rand() % 100 / 100.0f; // gerando um n�mero aleat�rio entre 0 e 1
		float g = rand() % 100 / 100.0f; // gerando um n�mero aleat�rio entre 0 e 1
		float b = rand() % 100 / 100.0f; // gerando um n�mero aleat�rio entre 0 e 1
		glUniform3f(uniformColor, r, g, b); // alterando a cor do tri�ngulo para uma cor randomica em tempo de execu��o (fica trocando a cada clock)


		//Desenhado o tri�ngulo
		glUseProgram(shaderProgram); // dizendo qual programa vamos usar

		glBindVertexArray(VAO); // dizendo qual VAO vamos usar
			glDrawArrays(GL_TRIANGLES, 0, 3); // desenhando o tri�ngulo, GL_TRIANGLES � o tipo de desenho, Come�ando na posi��o 0 do array, 3 � a quantidade de v�rtices
		glBindVertexArray(0); // desativando o VAO




		// desenhar
		glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();
}