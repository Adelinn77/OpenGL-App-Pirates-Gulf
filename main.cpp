#if defined (__APPLE__)
#define GLFW_INCLUDE_GLCOREARB
#define GL_SILENCE_DEPRECATION
#else
#define GLEW_STATIC
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include <glm/glm.hpp> //core glm functionality
#include <glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <glm/gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"

#include <iostream>

// window
gps::Window myWindow;

// matrices
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normalMatrix;

// light parameters
glm::vec3 lightDir;
glm::vec3 lightColor;

// shader uniform locations
GLint modelLoc;
GLint viewLoc;
GLint projectionLoc;
GLint normalMatrixLoc;
GLint lightDirLoc;
GLint lightColorLoc;
GLint isSkyLoc;

// point light (NEW)
glm::vec3 pointLightPos = glm::vec3(-3.471f, 21.617f, -61.781f);
glm::vec3 pointLightColor = glm::vec3(15.0f, 14.0f, 10.0f);
glm::vec3 pointLightAtt = glm::vec3(1.0f, 0.02f, 0.001f);


GLint pointLightPosLoc;
GLint pointLightColorLoc;
GLint pointLightAttLoc;

GLint dirLightIntensityLoc;

GLint timeLoc;


GLint isFlatLoc;

// Shadows
GLuint shadowMapFBO;
GLuint depthMapTexture;

const unsigned int SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;

gps::Shader depthMapShader;


// camera
gps::Camera myCamera(
    glm::vec3(0.0f, 0.0f, 3.0f),
    glm::vec3(0.0f, 0.0f, -10.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));

GLboolean pressedKeys[1024];

bool firstMouse = true;
double lastX = 512, lastY = 384;
float mouseSensitivity = 0.05f;
bool mouseEnabled = true;
float pitchAcc = 0.0f;
float lastFrame = 0.0f;
float deltaTime = 0.0f;
float baseMoveSpeed = 36.0f;
float dirLightIntensity = 0.2f;
float dirLightStep = 0.05f;

// models
gps::Model3D teapot;
gps::Model3D scene;
gps::Model3D skydome;
gps::Model3D canon;
GLfloat angle = 0.0f;
glm::vec3 canonPos = glm::vec3(0.0f, 0.0f, 0.0f);
float canonScale = 1.0f;
float canonRotation = 0.0f;


gps::Shader myBasicShader;

void initFBO() {

    glGenFramebuffers(1, &shadowMapFBO);

    glGenTextures(1, &depthMapTexture);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete! Status: " << status << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLenum glCheckError_(const char* file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        std::string error;
        switch (errorCode) {
        case GL_INVALID_ENUM:
            error = "INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            error = "INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            error = "INVALID_OPERATION";
            break;
        case GL_OUT_OF_MEMORY:
            error = "OUT_OF_MEMORY";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            error = "INVALID_FRAMEBUFFER_OPERATION";
            break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void focusCallback(GLFWwindow* window, int focused) {
    if (!focused) {
        mouseEnabled = false;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        firstMouse = true;
        memset(pressedKeys, 0, sizeof(pressedKeys));
    }
}

void windowResizeCallback(GLFWwindow* window, int width, int height) {
    fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);

    projection = glm::perspective(glm::radians(45.0f),
        (float)width / (float)height, 1.0f, 20000.0f);

    myBasicShader.useShaderProgram();
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}


void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) pressedKeys[key] = true;
        else if (action == GLFW_RELEASE) pressedKeys[key] = false;
    }

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        mouseEnabled = !mouseEnabled;

        glfwSetInputMode(window, GLFW_CURSOR,
            mouseEnabled ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);

        firstMouse = true;
        pressedKeys[GLFW_KEY_W] = false;
        pressedKeys[GLFW_KEY_A] = false;
        pressedKeys[GLFW_KEY_S] = false;
        pressedKeys[GLFW_KEY_D] = false;
    }

    if (action == GLFW_PRESS || action == GLFW_REPEAT) {

        if (key == GLFW_KEY_UP) {
            dirLightIntensity += dirLightStep;
            if (dirLightIntensity > 1.5f) dirLightIntensity = 1.5f;
            myBasicShader.useShaderProgram();
            glUniform1f(dirLightIntensityLoc, dirLightIntensity);
            std::cout << "Dir intensity: " << dirLightIntensity << "\n";
        }

        if (key == GLFW_KEY_DOWN) {
            dirLightIntensity -= dirLightStep;
            if (dirLightIntensity < 0.0f) dirLightIntensity = 0.0f;
            myBasicShader.useShaderProgram();
            glUniform1f(dirLightIntensityLoc, dirLightIntensity);
            std::cout << "Dir intensity: " << dirLightIntensity << "\n";
        }

        if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            myBasicShader.useShaderProgram();
            glUniform1i(isFlatLoc, 1); 
        }
   
        if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            myBasicShader.useShaderProgram();
            glUniform1i(isFlatLoc, 0); 
        }

        if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            glPointSize(3.0f); 
            myBasicShader.useShaderProgram();
            glUniform1i(isFlatLoc, 0);
        }

        if (key == GLFW_KEY_4 && action == GLFW_PRESS) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            myBasicShader.useShaderProgram();
            glUniform1i(isFlatLoc, 0); 
        }
    }


}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (!mouseEnabled) return;

    if (firstMouse) {
        lastX = xpos; lastY = ypos;
        firstMouse = false;
    }

    double xoffset = xpos - lastX;
    double yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    float yawDelta = (float)(-xoffset * mouseSensitivity);
    float pitchDeltaRaw = (float)(yoffset * mouseSensitivity);

    float newPitch = pitchAcc + pitchDeltaRaw;
    if (newPitch > 89.0f) newPitch = 89.0f;
    if (newPitch < -89.0f) newPitch = -89.0f;

    float pitchDelta = newPitch - pitchAcc;
    pitchAcc = newPitch;

    myCamera.rotate(pitchDelta, yawDelta);

    view = myCamera.getViewMatrix();
    myBasicShader.useShaderProgram();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
}

