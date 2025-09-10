#include <iostream>
#include <GL/glew.h>
#include <GLFW//glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

const GLint WIDTH = 800, HEIGHT = 600;
// VAO é o ponteiro do programa que estou rodando, espaço de memória que representa aquele objeto
// VBO é o ponteiro para os meus dados brutos, são os dados do objeto a ser representado pelo VAO 
// shaderProgram é qual programa estou rodando 
// todo programa pode ser chamado de shader
GLuint VAO, VBO, shaderProgram;
GLuint IBO; // Index Buffer Object, é um buffer que guarda os índices dos vértices que formam os triângulos, faz o mapeamento de que ponto liga com qual ponto

// aqui estamos fazendo um programa (shader) em GLSL

// Radiano
float toRadian = 3.1415f / 180.0f; // valor de pi dividido por 180 graus (conversão de graus para radiano)


//float direction = 0.0f; // direção do movimento do triângulo
//float directionSize = 0.01f; // tamanho do movimento do triângulo

bool direction = false, directionSize = false;

float triOffsetSize = 0.2f, triOffsetSizeMax = 1.2f, triOffsetSizeMin = 0.2f, triOffsetSizeIncrement = 0.005f; // tamanho do movimento do triângulo
float triCurrentAngle = 0.0f, triAngleIncrement = 1.0f; // ângulo atual do triângulo e o incremento do ângulo (velocidade de rotação)


// valor atual X e Y, maximo, minimo do deslocamento
float triOffset = 0.0f, triMaxOffset = 0.7f, triMinOffset = -0.7f;
float triIncrement = 0.01f; // valor que vamos somar a cada frame


// Enquando estiver falso vamos somando
//bool directionX = false;
//bool directionY = false;
//float triOffsetX = 0.0f, triOffsetY = 0.0f, triMaxOffset = 0.7f, triMinOffset = -0.7f;
//float triIncrementX = 0.01f;  // valor que vamos somar a cada frame
//float triIncrementY = 0.005f; // valor que vamos somar a cada frame

// shader para renderizar pontos na tela
//static const char* vertexShader = "                           \n\
//#version 330                                                   \n\
//                                                                \n\
//                                                                 \n\
//// passando um argumento para o inicio do programa (args do C//   \n\
//// estou passando um argumento de entrada na primeira posiçâo      \n\
//// esse argumento deve ser um vetor de duas posições                \n\
//layout(location=0) in vec2 pos;                                      \n\
//uniform float posX;                                                   \n\
//uniform float posY;                                                    \n\
//                                                                        \n\
//void main() {                                                            \n\
//	gl_Position = vec4(pos.x + posX, pos.y + posY, 0.0, 1.0);             \n\
//}                                                                          \n\
//";

// shader para renderizar pontos na tela
static const char* vertexShader = "                           \n\
#version 330                                                   \n\
                                                                \n\
                                                                 \n\
// passando um argumento para o inicio do programa (args do C//   \n\
// estou passando um argumento de entrada na primeira posiçâo      \n\
// esse argumento deve ser um vetor de duas posições                \n\
layout(location=0) in vec3 pos;                                      \n\
uniform mat4 model;                                                   \n\
uniform mat4 projection;                                               \n\
out vec4 vCol;														    \n\
                                                                         \n\
void main() {                                                             \n\
	gl_Position = projection * model * vec4(pos, 1.0);                     \n\
	// passando a cor para o fragment shader						   	    \n\
	vCol = vec4(clamp(pos, 0.0f, 1.0f), 1.0);						         \n\
}                                                                             \n\
";


// fragment pode ser lido como "a partir desse ponto o que faço com ele?"
// shader para atribuir cores aos pontos

static const char* fragmentShader = "                                             \n\
#version 330                                                                       \n\
                                                                                    \n\
in vec4 vCol; // recebendo a cor do vertex shader							     	 \n\
																					  \n\
// diferente da entrada por layout, uniform é uma entrada em tempo de execução         \n\
uniform vec3 triColor;															        \n\
// saída do fragment shader, a cor final do pixel na tela                                \n\
out vec4 color;                                                                           \n\
                                                                                           \n\
void main() {                                                                               \n\
	color = vCol;                                                                            \n\
}                                                                                             \n\
";

