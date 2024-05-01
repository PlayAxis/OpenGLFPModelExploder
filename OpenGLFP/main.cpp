#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader.h>
#include <camera.h>
#include <model.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 1080;
const unsigned int SCR_HEIGHT = 840;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float freezeFrame = 0.0f;
bool timeStop = false;

// models
vector<Model> modelVect;
vector<glm::mat4> matrixVect;
int modelNum = 0;

//Explosion settings
bool explodeStart = false;
bool shatterStart = false;

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Model Exploder", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader ourShader("shader.vs", "shader.fs", "gshader.gs");

    // load models
    // -----------
    Model fumoModel("resources/objects/remilia/remilia.obj");
    modelVect.push_back(fumoModel);
    glm::mat4 fumoMat = glm::mat4(1.0f);
    fumoMat = glm::rotate(fumoMat, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    fumoMat = glm::rotate(fumoMat, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    fumoMat = glm::translate(fumoMat, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
    fumoMat = glm::scale(fumoMat, glm::vec3(0.05f, 0.05f, 0.05f));
    matrixVect.push_back(fumoMat);

    Model zackModel("resources/objects/zack/Zack.obj");
    modelVect.push_back(zackModel);
    glm::mat4 zackMat = glm::mat4(1.0f);
    zackMat = glm::translate(zackMat, glm::vec3(0.0f, -0.9f, 0.0f)); // translate it down so it's at the center of the scene
    zackMat = glm::scale(zackMat, glm::vec3(0.01f, 0.01f, 0.01f));
    matrixVect.push_back(zackMat);

    Model backModel("resources/objects/backpack/backpack.obj");
    modelVect.push_back(backModel);
    glm::mat4 backMat = glm::mat4(1.0f);
    backMat = glm::translate(backMat, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
    backMat = glm::scale(backMat, glm::vec3(0.3f, 0.3f, 0.3f));
    matrixVect.push_back(backMat);

    Model susModel("resources/objects/sus/WhenTheImposterIsSus.obj");
    modelVect.push_back(susModel);
    glm::mat4 susMat = glm::mat4(1.0f);
    susMat = glm::translate(susMat, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
    susMat = glm::scale(susMat, glm::vec3(0.5f, 0.5f, 0.5f));
    matrixVect.push_back(susMat);

    Model mikuModel("resources/objects/miku/Miku.obj");
    modelVect.push_back(mikuModel);
    glm::mat4 mikuMat = glm::mat4(1.0f);
    mikuMat = glm::rotate(mikuMat, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    mikuMat = glm::translate(mikuMat, glm::vec3(-0.05f, -1.9f, -0.3f)); // translate it down so it's at the center of the scene
    mikuMat = glm::scale(mikuMat, glm::vec3(0.01f, 0.01f, 0.01f));
    matrixVect.push_back(mikuMat);

    //Initialize explosion parameters
    ourShader.setInt("exploding", 0);
    ourShader.setInt("shattering", 0);
    ourShader.setFloat("minY", -100.0);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());

        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        if (!timeStop) {
            freezeFrame = currentFrame;
        }

        // input
        // -----
        processInput(window);

        //Check explode inputs and start explosion if true
        if (explodeStart) {
            shatterStart = false;
            ourShader.setInt("exploding", 1);
        }
        else if (shatterStart) {
            ourShader.setInt("shattering", 1);
        }
        else {
            ourShader.setInt("exploding", 0);
            ourShader.setInt("shattering", 0);
        }

        // render
        // ------
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        //glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //configure transform 
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 1.0f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);

        model = matrixVect[modelNum];

        ourShader.use();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
        ourShader.setMat4("model", model);

        if (timeStop == true) {
            ourShader.setFloat("time", freezeFrame);
        }
        else {
            ourShader.setFloat("time", static_cast<float>(glfwGetTime()));
        }

        modelVect[modelNum].Draw(ourShader);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        if (glfwGetTime() > 1) {
            explodeStart = !explodeStart;
            glfwSetTime(0.0);
        }
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        if (glfwGetTime() > 1) {
            shatterStart = !shatterStart;
            glfwSetTime(0.0);
        }
    }

    if (timeStop) {
        if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS)
            freezeFrame += deltaTime;
        if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS)
            freezeFrame -= deltaTime;
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

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        if (timeStop) {
            glfwSetTime(freezeFrame);
            timeStop = false;
        }
        else {
            timeStop = true;
        }
    }

    if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
        modelNum++;
        if(modelNum >= modelVect.size()) {
            modelNum = 0;
        }
    }
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
        modelNum--;
        if(modelNum < 0) {
            modelNum = modelVect.size() - 1;
        }
    }
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

