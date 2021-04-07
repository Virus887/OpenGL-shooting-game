#include <glad/glad.h>
#include <GLFW/glfw3.h>


#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "Shader.h"
#include "Camera.h"
#include "Model.h"


#include <iostream>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window, Shader* shader);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadTexture(char const* path);

// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
float lastX = SCR_WIDTH/2;
float lastY = SCR_HEIGHT/2;

Camera ourCamera;

float fov = 45.0f;

bool firstMouse = true;

bool reverse_texture = false;

bool zoomMode = false;

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

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

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    stbi_set_flip_vertically_on_load(reverse_texture);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader* GouraudShader = new Shader("./Shaders/phong.vs", "./Shaders/phong.fs");

    Shader lightShader("./Shaders/light.vs", "./Shaders/light.fs");


    ourCamera = Camera();
    ourCamera.Front = glm::vec3(0.5f, 1.5f, 0.5f);

    //Model ourModel("./Objects/backpack/backpack.obj");
    Model board("./Objects/board/board.obj");
    //Model house("./Objects/house/house.obj");
    Model awp("./Objects/lamp/lamp.obj");
    Model bullet("./Objects/bullet/bullet.obj");


    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };
    // world space positions of our cubes
    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

        // positions of the point lights
        glm::vec3 pointLightPositions[] = {
            glm::vec3(0.7f,  1.2f,  2.0f),
            glm::vec3(2.3f,  3.3f, -4.0f),
            glm::vec3(-4.0f,  2.0f, -12.0f),
            glm::vec3(0.0f,  1.0f, -3.0f)
    };



   
    unsigned int VBO, lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(lightVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    unsigned int scopeTexture = loadTexture("./Textures/scopeTexture.png");


    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    GouraudShader->use();
    GouraudShader->setInt("scopeTexture", 3);

    lightShader.use();
    lightShader.setVec3("lightColor", lightColor);

    ourCamera.Position = glm::vec3(15.0f, 1.35f, 15.0f);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        GouraudShader->use();
        GouraudShader->setVec2("viewPort", SCR_WIDTH, SCR_HEIGHT);
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window, GouraudShader);



        //lightColor = glm::vec3(sin(currentFrame), cos(currentFrame), 1.0f);
        //glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
        //glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);
        //GouraudShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        //GouraudShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f); // darken diffuse light a bit
        // render
        // ------


        glClearColor(0.05f, 0.03f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!


        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, scopeTexture);


        // activate shader
    

        // create transformations
        glm::mat4 projection = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        const float radius = 10.0f;
        float camX = sin(glfwGetTime()) * radius;
        float camZ = cos(glfwGetTime()) * radius;
        view = ourCamera.GetViewMatrix();
        // pass transformation matrices to the shader
        GouraudShader->setMat4("projection", projection); // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
        GouraudShader->setMat4("view", view);
        GouraudShader->setVec3("lightColor", lightColor);
        GouraudShader->setVec3("viewPos", ourCamera.Position);


        //wklei³em

        GouraudShader->setInt("texture1", 0);
        GouraudShader->setInt("material.diffuse", 1);
        GouraudShader->setInt("material.specular", 2);
        GouraudShader->setVec3("lightPos", lightPos);
        GouraudShader->setVec3("lightColor", lightColor);
        GouraudShader->setVec3("viewPos", ourCamera.Position);


        GouraudShader->setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        GouraudShader->setVec3("dirLight.ambient", 0.2f, 0.2f, 0.2f);
        GouraudShader->setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        GouraudShader->setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
        // point light 1
        GouraudShader->setVec3("pointLights[0].position", pointLightPositions[0]);
        GouraudShader->setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        GouraudShader->setVec3("pointLights[0].diffuse", 0.8f, 0.4f, 0.4f);
        GouraudShader->setVec3("pointLights[0].specular", 1.0f, 0.5f, 0.5f);
        GouraudShader->setFloat("pointLights[0].constant", 1.0f);
        GouraudShader->setFloat("pointLights[0].linear", 0.09);
        GouraudShader->setFloat("pointLights[0].quadratic", 0.032);
        // point light 2
        GouraudShader->setVec3("pointLights[1].position", pointLightPositions[1]);
        GouraudShader->setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
        GouraudShader->setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
        GouraudShader->setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        GouraudShader->setFloat("pointLights[1].constant", 1.0f);
        GouraudShader->setFloat("pointLights[1].linear", 0.09);
        GouraudShader->setFloat("pointLights[1].quadratic", 0.032);
        // point light 3
        GouraudShader->setVec3("pointLights[2].position", pointLightPositions[2]);
        GouraudShader->setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
        GouraudShader->setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
        GouraudShader->setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        GouraudShader->setFloat("pointLights[2].constant", 1.0f);
        GouraudShader->setFloat("pointLights[2].linear", 0.09);
        GouraudShader->setFloat("pointLights[2].quadratic", 0.032);
        // point light 4
        GouraudShader->setVec3("pointLights[3].position", pointLightPositions[3]);
        GouraudShader->setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
        GouraudShader->setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
        GouraudShader->setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
        GouraudShader->setFloat("pointLights[3].constant", 1.0f);
        GouraudShader->setFloat("pointLights[3].linear", 0.09);
        GouraudShader->setFloat("pointLights[3].quadratic", 0.032);
        // spotLight
        GouraudShader->setVec3("spotLight.position", ourCamera.Position);
        GouraudShader->setVec3("spotLight.direction", ourCamera.Front);
        GouraudShader->setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        GouraudShader->setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        GouraudShader->setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        GouraudShader->setFloat("spotLight.constant", 1.0f);
        GouraudShader->setFloat("spotLight.linear", 0.09);
        GouraudShader->setFloat("spotLight.quadratic", 0.032);
        GouraudShader->setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        GouraudShader->setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

        GouraudShader->setFloat("material.shininess", 32.0f);



        //GouraudShader->setVec3("light.position", lightPos);
        GouraudShader->setVec3("light.position", ourCamera.Position);
        GouraudShader->setVec3("light.direction", ourCamera.Front);
        GouraudShader->setFloat("light.cutOff", glm::cos(glm::radians(12.5f)));
        GouraudShader->setFloat("light.outerCutOff", glm::cos(glm::radians(17.5f)));

        GouraudShader->setFloat("light.constant", 1.0f);
        GouraudShader->setFloat("light.linear", 0.09f);
        GouraudShader->setFloat("light.quadratic", 0.032f);

        lightShader.use();
        lightShader.setMat4("projection", projection); // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
        lightShader.setMat4("view", view);

        GouraudShader->use();

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.35f, 0.0f));
        GouraudShader->setMat4("model", model);    
        board.Draw(*GouraudShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(10.0f, 0.0f, 10.0f));
        GouraudShader->setMat4("model", model);
        //house.Draw(*GouraudShader);

        
      
        //AWP
        model = glm::mat4(1.0f);
        model = glm::translate(model, ourCamera.Position - glm::normalize(ourCamera.Position - ourCamera.Front) / glm::vec3(6) - glm::vec3(0, 0.2f, 0));

        model = glm::scale(model, glm::vec3(0.05f));
        model = glm::rotate(model, -glm::radians(ourCamera.Yaw), glm::vec3(0.0, 1.0, 0.0));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
        GouraudShader->setMat4("model", model);
        if (!zoomMode) awp.Draw(*GouraudShader);



        //if (!zoomMode) awp.Draw(*GouraudShader);

        //BULLET
        model = glm::mat4(1.0f);
        model = glm::translate(model, ourCamera.Position - glm::normalize(ourCamera.Position - ourCamera.Front) / glm::vec3(3) - glm::vec3(0, 0.05f, 0));

        model = glm::scale(model, glm::vec3(0.01f));
        model = glm::rotate(model, -glm::radians(ourCamera.Yaw), glm::vec3(0.0, 1.0, 0.0));
        model = glm::rotate(model, -glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
        GouraudShader->setMat4("model", model);
        if (!zoomMode) bullet.Draw(*GouraudShader);



        lightShader.use(); 
        glBindVertexArray(lightVAO);
        for (int i = 0; i < 4; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.1f));
            lightShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }



        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------

float lastTimeProcess = 0;
void processInput(GLFWwindow* window, Shader* shader)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        ourCamera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        ourCamera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        ourCamera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        ourCamera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);


    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
    {
        float currentProcess = glfwGetTime();
        if (currentProcess - lastTimeProcess < 1)return;
        lastTimeProcess = currentProcess;
        if (zoomMode == false)
        {
            fov = 25.0f;
        }
        else
        {
            fov = 45.0f;
        }
        zoomMode = !zoomMode;

        shader->setBool("zoomMode", zoomMode);
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
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
    lastX = xpos;
    lastY = ypos;

    ourCamera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}

unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}