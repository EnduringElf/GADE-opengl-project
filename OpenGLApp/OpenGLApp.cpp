#include <iostream>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include "SOIL2.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other includes
#include "Shader.h"
#include "Camera.h"
#include <vector>
#include <chrono>
#include "Camera.h"

using namespace std;


// Properties
const GLuint WIDTH = 1600, HEIGHT = 1200;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Function prototypes
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void ScrollCallback(GLFWwindow* window, double xOffset, double yOffset);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);

void DoMovement();

// Camera
Camera  camera(glm::vec3(0.0f, 0.0f, 3.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

float columns = 7;
float rows = 7;

void drawBoard(Shader W, Shader B, Shader G, GLuint VAO, GLfloat y[], GLuint texture);
void drawTerrain(Shader &s, GLuint VAO, const unsigned int NUM_STRIPS, const unsigned int NUM_VERTS_PER_STRIP);
//void drawTerraintwo(Shader& s, GLuint VAO2, const unsigned int NUM_STRIPS, const unsigned int NUM_VERTS_PER_STRIP);
void DrawPiece(Shader W, Shader B, GLuint VAO, GLfloat y[], int xpos, float ypos, int zpos, float xscale, float yscale, float zscale);
void drawPawn(Shader W, Shader B, GLuint VAO, GLfloat y[]);
void drawKing(Shader W, Shader B, GLuint VAO, GLfloat y[]);
void DrawQueen(Shader W, Shader B, GLuint VAO, GLfloat y[]);
void DrawBishop(Shader W, Shader B, GLuint VAO, GLfloat y[]);
void DrawRook(Shader W, Shader B, GLuint VAO, GLfloat y[]);
void drawspotlight(Shader L);
void drawPointlight(Shader L);
void drawDirectionalLight(Shader L);
void printFPS();
unsigned int loadCubemap(string faces[]);




// The MAIN function, from here we start the application and run the game loop
int main()
{
    // Init GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr); // Windowed

  
   

    if (nullptr == window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();

        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);

    glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

    // Set the required callback functions
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetScrollCallback(window, ScrollCallback);
    Camera  camera(glm::vec3(0.0f, 0.0f, 3.0f));
    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
    // Options, removes the mouse cursor for a more immersive experience
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    if (GLEW_OK != glewInit())
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return EXIT_FAILURE;
    }

    // Define the viewport dimensions
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    // Setup OpenGL options
    glEnable(GL_DEPTH_TEST);

    // enable alpha support
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Build and compile our shader program
    Shader HightmapShader("res/shaders/Hightmap.vs", "res/shaders/Hightmap.frag");
    Shader ourShader("res/shaders/core.vs", "res/shaders/core.frag");
    Shader WhiteColor("res/shaders/whiteC.vs", "res/shaders/whiteC.frag");
    Shader BlackColor("res/shaders/blackC.vs", "res/shaders/BlackC.frag");
    Shader grayColor("res/shaders/grayC.vs", "res/shaders/grayC.frag");
    Shader SecondTexture("res/shaders/texturehight.vs", "res/shaders/texturehight.frag");
    
    // Build and compile our shader program
    Shader lightingShader("res/shaders/lighting.vs", "res/shaders/lighting.frag");
    //spot light
    Shader lampShader("res/shaders/spotlight.vs", "res/shaders/spotlight.frag");
    //point light
    Shader PointShader("res/shaders/PointLight.vs", "res/shaders/PointLight.frag");
    //skybox shader
    Shader skyboxShader("res/shaders/Skybox.vs", "res/shaders/Skybox.frag");


    // Positions of the point lights
    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f,  0.2f,  2.0f),
        glm::vec3(200.3f, -30.3f, -40.0f),
        glm::vec3(-200.3f, -60.3f, -90.0f),
        glm::vec3(300.0f,  1.0f, -3.0f)
    };


    GLfloat vertices2[] =
    {
        // Positions            // Normals              // Texture Coords
        -0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,
        0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,
        0.5f, -0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f,  1.0f,
        0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,  	1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  0.0f,

        0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  1.0f,
        0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  1.0f,
        0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  1.0f,
        0.5f, -0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  0.0f,
        0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  1.0f,
        0.5f, -0.5f, -0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  1.0f,
        0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  1.0f,
        0.5f,  0.5f, -0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  1.0f,
        0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  1.0f
    };

    // Positions all containers
    glm::vec3 cubePositions2[] = {
        glm::vec3(0.0f,   0.0f,   0.0f),
        glm::vec3(2.0f,   5.0f,   -15.0f),
        glm::vec3(-1.5f,  -2.2f,  -2.5f),
        glm::vec3(-3.8f,  -2.0f,  -12.3f),
        glm::vec3(2.4f,   -0.4f,  -3.5f),
        glm::vec3(-1.7f,  3.0f,   -7.5f),
        glm::vec3(1.3f,   -2.0f,  -2.5f),
        glm::vec3(1.5f,   2.0f,   -2.5f),
        glm::vec3(1.5f,   0.2f,   -1.5f),
        glm::vec3(-1.3f,  1.0f,   -1.5f)
    };
    //cube vertices
    GLfloat vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    }; 
    //skybox vertices
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    //y rand position for the board
    GLfloat ypos[121];

    for (size_t i = 0; i < 121; i++)
    {

        ypos[i] = ((float)rand() / 100000);

    }

    //vertices ++ indices for terrain
    int width, height, nChannels;
   // int width1, height1, nChannels2;
   // int width3, height3, nChannels3;
    unsigned char* data = SOIL_load_image("res/heightmaps/Hightmap50x50.png", &width, &height, &nChannels, 0);
    //unsigned char* data2 = SOIL_load_image("res/images/image2.png", &width3, &height3, &nChannels, 0);
    if (data)
    {
        std::cout << "Loaded heightmap of size " << height << " x " << width << std::endl;
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    glBindTexture(GL_TEXTURE_2D, 1);
    glBindTexture(GL_TEXTURE_2D, 2);

    // vertex generation
    std::vector<float> Hvertices;
    float yScale = 64.0f / 256.0f, yShift = 16.0f;  // apply a scale+shift to the height data
    for (unsigned int i = 0; i < height; i++)
    {
        for (unsigned int j = 0; j < width; j++)
        {
            // retrieve texel for (i,j) tex coord
            unsigned char* texel = data + (j + width * i) * nChannels;
          //  unsigned char* texel2 = data2 + (j + width * i) * nChannels;
            // raw height at coordinate
            unsigned char y = texel[0];
            //unsigned char x = texel2[0];

            // vertex
            Hvertices.push_back(-height / 2.0f + i);        // v.x
            Hvertices.push_back((int)y * yScale - yShift);  // v.y
            Hvertices.push_back(-width / 2.0f + j);        // v.z
        }
    }

   // std::vector<float> Hvertices2;
   // float yScale2 = 64.0f / 256.0f, yShift2 = 16.0f;  // apply a scale+shift to the height data
   // for (unsigned int i = 0; i < height3; i++)
   // {
   //     for (unsigned int j = 0; j < width3; j++)
   //     {
   //         // retrieve texel for (i,j) tex coord
   //         unsigned char* texel2 = data2 + (j + width3 * i) * nChannels;
   //         // raw height at coordinate
   //       
   //         unsigned char x = texel2[0];

   //         // vertex
   //         Hvertices2.push_back(-height3 / 2.0f + i);        // v.x
   //         Hvertices2.push_back((int)x * yScale - yShift);  // v.y
   //         Hvertices2.push_back(-width3 / 2.0f + j);        // v.z
   //     }
   // }
   // std::cout << "Loaded " << Hvertices.size() / 3 << " vertices" << std::endl;
   // SOIL_free_image_data(data);
   //// SOIL_free_image_data(data2);

    // index generation
    std::vector<unsigned int> indices;
    for (unsigned int i = 0; i < height - 1; i++)       // for each row a.k.a. each strip
    {
        for (unsigned int j = 0; j < width; j++)      // for each column
        {
            for (unsigned int k = 0; k < 2; k++)      // for each side of the strip
            {
                indices.push_back(j + width * (i + k));
            }
        }
    }
    std::cout << "Loaded " << indices.size() << " indices" << std::endl;
    const unsigned int NUM_STRIPS = height - 1;
    const unsigned int NUM_VERTS_PER_STRIP = width * 2;
    std::cout << "Created lattice of " << NUM_STRIPS << " strips with " << NUM_VERTS_PER_STRIP << " triangles each" << std::endl;
    std::cout << "Created " << NUM_STRIPS * NUM_VERTS_PER_STRIP << " triangles total" << std::endl;

    // register terrain vaos
    GLuint terrainVAO, terrainVBO, terrainEBO;
    glGenVertexArrays(1, &terrainVAO);
    glBindVertexArray(terrainVAO);
    glGenBuffers(1, &terrainVBO);
    glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
    glBufferData(GL_ARRAY_BUFFER, Hvertices.size() * sizeof(float), &Hvertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);
    glGenBuffers(1, &terrainEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    glBindVertexArray(0);

    //NEW
    // First, set the container's VAO (and VBO)
    GLuint VBObox, boxVAO;
    glGenVertexArrays(1, &boxVAO);
    glGenBuffers(1, &VBObox);

    glBindBuffer(GL_ARRAY_BUFFER, VBObox);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(boxVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    // Then, we set the light's VAO (VBO stays the same. After all, the vertices are the same for the light object (also a 3D cube))
    GLuint lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    // We only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need.
    glBindBuffer(GL_ARRAY_BUFFER, VBObox);
    // Set the vertex attributes (only position data for the lamp))
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0); // Note that we skip over the other data in our buffer object (we don't need the normals/textures, only positions).
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);


    // Load textures
    GLuint diffuseMap, specularMap, emissionMap;
    glGenTextures(1, &diffuseMap);
    glGenTextures(1, &specularMap);
    glGenTextures(1, &emissionMap);

    int imageWidth, imageHeight;
    
    // Diffuse map
    unsigned char* image5 = SOIL_load_image("res/images/image2.png", &imageWidth, &imageHeight, 0, SOIL_LOAD_RGB);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image5);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image5);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);

    // Specular map
    unsigned char* image6 = SOIL_load_image("res/images/image2.png", &imageWidth, &imageHeight, 0, SOIL_LOAD_RGB);
    glBindTexture(GL_TEXTURE_2D, specularMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image6);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image6);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Load textures
    GLuint diffuseMap1, specularMap1, emissionMap1;
    glGenTextures(1, &diffuseMap1);
    glGenTextures(1, &specularMap1);
    glGenTextures(1, &emissionMap1);
    //NEW
   
   
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    //Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    //texture coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0); // Unbind VAO
    GLuint texture;
    // ===================
    // Texture
    // ===================
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Set our texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    


    // Load, create texture and generate mipmaps
    int hwidth, hheight;
    unsigned char* image3 = SOIL_load_image("res/images/image1.jpg", &hwidth, &hheight, 0, SOIL_LOAD_RGBA);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, hwidth, hheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image3);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image3);
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
  
    // Set texture units
    lightingShader.Use();
    glUniform1i(glGetUniformLocation(lightingShader.Program, "material.diffuse"), 0);
    glUniform1i(glGetUniformLocation(lightingShader.Program, "material.specular"), 1);

    glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);

    // skybox VAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // load textures
    // -------------
    string faces[] = {
        "res/images/right.jpg",
        "res/images/left.jpg",
        "res/images/top.jpg",
        "res/images/bottom.jpg",
        "res/images/front.jpg",
        "res/images/back.jpg"
    };
    unsigned int cubemapTexture = loadCubemap(faces);

    skyboxShader.Use();
    glUniform1i(glGetUniformLocation(skyboxShader.Program, "skybox"), 0);


    // check the above later 
    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        // Set frame time
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Check and call events
        glfwPollEvents();
        DoMovement();

        // Clear the colorbuffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDepthFunc(GL_LEQUAL);
        glDepthMask(GL_FALSE);  // change depth function so depth test passes when values are equal to depth buffer's content

        // draw skybox as last
        glm::mat4 view;
        glm::mat4 projection(1.f);
        projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);
        
        skyboxShader.Use();
        view = camera.GetViewMatrix();
        //view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        glm::mat4 Skyboxmodel;
        
        glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        //skyboxShader.setMat4("projection", projection);
        glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(Skyboxmodel));
        Skyboxmodel = glm::mat4(1.0f);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthMask(GL_TRUE); // set depth function back to default
        glDepthFunc(GL_LESS);

        
        
        drawBoard(WhiteColor , BlackColor, grayColor, VAO, ypos, texture);
                                       // make sure to draw lights first so we can map them to the terrain below these calls
        drawspotlight(lightingShader);          //spot light
        drawDirectionalLight(lightingShader);  //directional light (lights entire map)
        drawPointlight(lightingShader);       // point lights we have 3 in scene 
        drawTerrain(lightingShader,  terrainVAO,  NUM_STRIPS, NUM_VERTS_PER_STRIP);
        //drawTerraintwo(HightmapShader, terrainVAOtwo, NUM_STRIPS, NUM_VERTS_PER_STRIP);

        drawPawn(grayColor, BlackColor, VAO, ypos);
        drawKing(grayColor, BlackColor, VAO, ypos);
        DrawQueen(grayColor, BlackColor, VAO, ypos);
        DrawBishop(grayColor, BlackColor, VAO, ypos);
        DrawRook(grayColor, BlackColor, VAO, ypos);
        printFPS();
        
        

        // Create camera transformation
        
        //NEW
        lightingShader.Use();
        GLint lightPosLoc = glGetUniformLocation(lightingShader.Program, "light.position");
        GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
        glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
       // glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);

        glUniformMatrix4fv(viewPosLoc, 1, GL_FALSE, glm::value_ptr(view));

        // Set the defualt lights properties to intitlize the rest of the lights to come
        glUniform3f(glGetUniformLocation(lightingShader.Program, "light.ambient"), 60.2f, 0.2f, 0.2f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "light.diffuse"), 50.5f, 99.5f, 90.5f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "light.specular"), 50.0f, 1.0f, 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "light.constant"), 7.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "light.linear"), 0.09);

        // making this a higher number makes lights less bright 
        glUniform1f(glGetUniformLocation(lightingShader.Program, "light.quadratic"), 0.032);
        // Set material properties
        glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 599.0f);

        // Create camera transformations
        
        view = camera.GetViewMatrix();

        // Get the uniform locations
        GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
        GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
        GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");
        // Pass the matrices to the shader
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // Bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        // Bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        // Draw 10 containers with the same VAO and VBO information; only their world space coordinates differ
        glm::mat4 model;
        glBindVertexArray(boxVAO);
        for (GLuint i = 0; i < 10; i++)
        {
            model = glm::mat4();
            model = glm::translate(model, cubePositions2[i]);
            GLfloat angle = 20.0f * i;
            model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glBindVertexArray(0);

       // Also draw the lamp object, again binding the appropriate shader
        lampShader.Use();
        // Get location objects for the matrices on the lamp shader (these could be different on a different shader)
        modelLoc = glGetUniformLocation(lampShader.Program, "model");
        viewLoc = glGetUniformLocation(lampShader.Program, "view");
        projLoc = glGetUniformLocation(lampShader.Program, "projection");
        // Set matrices
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        model = glm::mat4();
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        // Draw the light object (using light's vertex attributes)
        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        //NEW
        // We now draw as many light bulbs as we have point lights.
        glBindVertexArray(lightVAO);
        for (GLuint i = 0; i < 4; i++)
        {
            model = glm::mat4();
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glBindVertexArray(0);
        glfwSwapBuffers(window);
        
    }
    // Properly de-allocate all resources once they've outlived their purpose
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &terrainVAO);
    glDeleteBuffers(1, &terrainVBO);
    glDeleteBuffers(1, &terrainEBO);
    glDeleteBuffers(1, &skyboxVBO);
    glDeleteVertexArrays(1, &skyboxVAO);


    //NEW
    glDeleteVertexArrays(1, &boxVAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBObox);
    //NEW
    glfwTerminate();

    return EXIT_SUCCESS;
}