void create_triangle() {
	unsigned int indices[] = {
		0, 1, 2, // BASE
		0, 1, 3, // ESQUERDA
		0, 2, 3, // DIREITA
		1, 2, 3  // FRENTE
	};

	GLfloat vertices[] = {   // nosso buffer de vertíces
		 0.0f ,  1.0f, 0.0f, // vertice 1 , posição 0
		-1.0f , -1.0f, 0.0f, // vertice 2 , posição 1
		 1.0f , -1.0f, 0.0f, // vertice 3 , posição 2
		 0.0f,  0.0f,  1.0f  // vertice 4 , posição 3
	};

	// iniciar um VAO
	// vamos abrir um espaço de memória na placa de vídeo
	// pegamos o endereço virutal da placa de video e guardamos ele na variável lá em cima
	glGenVertexArrays(1, &VAO);

	// estamos dizendo que a partir desse momento todas as alterações serão feitas nesse espaço de memória
	// agora tudo o que vamos criar será nesse espaço de memória como os pontos da nossa tela
	glBindVertexArray(VAO);

	glGenBuffers(1, &IBO); // alocar um buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO); // a partir desse momento estamos modificando esse buffer
		// GL_ELEMENT_ARRAY_BUFFER é o tipo do buffer, sizeof é o tamanho do buffer, indices é o ponteiro para os dados que queremos colocar no buffer, Gl_STATIC_DRAW é a forma como vamos usar esse buffer (static porque não vamos mudar esses dados)
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); // colocando os dados dentro do buffer

			// iniciar um VBO
			glGenBuffers(1, &VBO); // alocar um buffer
			// agora tudo o que vamos criar será nesse buffer
			glBindBuffer(GL_ARRAY_BUFFER, VBO); // a partir desse momento estamos modificando esse buffer
				// agora vamos colocar os dados dentro desse buffer
				// GL_array_buffer é o tipo do buffer, 
				// sizeof é o tamanho do buffer, vertices é o ponteiro para os dados que queremos colocar no buffer, 
				// GL_STATIC_DRAW é a forma como vamos usar esse buffer (static porque não vamos mudar esses dados)
				glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); 
				// agora vamos explicar como esses dados estão organizados
				// 0 é o layout(location=0) que criamos lá em cima no vertex shader, 3 é porque são 3 posições por vértice (x, y e z), 
				// gl_FLOAT é o tipo de dado, GL_FALSE é se os dados estão normalizados ou não (não estão), 
				// 0 é o espaçamento entre os dados (0 porque são dados contínuos), 0 é o offset (0 porque começam do início)
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // explicando como interpretar esses dados
				// habilitar o atributo que criamos lá em cima
				glEnableVertexAttribArray(0);  // location

			// deixar de apontar para o VAO e VBO
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

// função para executar um buffer na tela
void add_shader(GLuint program, const char* shaderCode, GLenum type) {
	GLuint _shader = glCreateShader(type);

	// converte char para glchar
	const GLchar* code[1];
	// coloca o código do shader na primeira posição do array
	code[0] = shaderCode;


	glShaderSource(_shader, 1, code, NULL);
	glCompileShader(_shader);

	// tratar os erros

	// pegando o status da compilação
	glAttachShader(program, _shader);
}

// função para criar o programa (shader)
void add_program() {
	shaderProgram = glCreateProgram();
	if (!shaderProgram) {
		cout << "Erro: falha ao criar o programa!";
	}

	// adicionando os shaders ao programa
	add_shader(shaderProgram, vertexShader, GL_VERTEX_SHADER);
	add_shader(shaderProgram, fragmentShader, GL_FRAGMENT_SHADER);

	// linkando o programa, a execução
	glLinkProgram(shaderProgram);
}





