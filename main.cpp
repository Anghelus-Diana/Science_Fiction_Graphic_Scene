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
#include "SkyBox.hpp"
#include "Model3D.hpp"
#include <irrKlang.h>
using namespace irrklang;

#include <iostream>

// window
gps::Window myWindow;

// matrices
glm::mat4 model;

glm::mat4 originlModelHand = model;
glm::mat4 originlModelPanou = model;
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

// camera
gps::Camera myCamera(
    glm::vec3(90.0f, 8.0f, 9.0f),
    glm::vec3(0.0f, 0.0f, -10.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));


//Mouse
bool firstMouse = true;
float lastX = 400, lastY = 300;
float pitch = 0.0f, yaw  = +180.0f;

GLfloat cameraSpeed = 0.1f;
GLboolean pressedKeys[1024];

//SkyBox
gps::SkyBox mySkyBox;
gps::Shader skyboxShader;

//-----------------------------------------------------------------Fog----------------------------------------------------------------------------
int activateFog = 0;


//--------------------------------------------------------------PointLight------------------------------------------------------------------------
int activatePointLight = 0;
glm::vec3 luminaUfoPos = glm::vec3(18.589f, -19.837f, 11.224f);


//---------------------------------------------------------------SpotLight-------------------------------------------------------------------------
float spotlight1;
float spotlight2;

glm::vec3 spotLightDirection;
glm::vec3 spotLightPosition;
int activateSpotLight = 1;


//-----------------------------------------------------------------Sounds-------------------------------------------------------------------------
irrklang::ISoundEngine* engine;
irrklang::ISound* sarbaSound;
irrklang::ISound* crowdSound;
irrklang::ISound* roarSound;


//-----------------------------------------------------------------Animatii------------------------------------------------------------------------
//Ball
float deltaBall=0;
bool moveUpBall = false;

//dinoHand
GLfloat angle;
GLfloat angle2;
GLfloat handAngle = 0.0f;
const float handRotationSpeed = 450.0f;

//dinoMouth
GLfloat mouthAngle = 0.0f;
const float mouthRotationSpeed = 130.0f;
float deltaMouth = 0;


//panou
GLfloat panouAngle = 0.0f;
float deltaPanou = 0.0f;
const float panouRotationSpeed = 50.0f;
bool stopPanou=true;


//UFO
GLfloat deltaUfo =0.0f;

//tank
GLfloat deltaTank1 = 0.0f;

//PeretiOpening
GLfloat deltaPereteSus = 0.0f;
GLfloat deltaPereteJos = 0.0f;

//Presentation
bool startPresentation = false;
GLfloat deltaFrontPresentation = 0.0f;

bool startParty = false;
GLfloat deltaParty = 0.0f;


//------------------------------------------------------------------------- Models----------------------------------------------------------------
gps::Model3D scene;
gps::Model3D ground;
gps::Model3D trotuar;
gps::Model3D felinar1;
gps::Model3D felinar2;
gps::Model3D ball;
gps::Model3D panou;
gps::Model3D tank1;
gps::Model3D tank2;
gps::Model3D tank3;
gps::Model3D dinoBody;
gps::Model3D dinoHand;
gps::Model3D ufo;
gps::Model3D dinoMouth;
gps::Model3D dinoBodyWithoutMouth;
gps::Model3D pereteSus;
gps::Model3D pereteJos;

glm::mat4 handModel;
glm::vec3 handPozitie = glm::vec3(16.487f, 9.4703f, -9.7289f);

glm::mat4 panouModel;
glm::vec3 panouPozitie = glm::vec3(42.266f, -12.951f, -0.1393f);

glm::mat4 mouthModel;
glm::vec3 mouthPozitie = glm::vec3(17.867f, 11.187f, -6.12789f);


// shaders
gps::Shader myBasicShader;


//----------------------------------------------------------------------Sounds-----------------------------------------------------------------------

void initSoundEngine()
{
    engine = irrklang::createIrrKlangDevice();
    if (!engine)
        return; //eroare daca nu functioneaza
}

void playSarbaSound()
{
    sarbaSound = engine->play2D("media/Sarba.ogg", GL_TRUE);
}

