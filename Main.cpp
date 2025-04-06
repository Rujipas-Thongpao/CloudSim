#include"imgui.h"
#include"imgui_impl_glfw.h"
#include"imgui_impl_opengl3.h"


#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include <math.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>

#include "shaderClass.h"
#include "CameraClass.h"

#include "Utils.h" 

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

Camera camera(glm::vec3(0.f, 0.f, 3.f), glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, 1.f, 0.f));
bool DEBUG = false;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

float periodicRand(int x, int y, int z) {
    return glm::sin(glm::dot(glm::vec3(x, y, z), glm::vec3(12.9898f, 78.233f, 45.164f))) * 0.5f + 0.5f;
}


int main()
{
    std::cout << "Hello Cloud simulation!" << std::endl;
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursorPosCallback(window, mouse_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    float quadVertices[] = {
        // positions   
        -1.0f,  1.0f,
        -1.0f, -1.0f,
         1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f
    };


    float cube_vertices[] = {
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f
    };

    Shader lightCubeShader = Shader("lightCubeVert.vs", "lightCubeFrag.fs");
    Shader quadShader = Shader("quadVert.vs", "quadFrag.fs");


    // TODO : create VAO, VBO object
    // Quad
    unsigned int QuadVAO;
    glGenVertexArrays(1, &QuadVAO);
    glBindVertexArray(QuadVAO);

    unsigned int QuadVBO;
    glGenBuffers(1, &QuadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, QuadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);



    // unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // lightcube object
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    unsigned int lightCubeVBO;
    glGenBuffers(1, &lightCubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, lightCubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    const int cellAmount = 10;
    // span -1 to 1
    float noiseBoxWidth = 2.0f;
    float noiseBoxHeight = 2.0f;

    glm::vec3 noise_offset[cellAmount][cellAmount][cellAmount];

    for (int x = 0; x < cellAmount; x++) {
        for (int y = 0; y < cellAmount; y++) {
            for (int z = 0; z < cellAmount; z++) {
                float rx = glm::linearRand(-.5f, .5f);
                float ry = glm::linearRand(-.5f, .5f);
                float rz = glm::linearRand(-.5f, .5f);
                noise_offset[x][y][z] = glm::vec3(rx, ry, rz);
            }
        }
    }


    // แตกตรงนี้! -> noiseResolution เยอะเกิน
    // TODO : move to compute shader.

    unsigned int noiseCube;
    const int noiseResolution = 50;
    const int cellResolution = noiseResolution / cellAmount;
    float data[noiseResolution][noiseResolution][noiseResolution];

    for (int x = 0; x < noiseResolution; x++) {
        for (int y = 0; y < noiseResolution; y++) {
            for (int z = 0; z < noiseResolution; z++) {
                glm::vec3 pos(x, y, z);

                int ix = floor((float(x) / noiseResolution) * cellAmount);
                int iy = floor((float(y) / noiseResolution) * cellAmount);
                int iz = floor((float(z) / noiseResolution) * cellAmount);

                float length = cellResolution; // maximum length of each should be.

                int ss[3] = { -1,0,1 };
                for (int i = 0; i < 3; i++) {
                    for (int j = 0; j < 3; j++) {
                        for (int k = 0; k < 3; k++) {

                            // get surround cell.
                            int six = (ix + ss[i] + cellAmount) % cellAmount;
                            int siy = (iy + ss[j] + cellAmount) % cellAmount;
                            int siz = (iz + ss[k] + cellAmount) % cellAmount;

                            glm::vec3 offset = noise_offset[six][siy][siz];
                            glm::vec3 centerBoxPosition = mults(glm::vec3(six, siy, siz), cellResolution) + 0.5f * cellResolution;

                            glm::vec3 pointPosition = centerBoxPosition + (mults(offset, cellResolution));
                            glm::vec3 delta = glm::abs(pointPosition - pos);
                            // we need this because if we point at cell 0 and next cell n-1, meanint the length is not correct! 0 |. |. |. | n-1 but we need n-1 | 0
                            glm::vec3 periodicDelta = glm::min(delta, glm::vec3(noiseResolution) - delta);
                            length = std::min(length, glm::length(periodicDelta));
                        }
                    }
                }
                // data[x][y][z] = random(glm::vec3(x,y,z));
                data[x][y][z] = 1.0f - length / cellResolution;
                // data[x][y][z] = 1.0f;
                // std::cout << data[x][y][z] << std::endl;
            }
        }
    }

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &noiseCube);
    glBindTexture(GL_TEXTURE_3D, noiseCube);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, noiseResolution, noiseResolution, noiseResolution, 0, GL_RED, GL_FLOAT, data);

    quadShader.use();
    quadShader.setInt("_cell_amount", cellAmount);
    quadShader.setFloat("_box_width", noiseBoxWidth);

	float radius = 0.6f;
    float absorption = 35.0f;
    float scatter= 35.0f;
    float cutTexture = 5.0f;
    bool powderFlag = true;
    float powderFactor = 2.0f;
    float powderOffset = 0.0f;
    float initialFbmAmplitude = 0.7f;
    int FbmOctave = 4;
    float noiseSize = 3.0f;
    float skyCol[4] = { 135.f/255.f, 206.f/255.f, 234.f/255.f , 1.0f};
    float topCol[4] = {1.0f,1.0f,1.0f,1.0f};
    float buttomCol[4] = { 0.0f, 0.0f,0.0f,1.0f};
    // float smoothEdge[2] = { 0.1f, 0.3f };
    //int innerEdge = 0.1f;
    //int outerEdge = 0.3f;

    // render loop
    // -----------
    bool isprint = false;
    while (!glfwWindowShouldClose(window))
    {
        // DEBUG TOGGLE
        if (DEBUG) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }

        // input
        // -----
        processInput(window);
		camera.processInput(window, DEBUG);

        // render
        // ------
        // glEnable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
        // glEnable(GL_STENCIL_TEST);

        glClearColor(skyCol[0], skyCol[1],skyCol[2],skyCol[3]);                   // state-setting // this is like configuration for 'glclear'
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // state-using


        // ImGui create new frame
        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

        // camera
        glm::vec3 cameraPos = camera.GetCameraPosition();
        glm::vec3 cameraFront = camera.GetLookPos();
        glm::mat4 proj = camera.GetPerspective();
        glm::mat4 view = camera.GetView();
        glm::mat4 model = glm::mat4(1.0);

        // light cube.
        glm::vec3 lightPos = glm::vec3(0.0f);
        glm::mat4 lightModel = glm::mat4(1.0);

        // lightModel = glm::rotate(lightModel,(float)glfwGetTime(), glm::vec3(1.f,0.f,0.f));
        lightModel = glm::translate(lightModel, glm::vec3(-5.0f, 5.f, 0.0f));
        lightModel = glm::scale(lightModel, glm::vec3(0.3f, 0.3f, 0.3f));

        lightPos = glm::vec3(lightModel * glm::vec4(glm::vec3(0.0f), 1.0f));


        // Render Light cube
        lightCubeShader.use();
        lightCubeShader.setMat4("model", lightModel);
        lightCubeShader.setMat4("view", view);
        lightCubeShader.setMat4("projection", proj);

        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindVertexArray(0);



        quadShader.use();
        quadShader.setVec3("cameraPosition", cameraPos);
        quadShader.setVec3("cameraFront", cameraFront);
        quadShader.setMat4("model", model);
        quadShader.setMat4("view", view);
        quadShader.setMat4("projection", proj);
        quadShader.setVec3("lightPosition", lightPos);
        quadShader.setInt("_noiseSampler", 0); // or with shader class
        quadShader.setFloat("_time", glfwGetTime() / 50.0f);
        quadShader.setFloat("_radius", radius);
        quadShader.setFloat("_absorption", absorption);
        quadShader.setFloat("_scatter", scatter);
        quadShader.setFloat("_cutTexture", cutTexture);
        // powder Parameter
        quadShader.setBool("_powderFlag", powderFlag);
        quadShader.setFloat("_powderFactor", powderFactor);
        quadShader.setFloat("_powderOffset", powderOffset);
        // FBM parameter
        quadShader.setFloat("_initialFbmAmplitude", initialFbmAmplitude);
        quadShader.setInt("_FbmOctave", FbmOctave);
        quadShader.setFloat("_noiseSize", noiseSize);
        quadShader.setVec4("_skyColor", glm::vec4(skyCol[0], skyCol[1],skyCol[2],skyCol[3]));
        quadShader.setVec4("_topColor", glm::vec4(topCol[0], topCol[1],topCol[2],topCol[3]));
        quadShader.setVec4("_buttomColor", glm::vec4(buttomCol[0], buttomCol[1],buttomCol[2],buttomCol[3]));



        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_3D, noiseCube);

        glBindVertexArray(QuadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);


        glBindVertexArray(0);


        ImGui::Begin("Cloud Simulation Parameter");
        if (ImGui::CollapsingHeader("Shape")) {
			ImGui::SliderFloat("Radius", &radius, 0.5f, 10.0f);
			ImGui::SliderFloat("Absorption", &absorption, 0.1f, 100.0f);
			ImGui::SliderFloat("Scatter", &scatter, 0.01f, 100.0f);
        }

        if (ImGui::CollapsingHeader("FBM")) {
			ImGui::SliderFloat("Cut Texture", &cutTexture, 1.0f, 10.0f);
			ImGui::SliderFloat("FBM initial amplitude", &initialFbmAmplitude, 0.3f, 0.9f);
			ImGui::SliderInt("FBM octave", &FbmOctave, 1, 7);
            ImGui::SliderFloat("Noise Size", &noiseSize, 1.0f, 20.0f);
			// ImGui::SliderFloat2("texture threshold", &smoothEdge[0], 0.01, 0.8);
        }

        if (ImGui::CollapsingHeader("Powder")) {
            ImGui::Checkbox("Powder flags", &powderFlag);
            ImGui::SliderFloat("Powder Factor", &powderFactor, 1.0f, 50.0f);
            ImGui::SliderFloat("Powder Offset", &powderOffset, 0.1f, 1.0f);
        }

        if (ImGui::CollapsingHeader("Color")) {
            ImGui::ColorEdit4("Sky Color",skyCol);
            ImGui::ColorEdit4("top Color",topCol);
            ImGui::ColorEdit4("buttom Color",buttomCol);
        }

        ImGui::End();


        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glDeleteBuffers(1, &QuadVBO);
    glDeleteVertexArrays(1, &QuadVAO);
    glDeleteBuffers(1, &lightCubeVBO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glfwTerminate();
    return 0;
}



bool able_to_toggle = true;
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    // DEBUG toggle

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && able_to_toggle) {
        DEBUG = !DEBUG;
        able_to_toggle = false;
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
        able_to_toggle = true;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    camera.mouse_callback(window, xpos, ypos, DEBUG);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.scroll_callback(window, xoffset, yoffset);
}