int main() {
	// inicando GLFW
	if (!glfwInit()) {
		cout << "Erro: GLFW não foi iniciado";
		glfwTerminate();
		return -1;
	}

	// set nas coisas obrigatórias para subir uma janela
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // versão máxima permitida 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // versão mínima
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // somente as funções core
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // qual a precisão de ponto flutuante que vamos usar, precisão da placa, deixando compativel com versões futuras e anteriores

	// criação de janela 
	// NULL, NULL são para monitor, se usa o monitor principal e share, que não vamos usar precisar usar agora
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Window", NULL, NULL);
	if (!window) {
		cout << "Erro: Não foi possível criar a janela";
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
		cout << "Erro: não foi possível iniciar o glew";
		glfwDestroyWindow(window);
		glfwTerminate();
		return -1;
	}

	// 0, 0 é onde a janela será renderizada, ou seja apartir do centro da tela, passando a largura e altura da tela
	glViewport(0, 0, bufferWidth, bufferHeight);

	create_triangle(); // criando o triângulo
	add_program(); // criando o programa
	
	// glm::perspective cria uma matriz de projeção
	// onde 45.0f é o campo de visão e a angulação (FOV), 
	// bufferWidth/bufferHeight é a proporção da tela, 
	// 0.1f é a distância mínima que a câmera enxerga, o quão perto será renderizado, 
	// 100.0f é a distância máxima que a câmera enxerga, o quão longe será renderizado,
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)bufferWidth / (GLfloat)bufferHeight, 0.1f, 100.0f); // criando a matriz de projeção

	while (!glfwWindowShouldClose(window)) {
		// cor de fundo
		glClearColor(0.29f, 0.0f, 0.51f, 1.0f); // RGBA indigo
		// glfwPollEvents() pega os eventos do teclado e mouse
		glfwPollEvents();
		// limpar a tela com a cor de fundo
		glClear(GL_COLOR_BUFFER_BIT);


		//Alterando a cor do triângulo
		// Guardando a entrada do programa de cor, na memoria RAM
		GLint uniformColor = glGetUniformLocation(shaderProgram, "triColor"); // pegando o endereço da variável triColor do fragment shader
		// 3f é porque são 3 floats (R, G, B)
		// como colocando um vec tres lá em cima a aqui tambem tem que ser 3
		float r = rand() % 100 / 100.0f; // gerando um número aleatório entre 0 e 1
		float g = rand() % 100 / 100.0f; // gerando um número aleatório entre 0 e 1
		float b = rand() % 100 / 100.0f; // gerando um número aleatório entre 0 e 1
		glUniform3f(uniformColor, r, g, b); // alterando a cor do triângulo para uma cor randomica em tempo de execução (fica trocando a cada clock)


		//// Movimenta o triangulo
		//GLint uniformPosX = glGetUniformLocation(shaderProgram, "posX"); // pegando o endereço da variável posX do fragment shader
		//GLint uniformPosY = glGetUniformLocation(shaderProgram, "posY"); // pegando o endereço da variável posY do fragment shader
		//glUniform1f(uniformPosX, triOffsetX); // alterando a posição do triângulo
		//glUniform1f(uniformPosY, triOffsetY); // alterando a posição do triângulo

		//// verificando os limites do deslocamento
		//if (directionX) {
		//	triOffsetX += triIncrementX; // somando o incremento ao valor atual
		//}
		//else {
		//	triOffsetX -= triIncrementX; // subtraindo o incremento ao valor atual
		//}

		//if(directionY) {
		//	triOffsetY += triIncrementY; // somando o incremento ao valor atual
		//}
		//else {
		//	triOffsetY -= triIncrementY; // subtraindo o incremento ao valor atual
		//}
		//
		//// se chegar no máximo inverte a direção ou se chegar no mínimo inverte a direção
		//if (triOffsetX >= triMaxOffset || triOffsetX <= triMinOffset) {
		//	directionX = !directionX; 
		//}
		//
		//if (triOffsetY >= triMaxOffset || triOffsetY <= triMinOffset) {
		//	directionY = !directionY;
		//}


		// Faz o triângulo girar e aumentar e diminuir de tamanho
		// Rotação
		if (!direction) {
			triOffset += triIncrement;
		}
		else {
			triOffset -= triIncrement;
		}

		if (triOffset >= triMaxOffset || triOffset <= triMinOffset) {
			direction = !direction;
		}

		triCurrentAngle += triAngleIncrement; // somando o incremento ao valor atual
		if (triCurrentAngle >= 360) {
			triCurrentAngle = 0;
		}

		// Tamanho
		if (!directionSize) {
			triOffsetSize += triOffsetSizeIncrement;
		}
		else {
			triOffsetSize -= triOffsetSizeIncrement;
		}

		if (triOffsetSize >= triOffsetSizeMax || triOffsetSize <= triOffsetSizeMin) {
			directionSize = !directionSize;
		}

		//printf("Offset: %f Angle: %f Size: %f \n", triOffset, triCurrentAngle, triOffsetSize);
		GLint uniformModel = glGetUniformLocation(shaderProgram, "model"); // pegando o endereço da variável model do vertex shader
		GLint uniformProjection = glGetUniformLocation(shaderProgram, "projection"); // pegando o endereço da variável model do vertex shader
		glm::mat4 model(1.0f); // criando uma matriz 4x4 identidade

		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f)); // movendo o triângulo
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f)); // escalonando o triângulo
		model = glm::rotate(model, triCurrentAngle * toRadian, glm::vec3(0.7f, 0.5f, 1.0f)); // rotacionando o triângulo


		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model)); // enviando a matriz para o vertex shader
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection)); // enviando a matriz para o vertex shader



		//Desenhado o triângulo
		glUseProgram(shaderProgram); // dizendo qual programa vamos usar

		glBindVertexArray(VAO); // dizendo qual VAO vamos usar
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO); // dizendo qual IBO vamos usar
				glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0); // desenhando o triângulo, GL_TRIANGLES é o tipo de desenho, 3 é a quantidade de vértices, GL_UNSIGNED_INT é o tipo dos índices, 0 é o offset
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0); // desativando o VAO

		//glBindVertexArray(VAO); // dizendo qual VAO vamos usar
		//	glDrawArrays(GL_TRIANGLES, 0, 3); // desenhando o triângulo, GL_TRIANGLES é o tipo de desenho, Começando na posição 0 do array, 3 é a quantidade de vértices
		//glBindVertexArray(0); // desativando o VAO




		// desenhar
		glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();
}