void playCrowdSound()
{
    sarbaSound = engine->play2D("media/CrowdPanic.ogg", GL_TRUE);
}

void playRoarSound()
{
    roarSound = engine->play2D("media/roar.ogg", GL_TRUE);
}

void stopTutorialSound()
{
    engine->stopAllSounds();
}


GLenum glCheckError_(const char *file, int line)
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

void initSkybox() {
    std::vector<const GLchar*> faces;
    faces.push_back("skybox/posx.jpg");
    faces.push_back("skybox/negx.jpg");
    faces.push_back("skybox/posy.jpg");
    faces.push_back("skybox/negy.jpg");
    faces.push_back("skybox/posz.jpg");
    faces.push_back("skybox/negz.jpg");
    mySkyBox.Load(faces);
}

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);
	//TODO
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key == GLFW_KEY_C && action == GLFW_PRESS)
    {
        activateFog == 0 ? activateFog = 1 : activateFog = 0;
        myBasicShader.useShaderProgram();
        glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "activateFog"), activateFog);
    }

    if (key == GLFW_KEY_L && action == GLFW_PRESS)
    {
        activatePointLight == 0 ? activatePointLight = 1 : activatePointLight = 0;
        myBasicShader.useShaderProgram();
        glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "activatePointLight"), activatePointLight);
    }

    if (key == GLFW_KEY_F && action == GLFW_PRESS)
    {
        activateSpotLight == 0 ? activateSpotLight = 1 : activateSpotLight = 0;
        myBasicShader.useShaderProgram();
        glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "activateSpotLight"), activateSpotLight);
    }

    if (key == GLFW_KEY_V && action == GLFW_PRESS)
    {
        playSarbaSound();

    }

    if (key == GLFW_KEY_N && action == GLFW_PRESS)
    {
        playCrowdSound();

    }

    if (key == GLFW_KEY_M && action == GLFW_PRESS)
    {
        playRoarSound();

    }
    

     if(key == GLFW_KEY_B && action == GLFW_PRESS)
    {
        engine->stopAllSounds();
    }


	if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            pressedKeys[key] = true;
        } else if (action == GLFW_RELEASE) {
            pressedKeys[key] = false;
        }
    }
}