void processMovement() {
    if (!mouseEnabled) return;

    float speed = baseMoveSpeed;
    if (pressedKeys[GLFW_KEY_LEFT_SHIFT])
        speed *= 3.0f;

    float camStep = speed * deltaTime;

    //CAMERA CONTROLS 
    if (pressedKeys[GLFW_KEY_W]) {
        myCamera.move(gps::MOVE_FORWARD, camStep);
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }
    if (pressedKeys[GLFW_KEY_S]) {
        myCamera.move(gps::MOVE_BACKWARD, camStep);
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }
    if (pressedKeys[GLFW_KEY_A]) {
        myCamera.move(gps::MOVE_LEFT, camStep);
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }
    if (pressedKeys[GLFW_KEY_D]) {
        myCamera.move(gps::MOVE_RIGHT, camStep);
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    // CANON CONTROLS 
    float objSpeed = speed * deltaTime;
    if (pressedKeys[GLFW_KEY_L]) canonPos.z -= objSpeed;
    if (pressedKeys[GLFW_KEY_J]) canonPos.z += objSpeed; 
    if (pressedKeys[GLFW_KEY_I]) canonPos.x -= objSpeed; 
    if (pressedKeys[GLFW_KEY_K]) canonPos.x += objSpeed; 

    
    if (pressedKeys[GLFW_KEY_Q]) canonRotation -= 1.0f;
    if (pressedKeys[GLFW_KEY_E]) canonRotation += 1.0f;

    if (pressedKeys[GLFW_KEY_MINUS]) {
        canonScale -= 0.001f;
        if (canonScale < 0.001f) canonScale = 0.001f; 
    }
    if (pressedKeys[GLFW_KEY_EQUAL]) {
        canonScale += 0.001f;
    }
}

void initOpenGLWindow() {
    myWindow.Create(1400, 900, "OpenGL Project Core");
    glfwSetInputMode(myWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void setWindowCallbacks() {
    glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
    glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
    glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
    glfwSetWindowFocusCallback(myWindow.getWindow(), focusCallback);
}

void initOpenGLState() {
    glClearColor(0.5f, 0.7f, 1.0f, 1.0f);
    glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS); 
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK); 
    glFrontFace(GL_CCW); 
}

void initModels() {
    scene.LoadModel("models/MainMap.obj");
    skydome.LoadModel("models/skydome.obj");
    canon.LoadModel("models/canonRight.obj");
}

void initShaders() {
    myBasicShader.loadShader(
        "shaders/basic.vert",
        "shaders/basic.frag");
}

void initUniforms() {
    myBasicShader.useShaderProgram();

    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");

    view = myCamera.getViewMatrix();
    viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");

    projection = glm::perspective(glm::radians(45.0f),
        (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
        1.0f, 20000.0f);
    projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    dirLightIntensityLoc = glGetUniformLocation(myBasicShader.shaderProgram, "dirLightIntensity");
    glUniform1f(dirLightIntensityLoc, dirLightIntensity);


    lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
    lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));


    lightColor = glm::vec3(1.0f, 1.0f, 1.0f); 
    lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

    pointLightPosLoc = glGetUniformLocation(myBasicShader.shaderProgram, "pointLightPos");
    pointLightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "pointLightColor");
    pointLightAttLoc = glGetUniformLocation(myBasicShader.shaderProgram, "pointLightAtt");

    glUniform3fv(pointLightPosLoc, 1, glm::value_ptr(pointLightPos));
    glUniform3fv(pointLightColorLoc, 1, glm::value_ptr(pointLightColor));
    glUniform3fv(pointLightAttLoc, 1, glm::value_ptr(pointLightAtt));

    isSkyLoc = glGetUniformLocation(myBasicShader.shaderProgram, "isSky");
    timeLoc = glGetUniformLocation(myBasicShader.shaderProgram, "time");
    isFlatLoc = glGetUniformLocation(myBasicShader.shaderProgram, "isFlat");
    glUniform1i(isFlatLoc, 0); 
}