void printFPS() {
    //create a FPS counter that displays to our console 
    static std::chrono::time_point<std::chrono::steady_clock> oldTime = std::chrono::high_resolution_clock::now();
    static int fps; fps++;

    if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - oldTime) >= std::chrono::seconds{ 1 }) {
        oldTime = std::chrono::high_resolution_clock::now();
        std::cout << "FPS: " << fps << std::endl;
        fps = 0;
    }
}

void drawspotlight (Shader L)

{
    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

    // Positions of the point lights
    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f,  0.2f,  2.0f),
        glm::vec3(200.3f, -30.3f, -40.0f),
        glm::vec3(-200.3f, -60.3f, -90.0f),
        glm::vec3(300.0f,  1.0f, -3.0f)
    };

    L.Use();
    GLint lightPosLoc = glGetUniformLocation(L.Program, "light.position");
    GLint viewPosLoc = glGetUniformLocation(L.Program, "viewPos");
    glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
    // SpotLight
    glUniform3f(glGetUniformLocation(L.Program, "spotLight.position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
    glUniform3f(glGetUniformLocation(L.Program, "spotLight.direction"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
    glUniform3f(glGetUniformLocation(L.Program, "spotLight.ambient"), 200.0f, 200.0f, 200.0f);
    glUniform3f(glGetUniformLocation(L.Program, "spotLight.diffuse"), 10.0f, 10.0f, 10.0f);
    glUniform3f(glGetUniformLocation(L.Program, "spotLight.specular"), 10.0f, 10.0f, 10.0f);
    glUniform1f(glGetUniformLocation(L.Program, "spotLight.constant"), 10.0f);
    glUniform1f(glGetUniformLocation(L.Program, "spotLight.linear"), 0.09f);
    glUniform1f(glGetUniformLocation(L.Program, "spotLight.quadratic"), 0.032f);
    glUniform1f(glGetUniformLocation(L.Program, "spotLight.cutOff"), glm::cos(glm::radians(12.5f)));
    glUniform1f(glGetUniformLocation(L.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(15.0f)));
}

void drawDirectionalLight(Shader L)
{
    L.Use();
    GLint lightPosLoc = glGetUniformLocation(L.Program, "light.position");
    GLint viewPosLoc = glGetUniformLocation(L.Program, "viewPos");

    // Directional light
    glUniform3f(glGetUniformLocation(L.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
    glUniform3f(glGetUniformLocation(L.Program, "dirLight.ambient"), 1.05f, 1.05f, 1.05f);
    glUniform3f(glGetUniformLocation(L.Program, "dirLight.diffuse"), 0.4f, 0.4f, 0.4f);
    glUniform3f(glGetUniformLocation(L.Program, "dirLight.specular"), 0.5f, 0.5f, 0.5f);
}

void drawPointlight(Shader L)

{
    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

    // Positions of the point lights
    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f,  0.2f,  2.0f),
        glm::vec3(200.3f, -30.3f, -40.0f),
        glm::vec3(-200.3f, -60.3f, -90.0f),
        glm::vec3(300.0f,  1.0f, -3.0f)
    };

    L.Use();
    GLint lightPosLoc = glGetUniformLocation(L.Program, "light.position");
    GLint viewPosLoc = glGetUniformLocation(L.Program, "viewPos");
    glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
    // SpotLight
    glUniform3f(glGetUniformLocation(L.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
    glUniform3f(glGetUniformLocation(L.Program, "pointLights[0].ambient"), 15.05f, 15.05f, 15.05f);
    glUniform3f(glGetUniformLocation(L.Program, "pointLights[0].diffuse"), 0.8f, 0.8f, 0.8f);
    glUniform3f(glGetUniformLocation(L.Program, "pointLights[0].specular"), 78.0f, 78.0f, 78.0f);
    glUniform1f(glGetUniformLocation(L.Program, "pointLights[0].constant"), 1.0f);
    glUniform1f(glGetUniformLocation(L.Program, "pointLights[0].linear"), 0.09f);
    glUniform1f(glGetUniformLocation(L.Program, "pointLights[0].quadratic"), 0.032f);

    // Point light 2
    glUniform3f(glGetUniformLocation(L.Program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
    glUniform3f(glGetUniformLocation(L.Program, "pointLights[1].ambient"), 10.05f, 10.05f, 10.05f);
    glUniform3f(glGetUniformLocation(L.Program, "pointLights[1].diffuse"), 0.8f, 0.8f, 0.8f);
    glUniform3f(glGetUniformLocation(L.Program, "pointLights[1].specular"), 1.0f, 1.0f, 1.0f);
    glUniform1f(glGetUniformLocation(L.Program, "pointLights[1].constant"), 1.0f);
    glUniform1f(glGetUniformLocation(L.Program, "pointLights[1].linear"), 0.09f);
    glUniform1f(glGetUniformLocation(L.Program, "pointLights[1].quadratic"), 0.032f);

    // Point light 3
    glUniform3f(glGetUniformLocation(L.Program, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
    glUniform3f(glGetUniformLocation(L.Program, "pointLights[2].ambient"), 15.05f, 15.05f, 15.05f);
    glUniform3f(glGetUniformLocation(L.Program, "pointLights[2].diffuse"), 0.8f, 0.8f, 0.8f);
    glUniform3f(glGetUniformLocation(L.Program, "pointLights[2].specular"), 1.0f, 1.0f, 1.0f);
    glUniform1f(glGetUniformLocation(L.Program, "pointLights[2].constant"), 1.0f);
    glUniform1f(glGetUniformLocation(L.Program, "pointLights[2].linear"), 0.09f);
    glUniform1f(glGetUniformLocation(L.Program, "pointLights[2].quadratic"), 0.032f);

    // Point light 4
    glUniform3f(glGetUniformLocation(L.Program, "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
    glUniform3f(glGetUniformLocation(L.Program, "pointLights[3].ambient"), 0.05f, 0.05f, 0.05f);
    glUniform3f(glGetUniformLocation(L.Program, "pointLights[3].diffuse"), 0.8f, 0.8f, 0.8f);
    glUniform3f(glGetUniformLocation(L.Program, "pointLights[3].specular"), 1.0f, 1.0f, 1.0f);
    glUniform1f(glGetUniformLocation(L.Program, "pointLights[3].constant"), 1.0f);
    glUniform1f(glGetUniformLocation(L.Program, "pointLights[3].linear"), 0.09f);
    glUniform1f(glGetUniformLocation(L.Program, "pointLights[3].quadratic"), 0.032f);

}

void drawPawn(Shader W, Shader B, GLuint VAO, GLfloat y[]) {

    int zpos = 2;
    float offset = 0.7;

    for (size_t i = 0; i < 9; i++)
    {
        DrawPiece(W, B, VAO, y, i+1,y[i+19]+offset, zpos, 0.8f, 0.2f, 0.8f);
    }
    for (size_t i = 0; i < 9; i++)
    {
        DrawPiece(W, B, VAO, y, i+1, y[i + 10] +0.6 + offset, zpos, 0.5f, 1.0f, 0.5f);
    }
    for (size_t i = 0; i < 9; i++)
    {
        DrawPiece(W, B, VAO, y, i+1, y[i + 10] +1 + offset, zpos, 0.7f, 0.2f, 0.7f);
    }
    for (size_t i = 0; i < 9; i++)
    {
        DrawPiece(W, B, VAO, y, i+1, y[i + 10] + 1.3 + offset, zpos, 0.2f, 0.2f, 0.2f);
    }

    for (size_t i = 0; i < 9; i++)
    {
        DrawPiece(W, B, VAO, y, i + 1, y[i + 19] + offset, zpos-1, 0.8f, 0.2f, 0.8f);
    }
    for (size_t i = 0; i < 9; i++)
    {
        DrawPiece(W, B, VAO, y, i + 1, y[i + 10] + 0.6 + offset, zpos-1, 0.5f, 1.0f, 0.5f);
    }

    zpos = 8;
    for (size_t i = 0; i < 9; i++)
    {
        DrawPiece(B, B, VAO, y, i + 1, y[i + 89] + offset, zpos, 0.8f, 0.2f, 0.8f);
    }
    for (size_t i = 0; i < 9; i++)
    {
        DrawPiece(B, B, VAO, y, i + 1, y[i + 89] + 0.6 + offset, zpos, 0.5f, 1.0f, 0.5f);
    }
    for (size_t i = 0; i < 9; i++)
    {
        DrawPiece(B, B, VAO, y, i + 1, y[i +89] + 1 + offset, zpos, 0.7f, 0.2f, 0.7f);
    }
    for (size_t i = 0; i < 9; i++)
    {
        DrawPiece(B, B, VAO, y, i + 1, y[i + 89] + 1.3 + offset, zpos, 0.2f, 0.2f, 0.2f);
    }

    for (size_t i = 0; i < 9; i++)
    {
        DrawPiece(B, B, VAO, y, i + 1, y[i + 89] + offset, zpos+1, 0.8f, 0.2f, 0.8f);
    }
    for (size_t i = 0; i < 9; i++)
    {
        DrawPiece(B, B, VAO, y, i + 1, y[i + 89] + 0.6 + offset, zpos+1, 0.5f, 1.0f, 0.5f);
    }
}

void drawKing(Shader W, Shader B, GLuint VAO, GLfloat y[]) {
    int zpos = 2;
    float offset = 0.8;
    int xpos = 5;
    DrawPiece(W, B, VAO, y, 5, y[5 + 19] + 1.3 + offset, zpos - 1, 1, 1.3f, 0.5f);
    zpos = 8;
    DrawPiece(B, B, VAO, y, 5, y[5 + 89] + 1.3 +  offset, zpos + 1, 1, 1.3f, 0.5f);
    
}

void DrawQueen(Shader W, Shader B, GLuint VAO, GLfloat y[]) {
    int zpos = 2;
    float offset = 0.8;
    int xpos = 5;
    DrawPiece(W, B, VAO, y, 4, y[4 + 19] + 1.3 + offset, zpos - 1, 1, 0.8f, 0.5f);
    DrawPiece(W, B, VAO, y, 6, y[4 + 19] + 1.3 + offset, zpos - 1, 1, 0.8f, 0.5f);
    zpos = 8;
    DrawPiece(B, B, VAO, y, 6, y[6 + 89] + 1.3 + offset, zpos + 1, 1, 0.8f, 0.5f);
    DrawPiece(B, B, VAO, y, 4, y[6 + 89] + 1.3 + offset, zpos + 1, 1, 0.8f, 0.5f);

}

void DrawBishop(Shader W, Shader B, GLuint VAO, GLfloat y[]) {
    int zpos = 2;
    float offset = 0.8;
    int xpos = 5;
    DrawPiece(W, B, VAO, y, 3, y[3 + 19] + 1.3 + offset, zpos - 1, 0.2f, 0.3f, 0.4f);
    DrawPiece(W, B, VAO, y, 7, y[3 + 19] + 1.3 + offset, zpos - 1, 0.2f, 0.3f, 0.4f);
    zpos = 8;
    DrawPiece(B, B, VAO, y, 7, y[3 + 89] + 1.3 + offset, zpos + 1, 0.2f, 0.3f, 0.4f);
    DrawPiece(B, B, VAO, y, 3, y[3 + 89] + 1.3 + offset, zpos + 1, 0.2f, 0.3f, 0.4f);

}

void DrawRook(Shader W, Shader B, GLuint VAO, GLfloat y[]) {
    int zpos = 2;
    float offset = 0.8;
    int xpos = 5;
    DrawPiece(W, B, VAO, y, 2, y[3 + 19] + 1.3 + offset, zpos - 1, 0.1f, 0.4f, 0.7f);
    DrawPiece(W, B, VAO, y, 8, y[3 + 19] + 1.3 + offset, zpos - 1, 0.2f, 0.4f, 0.7f);
    zpos = 8;
    DrawPiece(B, B, VAO, y, 8, y[3 + 89] + 1.3 + offset, zpos + 1, 0.2f, 0.4f, 0.7f);
    DrawPiece(B, B, VAO, y, 2, y[3 + 89] + 1.3 + offset, zpos + 1, 0.2f, 0.4f, 0.7f);

}

void DrawPiece(Shader W, Shader B, GLuint VAO, GLfloat y[],int xpos,float ypos,int zpos,float xscale, float yscale, float zscale) {

    bool activeshader = true;

    Shader s = W;

    glm::mat4 projection(1.f);
    projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 10000.0f);

    // Create camera transformation
    glm::mat4 view(1.f);
    view = camera.GetViewMatrix();

    int i = 0;

    s.Use();

    // Get the uniform locations
    GLint modelLoc = glGetUniformLocation(s.Program, "model");
    GLint viewLoc = glGetUniformLocation(s.Program, "view");
    GLint projLoc = glGetUniformLocation(s.Program, "projection");

    // Pass the matrices to the shader
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    //cubes
    glBindVertexArray(VAO);
    // Calculate the model matrix for each object and pass it to shader before drawing
    glm::mat4 model(1.f);
    model = glm::translate(model, glm::vec3(xpos, ypos, zpos));
    model = glm::scale(model, glm::vec3(xscale, yscale, zscale));
    i++;
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, 36);

}

void drawBoard(Shader W, Shader B,Shader G, GLuint VAO, GLfloat y[], GLuint texture)
{
    bool activeshader = true;
    bool uniqueShader = false;
    Shader s = W;

    glm::mat4 projection(1.f);
    projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 10000.0f);

    // Create camera transformation
    glm::mat4 view(1.f);
    view = camera.GetViewMatrix();

    //creating cube board
    int i = 0;
    for (GLuint x = 0; x < 11; x++)
    {
        if (activeshader == false) {
            s = W;
            activeshader = false;
        }
        else
        {
            activeshader = true;
            s = B;
        }
        
        for (GLuint z = 0; z < 11; z++) {
            if (activeshader == true) {
                s = W;
                activeshader = false;
            }
            else
            {
                activeshader = true;
                s = B;
            };
            if (z == 0 || z == 10) {
                s = G;
                uniqueShader = true;
                y[i] = 0;
                glScalef(2.0, 1.0, 1.0);

            }
            if (x == 0 || x == 10) {
                s = G;
                y[i] = 0;
                glScalef(10.0, 10.0, 10.0);

            }

            s.Use();
            // Get the uniform locations
            GLint modelLoc = glGetUniformLocation(s.Program, "model");
            GLint viewLoc = glGetUniformLocation(s.Program, "view");
            GLint projLoc = glGetUniformLocation(s.Program, "projection");

            if (uniqueShader == true) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture);
                glUniform1i(glGetUniformLocation(s.Program, "ourTexture1"), 0);
               
            }
            
            // Pass the matrices to the shader
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
            //cubes
            glBindVertexArray(VAO);
            // Calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model(1.f);
            model = glm::translate(model, glm::vec3(x,y[i], z));
            i++;
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
            
        }

    }
    
    glUseProgram(0);
}

void drawTerrain(Shader &s,  GLuint VAO, const unsigned int NUM_STRIPS, const unsigned int NUM_VERTS_PER_STRIP)
{


    glm::mat4 projection(1.f);
    projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 10000.0f);

    glm::mat4 view(1.f);
    view = camera.GetViewMatrix();

    s.Use();
   ;
    
    GLint modelLoc = glGetUniformLocation(s.Program, "model");
    GLint viewLoc = glGetUniformLocation(s.Program, "view");
    GLint projLoc = glGetUniformLocation(s.Program, "projection");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    
    glBindVertexArray(VAO);
    glm::mat4 model(1.f);
    model = glm::translate(model, glm::vec3(0, -50, 0));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    
    for (unsigned int strip = 0; strip < NUM_STRIPS; ++strip)
    {
        glDrawElements(GL_TRIANGLE_STRIP,   // primitive type
            NUM_VERTS_PER_STRIP, // number of indices to render
            GL_UNSIGNED_INT,     // index data type
            (void*)(sizeof(unsigned int)
                * NUM_VERTS_PER_STRIP
                * strip)); // offset to starting index
    }
   
    glUseProgram(0);
}

//
//void drawTerraintwo(Shader& s, GLuint VAO2, const unsigned int NUM_STRIPS, const unsigned int NUM_VERTS_PER_STRIP)
//{
//    glm::mat4 projection(1.f);
//    projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 10000.0f);
//
//    glm::mat4 view(1.f);
//    view = camera.GetViewMatrix();
//
//    s.Use();
//
//    GLint modelLoc = glGetUniformLocation(s.Program, "model");
//    GLint viewLoc = glGetUniformLocation(s.Program, "view");
//    GLint projLoc = glGetUniformLocation(s.Program, "projection");
//
//    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
//    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
//
//    glBindVertexArray(VAO2);
//    glm::mat4 model(1.f);
//    model = glm::translate(model, glm::vec3(0, -50, 0));
//    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//
//    for (unsigned int strip = 0; strip < NUM_STRIPS; ++strip)
//    {
//        glDrawElements(GL_TRIANGLE_STRIP,   // primitive type
//            NUM_VERTS_PER_STRIP, // number of indices to render
//            GL_UNSIGNED_INT,     // index data type
//            (void*)(sizeof(unsigned int)
//                * NUM_VERTS_PER_STRIP
//                * strip)); // offset to starting index
//    }
//
//    glUseProgram(0);
//}

unsigned int loadCubemap(string faces[]) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrComponents;
    for (unsigned int i = 0; i < 6; i++)
    {
        unsigned char *data = SOIL_load_image(faces[i].c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            std::cout << "cube map face :  " << i << faces[i] << std::endl;
            SOIL_free_image_data(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            SOIL_free_image_data(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}


// Moves/alters the camera positions based on user input
void DoMovement()
{
    // Camera controls
    if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
    {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }

    if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
    {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }

    if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
    {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }

    if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
    {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
}

// Is called whenever a key is pressed/released via GLFW
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
        {
            keys[key] = true;
        }
        else if (action == GLFW_RELEASE)
        {
            keys[key] = false;
        }
    }
}

void MouseCallback(GLFWwindow* window, double xPos, double yPos)
{
    if (firstMouse)
    {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }

    GLfloat xOffset = xPos - lastX;
    GLfloat yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left

    lastX = xPos;
    lastY = yPos;

    camera.ProcessMouseMovement(xOffset, yOffset);
}


void ScrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
    camera.ProcessMouseScroll(yOffset);
}