void cameraPresentation()
{
    if (deltaFrontPresentation < 42)
    {
        deltaFrontPresentation += 0.3;
        //store the original model matrix
        glm::mat4 originalModel = model;

        deltaPereteSus += 0.1f;
        deltaPereteJos -= 0.1f;
        deltaTank1 -= 0.2f;

        glm::mat4 pereteJosModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, deltaPereteJos, 0.0f));
        glm::mat4 pereteSusModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, deltaPereteSus, 0.0f));
        glm::mat4 tank1Model = glm::translate(glm::mat4(1.0f), glm::vec3(deltaTank1, 0.0f, 0.0f));
        model = pereteSusModel;

        // send new model matrix data to shader
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        // send new normal matrix data to shader
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

        model = originalModel;

    }
    else
        if (deltaFrontPresentation >= 42 && deltaFrontPresentation < 45)
        {
            deltaFrontPresentation += 0.1;
            myCamera.move(gps::MOVE_FORWARD, 1.2f);
            view = myCamera.getViewMatrix();
            myBasicShader.useShaderProgram();
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        }
    else 
        if (deltaFrontPresentation >= 45 && deltaFrontPresentation < 50)
        {
            deltaFrontPresentation += 0.1;
            angle += 0.6f;
            myCamera.rotate(180.0f, angle);
            view = myCamera.getViewMatrix();
            myBasicShader.useShaderProgram();
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            normalMatrix = glm::mat3(glm::inverseTranspose(view * model)); 
        }
        else
            if (deltaFrontPresentation >= 50 && deltaFrontPresentation < 62)
            {
                deltaFrontPresentation += 0.1;
                myCamera.move(gps::MOVE_BACKWARD, 0.1f);
                view = myCamera.getViewMatrix();
                myBasicShader.useShaderProgram();
                glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
                normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
            }
            else
            if (deltaFrontPresentation >= 62 && deltaFrontPresentation < 67)
            {
                deltaFrontPresentation += 0.1;
                //store the original model matrix
                glm::mat4 originalModel = model;

                deltaUfo += 0.1f;

                glm::mat4 ufoModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, deltaUfo, 0.0f));
                model = ufoModel;

                // send new model matrix data to shader
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

                // send new normal matrix data to shader
                glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

                model = originalModel;
            }
            else
                if (deltaFrontPresentation >= 67 && deltaFrontPresentation < 72)
                {
                    deltaFrontPresentation += 0.1;
                    //store the original model matrix
                    glm::mat4 originalModel = model;

                    deltaUfo -= 0.1f;

                    glm::mat4 ufoModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, deltaUfo, 0.0f));
                    model = ufoModel;

                    // send new model matrix data to shader
                    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

                    // send new normal matrix data to shader
                    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

                    model = originalModel;
                }
                else
                    if (deltaFrontPresentation >= 72 && deltaFrontPresentation < 78)
                    {
                        
                        deltaFrontPresentation += 0.1;
                        myCamera.move(gps::MOVE_FORWARD, 0.2f);
                        view = myCamera.getViewMatrix();
                        myBasicShader.useShaderProgram();
                        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
                        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

                    }
                    if (deltaFrontPresentation >= 78 && deltaFrontPresentation < 89)
                    {
                        deltaFrontPresentation += 0.1;
                        myCamera.move(gps::MOVE_BACKWARD, 0.2f);
                        view = myCamera.getViewMatrix();
                        myBasicShader.useShaderProgram();
                        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
                        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

                        //Light Show
                        activatePointLight == 0 ? activatePointLight = 1 : activatePointLight = 0;
                        myBasicShader.useShaderProgram();
                        glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "activatePointLight"), activatePointLight);

                        activateSpotLight == 0 ? activateSpotLight = 1 : activateSpotLight = 0;
                        myBasicShader.useShaderProgram();
                        glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "activateSpotLight"), activateSpotLight);

                    }
            
}

void partyAnimation()
{
    if (deltaParty < 300)
    {   
        deltaParty += 0.1;

        //Light Show
        activatePointLight == 0 ? activatePointLight = 1 : activatePointLight = 0;
        myBasicShader.useShaderProgram();
        glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "activatePointLight"), activatePointLight);

        activateSpotLight == 0 ? activateSpotLight = 1 : activateSpotLight = 0;
        myBasicShader.useShaderProgram();
        glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "activateSpotLight"), activateSpotLight);
      }
}

void swichVisualisation() {
    if (pressedKeys[GLFW_KEY_R])
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    if (pressedKeys[GLFW_KEY_T])
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    if (pressedKeys[GLFW_KEY_Y])
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    }
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.05f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    myCamera.rotate(pitch, yaw);
    myBasicShader.useShaderProgram();
    view = myCamera.getViewMatrix();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * lightDir));
}

void processMovement() {

    swichVisualisation();

	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, 4*cameraSpeed);
		//update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, 4*cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, 4*cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, 4*cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	}

    if (pressedKeys[GLFW_KEY_P]) {
        playRoarSound();
        playCrowdSound();
        startPresentation = true;
    }

    if (pressedKeys[GLFW_KEY_O]) {
        startPresentation = false;
    }

    if (pressedKeys[GLFW_KEY_H])
    {
        playSarbaSound();
        startParty = true;
    }

    if (pressedKeys[GLFW_KEY_X])
    {
        startParty = false;
    }


}

void initOpenGLWindow() {
    myWindow.Create(1920, 1000, "OpenGL Project Core");
}

