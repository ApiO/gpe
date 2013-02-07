#include "gl_tmp.h"
#include <stdio.h>
#include "window_manager.h"
#include <SOIL\SOIL.h>
#include <math.h>

#define HEIGHT      600
#define WIDTH       800
/*
F32 worldViewProjectionMatrix[16]; // Concaténation de la matrice world et projection
unsigned int shaderProgram; // Identifiant du programme de shader
unsigned int uniformID; // Emplacement de la variable uniform dans le shader
unsigned int vertexPositionAttrib; // Emplacement de l'attribut position dans le shader
unsigned int vertexColorAttrib; // Emplacement de l'attribut couleur dans le shader
unsigned int cubeBuffers[2]; // Les identifiants des buffer

// Les trois premières valeurs de chaque ligne sont les couleurs et les trois suivantes les coordonnées
float CubeArray[48] = {
	1.0f, 0.0f, 0.0f, -1.0f, 1.0f, -1.0f,
	1.0f, 0.0f, 1.0f, -1.0f, -1.0f, -1.0f,
	1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f,
	0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f,
	1.0f, 0.0f, 1.0f, 1.0f, 1.0f, -1.0f,
	1.0f, 1.0f, 0.0f, 1.0f, -1.0f, -1.0f
};

// Les indices de vertices
unsigned int IndiceArray[36] = {
	0,1,2,2,1,3,
	4,5,6,6,5,7,
	3,1,5,5,1,7,
	0,2,6,6,2,4,
	6,7,0,0,7,1,
	2,3,4,4,3,5
};

void SetProjectionMatrix (F32 *data, const float fov_, const float aspect_, const float nearPlane_, const float farPlane_);
void SetTransformationMatrix (F32 *data, const float x_, const float y_, const float z_);
float *GetMatrixForOpenGL (F32 *data);
void multMat ( F32 *mat1_, F32 *mat2_, F32 *res);
void InitOpenGL();
static void LoadShaders ();
static void CreateBuffers ();
void display();


void GL_TMP_H_FOO()
{
  window_manager w;
  gpr_memory_init(410241024);

  window_manager_init(&w, "simple gl rendering test", HEIGHT, WIDTH);
  w.display_axes = 1;
  w.display_fps = 1;
  
	// Initialisation d'OpenGL
	InitOpenGL ();

	// Chargement des shaders
	LoadShaders ();

	// Creation des buffers
	CreateBuffers ();

  while(w.running)
  {
    window_manager_clear(&w);

    display();

    window_manager_swapBuffers(&w);
  }
  
  window_manager_free(&w);
  gpr_memory_shutdown();
}

void InitOpenGL()
{
	// Création des matrices de transformation. Pour plus de simplicité, les matrices ne bougeront jamais
	F32 projectionMatrix[16];
	SetProjectionMatrix(projectionMatrix, 60.0f, 1.0f, 1.0f, 500.0f);
	
	F32 worldMatrix[16];
	SetTransformationMatrix(worldMatrix, 0.0f, 0.0f, -5.0f);

	multMat(projectionMatrix, worldMatrix, worldViewProjectionMatrix);
}

static void LoadShaders ()
{
	// Création des shaders
	unsigned int vertexShaderID = glCreateShader (GL_VERTEX_SHADER);
	unsigned int fragmentShaderID = glCreateShader (GL_FRAGMENT_SHADER);
	
	// On charge les sources
	const char * vertexShader = "#version 130\n\n// Attributs\nin vec3 VertexColor;\nin vec3 VertexPosition;\n\n// Uniform\nuniform mat4 ModelViewProjectionMatrix;\n\n// gl_Position déclaré invariant (voir tutoriel sur les shaders avec OpenGL 3.x pour plus d'informations)\ninvariant gl_Position;\n\n// Couleur interpolée, passée au fragment shader\nsmooth out vec3 InterpolatedColor;\n\nvoid main ()\n{\n	InterpolatedColor = VertexColor;\n	gl_Position = ModelViewProjectionMatrix * vec4 (VertexPosition, 1.0);\n}\0";
	const char * fragmentShader = "#version 130\n\nprecision highp float;\n\nsmooth in vec3 InterpolatedColor;\n\n// Sortie du fragment shader, ira dans le framebuffer principal\nout vec4 Color;\n\nvoid main()\n{\n	Color = vec4 (InterpolatedColor, 1.0);\n}\0";
	glShaderSource (vertexShaderID, 1, &vertexShader, NULL);
	glShaderSource (fragmentShaderID, 1, &fragmentShader, NULL);

	// On compile les shaders
	glCompileShader (vertexShaderID);
	glCompileShader (fragmentShaderID);
	
	// On vérifie qu'ils aient bien été compilés
	GLint error;
	glGetShaderiv (vertexShaderID, GL_COMPILE_STATUS, &error);
	if (error != GL_TRUE)
		printf("Probleme!");

	glGetShaderiv (fragmentShaderID, GL_COMPILE_STATUS, &error);
	if (error != GL_TRUE)
		printf("Probleme!");
	
	// Création du programme
	shaderProgram = glCreateProgram ();
	glAttachShader (shaderProgram, vertexShaderID);
	glAttachShader (shaderProgram, fragmentShaderID);

	// Linkage du programme
	glLinkProgram (shaderProgram);
	
	// On récupère l'emplacement de la variable uniform dans le shader
	uniformID = glGetUniformLocation (shaderProgram, "ModelViewProjectionMatrix");
	
	// On récupère les emplacements des deux attributs dans le shader
	vertexPositionAttrib = glGetAttribLocation (shaderProgram, "VertexPosition");
	vertexColorAttrib = glGetAttribLocation (shaderProgram, "VertexColor");
}

static void CreateBuffers ()
{
	// Génération des buffers
	glGenBuffers (2, cubeBuffers);

	// Buffer d'informations de vertices
	glBindBuffer(GL_ARRAY_BUFFER, cubeBuffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(CubeArray), CubeArray, GL_STATIC_DRAW);

	// Buffer d'indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeBuffers[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(IndiceArray), IndiceArray, GL_STATIC_DRAW);
}

void display()
{
	// On commence par activer le shader
	glUseProgram (shaderProgram);

	// On envoit la variable uniform
	glUniformMatrix4fv (uniformID, 1, GL_FALSE, GetMatrixForOpenGL(worldViewProjectionMatrix));

	// On active les deux tableaux génériques
	glEnableVertexAttribArray (vertexPositionAttrib);
	glEnableVertexAttribArray (vertexColorAttrib);
		
	// On active le buffer des vertices
	glBindBuffer(GL_ARRAY_BUFFER, cubeBuffers[0]);

	// On envoie les données
	glVertexAttribPointer (vertexColorAttrib, 3, GL_FLOAT, GL_FALSE, sizeof (float) * 6, (float *) NULL + (0));
	glVertexAttribPointer (vertexPositionAttrib, 3, GL_FLOAT, GL_FALSE, sizeof (float) * 6, (float *) NULL + (3));
		
	// On active le buffer des indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeBuffers[1]);

	// Rendu de notre géométrie
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	// On désactive les tableaux génériques
	glDisableVertexAttribArray (vertexPositionAttrib);
	glDisableVertexAttribArray (vertexColorAttrib);

	// On désactive le shader
	glUseProgram (0);
}

// Construit une matrice de projection
void SetProjectionMatrix (F32 *data, const float fov_, const float aspect_, const float nearPlane_, const float farPlane_)
{
  int i;
	float maxY = nearPlane_ * tanf (fov_ * 3.14159256f / 360.0f);
	float minY = -maxY;
	float minX = minY * aspect_;
	float maxX = maxY * aspect_;

  for (i=0; i<16; i++) data[i]=0.0f;

	data[0] = 2.0f * nearPlane_ / (maxX - minX);
	data[5] = 2.0f * nearPlane_ / (maxY - minY);
	data[8] = (maxX + minX) / (maxX - minX);
	data[9] = (maxY + minY) / (maxY - minY);
	data[10] = -(farPlane_ + nearPlane_) / (farPlane_ - nearPlane_);
	data[11] = -1.0f;
	data[14] = -(2.0f * farPlane_ * nearPlane_) / (farPlane_ - nearPlane_);
}

void SetTransformationMatrix (F32 *data, const float x_, const float y_, const float z_)
{
	data[12] += x_;
	data[13] += y_;
	data[14] += z_;
}

// Retourne la matrice sous la forme d'un pointeur (près pour être envoyé à OpenGL)
float *GetMatrixForOpenGL (F32 *data)
{
	return &data[0];
}

// Multiplie deux matrices
void multMat ( F32 *mat1_, F32 *mat2_, F32 *res)
{
  res [0] = mat1_[0] * mat2_[0] + mat1_[4] * mat2_[1] + mat1_[8] * mat2_[2] + mat1_[12] * mat2_[3];
  res [1] = mat1_[1] * mat2_[0] + mat1_[5] * mat2_[1] + mat1_[9] * mat2_[2] + mat1_[13] * mat2_[3];
  res [2] = mat1_[2] * mat2_[0] + mat1_[6] * mat2_[1] + mat1_[10] * mat2_[2] + mat1_[14] * mat2_[3];
  res [3] = mat1_[3] * mat2_[0] + mat1_[7] * mat2_[1] + mat1_[11] * mat2_[2] + mat1_[15] * mat2_[3];

  res [4] = mat1_[0] * mat2_[4] + mat1_[4] * mat2_[5] + mat1_[8] * mat2_[6] + mat1_[12] * mat2_[7];
  res [5] = mat1_[1] * mat2_[4] + mat1_[5] * mat2_[5] + mat1_[9] * mat2_[6] + mat1_[13] * mat2_[7];
  res [6] = mat1_[2] * mat2_[4] + mat1_[6] * mat2_[5] + mat1_[10] * mat2_[6] + mat1_[14] * mat2_[7];
  res [7] = mat1_[3] * mat2_[4] + mat1_[7] * mat2_[5] + mat1_[11] * mat2_[6] + mat1_[15] * mat2_[7];

  res [8] = mat1_[0] * mat2_[8] + mat1_[4] * mat2_[9] + mat1_[8] * mat2_[10] + mat1_[12] * mat2_[11];
  res [9] = mat1_[1] * mat2_[8] + mat1_[5] * mat2_[9] + mat1_[9] * mat2_[10] + mat1_[13] * mat2_[11];
  res [10] = mat1_[2] * mat2_[8] + mat1_[6] * mat2_[9] + mat1_[10] * mat2_[10] + mat1_[14] * mat2_[11];
  res [11] = mat1_[3] * mat2_[8] + mat1_[7] * mat2_[9] + mat1_[11] * mat2_[10] + mat1_[15] * mat2_[11];

  res [12] = mat1_[0] * mat2_[12] + mat1_[4] * mat2_[13] + mat1_[8] * mat2_[14] + mat1_[12] * mat2_[15];
  res [13] = mat1_[1] * mat2_[12] + mat1_[5] * mat2_[13] + mat1_[9] * mat2_[14] + mat1_[13] * mat2_[15];
  res [14] = mat1_[2] * mat2_[12] + mat1_[6] * mat2_[13] + mat1_[10] * mat2_[14] + mat1_[14] * mat2_[15];
  res [15] = mat1_[3] * mat2_[12] + mat1_[7] * mat2_[13] + mat1_[11] * mat2_[14] + mat1_[15] * mat2_[15];
}

*/

