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
void DrawPiece(Shader W, Shader B, GLuint VAO, GLfloat y[], int xpos, float ypos, int zpos, float xscale, float yscale, float zscale);
void drawPawn(Shader W, Shader B, GLuint VAO, GLfloat y[]);
void drawKing(Shader W, Shader B, GLuint VAO, GLfloat y[]);
void DrawQueen(Shader W, Shader B, GLuint VAO, GLfloat y[]);
void DrawBishop(Shader W, Shader B, GLuint VAO, GLfloat y[]);
void DrawRook(Shader W, Shader B, GLuint VAO, GLfloat y[]);



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
    //y rand position for the board
    GLfloat ypos[121];

    for (size_t i = 0; i < 121; i++)
    {

        ypos[i] = ((float)rand() / 100000);

    }

    //vertices ++ indices for terrain
    int width, height, nChannels;
    unsigned char* data = SOIL_load_image("res/heightmaps/Hightmap50x50.png", &width, &height, &nChannels, 0);

    if (data)
    {
        std::cout << "Loaded heightmap of size " << height << " x " << width << std::endl;
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    // vertex generation
    std::vector<float> Hvertices;
    float yScale = 64.0f / 256.0f, yShift = 16.0f;  // apply a scale+shift to the height data
    for (unsigned int i = 0; i < height; i++)
    {
        for (unsigned int j = 0; j < width; j++)
        {
            // retrieve texel for (i,j) tex coord
            unsigned char* texel = data + (j + width * i) * nChannels;
            // raw height at coordinate
            unsigned char y = texel[0];

            // vertex
            Hvertices.push_back(-height / 2.0f + i);        // v.x
            Hvertices.push_back((int)y * yScale - yShift);  // v.y
            Hvertices.push_back(-width / 2.0f + j);        // v.z
        }
    }
    std::cout << "Loaded " << Hvertices.size() / 3 << " vertices" << std::endl;
    SOIL_free_image_data(data);

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
    unsigned char* image = SOIL_load_image("res/images/image1.jpg", &hwidth, &hheight, 0, SOIL_LOAD_RGBA);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, hwidth, hheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
   
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
        
        drawBoard(WhiteColor,BlackColor,grayColor, VAO, ypos,texture);

        drawTerrain(HightmapShader, terrainVAO, NUM_STRIPS, NUM_VERTS_PER_STRIP);

        drawPawn(grayColor, BlackColor, VAO, ypos);
        drawKing(grayColor, BlackColor, VAO, ypos);
        DrawQueen(grayColor, BlackColor, VAO, ypos);
        DrawBishop(grayColor, BlackColor, VAO, ypos);
        DrawRook(grayColor, BlackColor, VAO, ypos);




        glfwSwapBuffers(window);
        
    }
    // Properly de-allocate all resources once they've outlived their purpose
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &terrainVAO);
    glDeleteBuffers(1, &terrainVBO);
    glDeleteBuffers(1, &terrainEBO);
    
    glfwTerminate();

    return EXIT_SUCCESS;
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

void drawTerrain(Shader &s, GLuint VAO, const unsigned int NUM_STRIPS, const unsigned int NUM_VERTS_PER_STRIP)
{
    glm::mat4 projection(1.f);
    projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 10000.0f);

    glm::mat4 view(1.f);
    view = camera.GetViewMatrix();

    s.Use();
    
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