void setWindowCallbacks() {
	glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
    glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
    glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
    glfwSetInputMode(myWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void initOpenGLState() {
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
	glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
}

void initModels() {
    
    scene.LoadModel("models/Scena_statica_fara_podele/Scena_statica_fara_podele.obj");
    ground.LoadModel("models/Ground/Ground.obj" );
    ball.LoadModel("models/ball/ball.obj");
    felinar1.LoadModel("models/Felinar1/felinar1.obj");
    felinar2.LoadModel("models/Felinar2/felinar2.obj");
    panou.LoadModel("models/panou/Panou.obj");
    trotuar.LoadModel("models/trotuar/trotuar.obj");
    tank1.LoadModel("models/tank1/tank1.obj");
    tank2.LoadModel("models/tank2/tank2.obj");
    tank3.LoadModel("models/tank3/tank3.obj");
    dinoHand.LoadModel("models/dinoHand/dinoHand.obj");
    ufo.LoadModel("models/ufo/ufo.obj");
    dinoMouth.LoadModel("models/dinoMouth/dinoMouth.obj");
    dinoBodyWithoutMouth.LoadModel("models/dinoBodyWithoutMouth/dinoBodyWithoutMouth.obj");
    pereteSus.LoadModel("models/pereteSus/pereteSus.obj");
    pereteJos.LoadModel("models/pereteJos/pereteJos.obj"); 
}

void initShaders() {
	myBasicShader.loadShader(
        "shaders/basic.vert",
        "shaders/basic.frag");

    //skybox
    skyboxShader.loadShader(
        "shaders/skyboxShader.vert", 
        "shaders/skyboxShader.frag");
    skyboxShader.useShaderProgram();

}

void initUniforms() {
	myBasicShader.useShaderProgram();

    // create model matrix for teapot
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");

	// get view matrix for current camera
	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
	// send view matrix to shader
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // compute normal matrix for teapot
    normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");

	// create projection matrix
	projection = glm::perspective(glm::radians(45.0f),
                               (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
                               0.1f, 2000.0f);
	projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
	// send projection matrix to shader
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));	

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
	lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");
	// send light dir to shader
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

	//set light color
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
	// send light color to shader
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

    // spotlight
    spotlight1 = glm::cos(glm::radians(40.5f));
    spotlight2 = glm::cos(glm::radians(100.5f));

    spotLightDirection = glm::vec3(0, -1, 0);
    spotLightPosition = glm::vec3(42.266f, 10.0f, 12.0f);

    // setup a material for the scene
    glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "material.shininess"), 100.0f);


    //------------------------------------------------------------DirLight-----------------------------------------------------------------------
    glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "dirLight.direction"), 1, glm::value_ptr(lightDir));
    glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "dirLight.ambient"), 0.2f, 0.2f, 0.2f);
    glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "dirLight.diffuse"), 0.5f, 0.5f, 0.5f);
    glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "dirLight.specular"), 1.0f, 1.0f, 1.0f);

    //------------------------------------------------------------PointLight----------------------------------------------------------------------
    glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight.position"), 1, glm::value_ptr(luminaUfoPos));
    glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight.constant"), 1.0f);
    glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight.linear"), 0.09f);
    glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight.quadratic"), 0.032f);
    glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight.ambient"), 0.2f, 0.2f, 0.2f);
    glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight.diffuse"), 0.5f, 0.5f, 0.5f);
    glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight.specular"), 1.0f, 1.0f, 1.0f);


    //----------------------------------------------------------SpotLight-----------------------------------------------------------------------------
    glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "spotlight1"), spotlight1);
    glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "spotlight2"), spotlight2);
    glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "spotLightDirection"), 1, glm::value_ptr(spotLightDirection));
    glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "spotLightPosition"), 1, glm::value_ptr(spotLightPosition));

    //----------------------------------------------------------ActivateLights-------------------------------------------------------------------------
    glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "activateSpotLight"), activateSpotLight);
    glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "activatePointLight"), activatePointLight);

 
}



void renderTeapot(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    //send teapot model matrix data to shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    //send teapot normal matrix data to shader
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    // draw static scene
    scene.Draw(shader);
    ground.Draw(shader);
    felinar1.Draw(shader);
    felinar2.Draw(shader);
    trotuar.Draw(shader);
    panou.Draw(shader);
    tank2.Draw(shader);
    tank3.Draw(shader);
    dinoBodyWithoutMouth.Draw(shader);
}