void GL_TMP_H_FOO()
{
  window_manager w;
  U32 vbo;
  U32 tex_id;
  F32 verticies[8] = { 0,0, 0,128, 128,128, 128,0 };
  F32 tex_coord[8] = { 0,0, 0,1, 1,1, 1,0};
  
  
  gpr_memory_init(410241024);

  window_manager_init(&w, "simple gl rendering test", HEIGHT, WIDTH);
  w.display_axes = 1;
  w.display_fps = 1;

  printf("GL_ARB_vertex_buffer_object %s", glewIsSupported("GL_ARB_vertex_buffer_object") ? "SUPPORTED" : "NOT supported");
  
  tex_id = SOIL_load_OGL_texture (
    "..\\..\\src\\ressources\\patate2.png", 
    SOIL_LOAD_AUTO, 
    SOIL_CREATE_NEW_ID, 
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_POWER_OF_TWO );
  
	// Generate And Bind The Vertex Buffer
	glGenBuffersARB( 1, &vbo );							          // Get A Valid Name
	glBindBufferARB( GL_ARRAY_BUFFER_ARB, vbo );			// Bind The Buffer
	// Load The Data
	//glBufferDataARB( GL_ARRAY_BUFFER_ARB, sizeof(verticies), verticies, GL_STATIC_DRAW_ARB );
  glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(verticies), NULL, GL_STATIC_DRAW_ARB);
  glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, sizeof(verticies), verticies);


  while(w.running)
  {
    window_manager_clear(&w);

		glBindBufferARB( GL_ARRAY_BUFFER_ARB, vbo );
    
		glVertexPointer( 2, GL_FLOAT, 0, 0 );		// Set The Vertex Pointer To The Vertex Buffer

	  glDrawArrays( GL_QUADS, 0, 4 );	// Draw All Of The Triangles At Once
    

    /*
    glColor3f(1.f, 1.f, 1.f);
    glTexCoordPointer(2, GL_FLOAT, 0, tex_coord);
    glVertexPointer(2, GL_FLOAT, 0, verticies);
    glBindTexture(GL_TEXTURE_2D, tex_id);  
    glDrawArrays(GL_QUADS, 0, 4);
    */
    window_manager_swapBuffers(&w);
  }

  
  glDeleteBuffersARB(1, &vbo);

  
  glDeleteTextures(1, &tex_id);
  window_manager_free(&w);
  gpr_memory_shutdown();
}