glm::mat4 computeLightSpaceTrMatrix() {
    glm::mat4 lightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, 0.1f, 250.0f);

    glm::vec3 lightPos = glm::normalize(lightDir) * 60.0f; 
    glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);        
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::mat4 lightView = glm::lookAt(lightPos, center, up);

    return lightProjection * lightView;
}

void renderScene() {
    glm::mat4 lightSpaceTrMatrix = computeLightSpaceTrMatrix();

    depthMapShader.useShaderProgram();

    glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
        1, GL_FALSE, glm::value_ptr(lightSpaceTrMatrix));

    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    // A. Draw Scene for Shadows
    glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
        1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
    scene.Draw(depthMapShader);

    // B. Draw Canon for Shadows
    glm::mat4 modelCanon = glm::mat4(1.0f);

    modelCanon = glm::translate(modelCanon, canonPos);

    modelCanon = glm::rotate(modelCanon, glm::radians(canonRotation), glm::vec3(0.0f, 1.0f, 0.0f));

    modelCanon = glm::scale(modelCanon, glm::vec3(canonScale));

    glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
        1, GL_FALSE, glm::value_ptr(modelCanon));
    canon.Draw(depthMapShader);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // Reset Viewport to Window Dimensions
    int retina_width, retina_height;
    glfwGetFramebufferSize(myWindow.getWindow(), &retina_width, &retina_height);
    glViewport(0, 0, retina_width, retina_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    myBasicShader.useShaderProgram();
    glUniform1f(timeLoc, (float)glfwGetTime());

    glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "lightSpaceTrMatrix"),
        1, GL_FALSE, glm::value_ptr(lightSpaceTrMatrix));

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "shadowMap"), 3);

    // A. DRAW SCENE 
    glUniform1i(isSkyLoc, 0);

    glm::mat4 modelScene = glm::mat4(1.0f);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelScene));

    glm::mat3 normalMatrixScene = glm::mat3(glm::inverseTranspose(view * modelScene));
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrixScene));

    scene.Draw(myBasicShader);

    // B. DRAW CANON
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelCanon));

    glm::mat3 normalMatrixCanon = glm::mat3(glm::inverseTranspose(view * modelCanon));
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrixCanon));

    canon.Draw(myBasicShader);

    // C. DRAW SKYDOME
    glUniform1i(isSkyLoc, 1);

    // 1. Disable Culling: inside of the dome
    glDisable(GL_CULL_FACE);

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    glm::mat4 modelSky = glm::mat4(1.0f);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelSky));

    glm::mat3 normalMatrixSky = glm::mat3(glm::inverseTranspose(view * modelSky));
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrixSky));

    skydome.Draw(myBasicShader);

    //RESTORE STATE 
    glEnable(GL_CULL_FACE); 
    glUniform1i(isSkyLoc, 0); 
}

void cleanup() {
    myWindow.Delete();
}

int main(int argc, const char* argv[]) {

    try {
        initOpenGLWindow();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    initOpenGLState();
    initFBO();
    depthMapShader.loadShader("shaders/depthMap.vert", "shaders/depthMap.frag"); \
    initModels();
    initShaders();
    initUniforms();
    setWindowCallbacks();

    glCheckError();
    while (!glfwWindowShouldClose(myWindow.getWindow())) {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processMovement();
        renderScene();

        glfwPollEvents();
        glfwSwapBuffers(myWindow.getWindow());

        glCheckError();
    }

    cleanup();

    return EXIT_SUCCESS;
}