void renderBall(gps::Shader shader)
{   //select active shader program
    shader.useShaderProgram();

    //store the original model matrix
    glm::mat4 originalModel = model;

    if (moveUpBall)
    {
        deltaBall += 0.1;
        if (deltaBall > 8.0) {
            //Daca ajunge sus,schimb directia
            moveUpBall = false;
        }
    }
    else
    {
        deltaBall -= 0.1;
        if (deltaBall < 0.0) {
            //Daca revine la pozitia initiala schimbam iar directia
            moveUpBall = true;
            //model = originalModel;
        }
    }

    //update model matrix fofr ball
    glm::mat4 ballModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, deltaBall, 0.0f));
    model = ballModel;

    // send ball model matrix data to shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    // send ball normal matrix data to shader
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    //draw ball
    ball.Draw(shader);
    model = originalModel;

}

void renderPereteSus(gps::Shader shader)
{   //select active shader program
    shader.useShaderProgram();

    //store the original model matrix
    glm::mat4 originalModel = model;

    if (pressedKeys[GLFW_KEY_1]) {
        deltaPereteSus += 0.1f;
    }
    else
        if (pressedKeys[GLFW_KEY_2])
        {
            deltaPereteSus -= 0.1f;
        }

    glm::mat4 pereteSusModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,deltaPereteSus, 0.0f));
    model = pereteSusModel;

    // send pereteSus model matrix data to shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    // send pereteSus normal matrix data to shader
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    //draw pereteSus only is presentation started
    if (startPresentation)
    pereteSus.Draw(shader);

    model = originalModel;

}

void renderPereteJos(gps::Shader shader)
{   //select active shader program
    shader.useShaderProgram();

    //store the original model matrix
    glm::mat4 originalModel = model;

    if (pressedKeys[GLFW_KEY_3]) {
        deltaPereteJos += 0.1f;
    }
    else
        if (pressedKeys[GLFW_KEY_4])
        {
            deltaPereteJos -= 0.1f;
        }

    glm::mat4 pereteJosModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, deltaPereteJos, 0.0f));
    model = pereteJosModel;

    // send pereteJos model matrix data to shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    // send pereteJos normal matrix data to shader
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));


    //draw pereteJos only is presentation started
    if (startPresentation)
    pereteJos.Draw(shader);

    model = originalModel;

}

void renderTank1(gps::Shader shader)
{
    //select active shader program
    shader.useShaderProgram();

    //store the original model matrix
    glm::mat4 originalModel = model;

    if (pressedKeys[GLFW_KEY_J] ) {
        deltaTank1+= 0.1f;
    }
    else
        if (pressedKeys[GLFW_KEY_K])
        {
            deltaTank1 -= 0.1f;
        }

    glm::mat4 tank1Model = glm::translate(glm::mat4(1.0f), glm::vec3(deltaTank1, 0.0f, 0.0f));
    model = tank1Model;

    // send tank1 model matrix data to shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    // send tank1 normal matrix data to shader
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));


    //draw tank1
    tank1.Draw(shader);

    model = originalModel;




}


void renderUfo(gps::Shader shader)
{
    //select active shader program
    shader.useShaderProgram();

    //store the original model matrix
    glm::mat4 originalModel = model;

    if (pressedKeys[GLFW_KEY_U] && deltaUfo<3) {
        deltaUfo += 0.1f;
    }
    else
    if (pressedKeys[GLFW_KEY_I] && deltaUfo > -4)
    {
        deltaUfo -= 0.1f;
    }

    glm::mat4 ufoModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, deltaUfo, 0.0f));
    model = ufoModel;

    // send ufo model matrix data to shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    // send ufo normal matrix data to shader
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));


    //draw ball
    ufo.Draw(shader);

    model = originalModel;

}


void renderHand(gps::Shader shader,float delta)
{
    //select active shader program
    shader.useShaderProgram();

    //store the original model matrix
    glm::mat4 originalModelHand = model;

    handAngle += handRotationSpeed * delta;

    handModel = glm::mat4(1.0f);

    // Translate the hand back to its original position
    handModel = glm::translate(handModel, handPozitie);
    //Rotate
    handModel = glm::rotate(handModel, glm::radians(handAngle), glm::vec3(1.0f, 0.0f, 0.0f));
    // Translate the hand to the origin (0, 0, 0)
    handModel = glm::translate(handModel, -handPozitie);

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(handModel));
    dinoHand.Draw(shader);
}

void renderMouth(gps::Shader shader, float delta)
{
    //select active shader program
    shader.useShaderProgram();

    //store the original model matrix
    glm::mat4 originalModelMouth = model;

    if (deltaMouth < 1.5)
    {
        mouthAngle -= mouthRotationSpeed * delta;

        mouthModel = glm::mat4(1.0f);

        deltaMouth += 0.1f;

        // Translate the mouth back to its original position
        mouthModel = glm::translate(mouthModel, mouthPozitie);
        //Rotate
        mouthModel = glm::rotate(mouthModel, glm::radians(mouthAngle), glm::vec3(1.0f, 0.0f, 0.0f));
        // Translate the mouth to the origin (0, 0, 0)
        mouthModel = glm::translate(mouthModel, -mouthPozitie);
        
    }
    else if (deltaMouth >= 1.5 && deltaMouth < 3)
    {
        mouthAngle += mouthRotationSpeed * delta;

        mouthModel = glm::mat4(1.0f);

        deltaMouth += 0.1f;

        // Translate the hand back to its original position
        mouthModel = glm::translate(mouthModel, mouthPozitie);
        //Rotate
        mouthModel = glm::rotate(mouthModel, glm::radians(mouthAngle), glm::vec3(1.0f, 0.0f, 0.0f));
        // Translate the hand to the origin (0, 0, 0)
        mouthModel = glm::translate(mouthModel, -mouthPozitie);
    }
    else
        if (deltaMouth >= 3)
        {
            deltaMouth = 0;
            mouthAngle = 0;
        }

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mouthModel));
        dinoMouth.Draw(shader);
    
}


void renderPanou(gps::Shader shader,float delta) {
    //select active shader program
    shader.useShaderProgram();

    panouAngle += panouRotationSpeed * delta;
    panouModel = glm::mat4(1.0f);

    //store the original model matrix
    glm::mat4 originalModelPanou = model;
    if (stopPanou) {
        panouModel = glm::translate(panouModel, panouPozitie);
        panouModel = glm::rotate(panouModel, glm::radians(panouAngle),glm::vec3(0.0f, 0.0f, 1.0f));
        panouModel = glm::translate(panouModel, -panouPozitie);
        deltaPanou += 1.0;
        if (deltaPanou>50)
            stopPanou = false;

    }

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(panouModel));
    panou.Draw(shader);


}

void renderScene(float delta) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
	//render the scene
 
    renderBall(myBasicShader);
    renderHand(myBasicShader,delta);
	renderTeapot(myBasicShader);
    renderUfo(myBasicShader);
    renderTank1(myBasicShader);
    renderMouth(myBasicShader,delta);
    renderPereteSus(myBasicShader);
    renderPereteJos(myBasicShader);

    //Render camera animation
    if (startPresentation )
    {
        cameraPresentation();
    }

    if (startParty)
    {
        partyAnimation();
    }
    mySkyBox.Draw(skyboxShader, view, projection);
}

void cleanup() {
    myWindow.Delete();
    //cleanup code for your own data
    engine->drop();
}

int main(int argc, const char * argv[]) {

   
    try {
        initOpenGLWindow();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    initOpenGLState();
	initModels();
	initShaders();
    initSkybox();
	initUniforms();
    setWindowCallbacks();

    //init sound engine
    initSoundEngine();
    
	glCheckError();
	// application loop
	while (!glfwWindowShouldClose(myWindow.getWindow())) {
        
        static float lastTime = glfwGetTime();
        float currentTime = glfwGetTime();
        float deltaT = currentTime - lastTime;
        lastTime = currentTime;

        
        processMovement();
	    renderScene(deltaT);

		glfwPollEvents();
		glfwSwapBuffers(myWindow.getWindow());

		glCheckError();
	}

	cleanup();

    return EXIT_SUCCESS;
}
