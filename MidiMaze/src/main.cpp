#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <string.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Bullets.h>
#include <EnemyBullets.h>
#include <map.h>
#include <wall.h>
#include <Enemies.h>
#include <Hud.h>
#include <sprite_renderer.h>

#include <Camera.h>
#include <model.h>

#include <iostream>

#include <assimp/scene.h>

#include <glut/glut.h>
#include <DevIL/il.h>
#include <irrKlang/irrKlang.h>

using namespace irrklang;

bool newBullet = false;

//ISoundEngine* SoundEngine = createIrrKlangDevice(); // to manage the sound effects

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void drawHollowCircle(float cx, float cy, float r, int num_segments);

void showFPS(int fps);

// Initial settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f; // Initial values in the middle of the screen
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// valores para normalizar el movimiento de la c�mara
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

float lastFrameFPS = 0.0f;
int countFrames = 0; // Para saber los frames que ha habido en 1s

unsigned int currentDelay = 0; // Delay inicializado a 0. Cuando se dispare, se pondrá un contador u otro dependiendo de la cadencia
int vidas = 3;

Map temp;

// ###Constantes varias###
const bool versionModerna = true;

SpriteRenderer* Renderer;




int main()
{
    
    // =====================================================================================================================
    // =================================== INITIALIZATION AND CONFIGURATION OF LIBRARIES ===================================
    // =====================================================================================================================
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // If in an Apple environment, declare this thing below
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Midi Maze :)", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwSetMouseButtonCallback(window, mouse_button_callback);
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

    //Model tests;

    // =====================================================================================================================
    // ================================== CONFIGURATION OF SHADERS, SHAPES AND TEXTURES ====================================
    // =====================================================================================================================

    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("shaders/1.model_loading.vs", "shaders/1.model_loading.fs");


    Shader spriteShader("shaders/sprite.vs", "shaders/sprite.vs");

    glm::mat4 projectionSprite = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH),
        static_cast<float>(SCR_HEIGHT), 0.0f, -1.0f, 1.0f);

    spriteShader.use();
    spriteShader.setMat4("projection", projectionSprite);

   
    Renderer = new SpriteRenderer();
    Renderer->setShader(&spriteShader);

    int texSprite = TextureFromFile("awesomeface.png", "resources/textures");

    glLoadIdentity();
    gluPerspective(60, (GLfloat)SCR_WIDTH / (GLfloat)SCR_HEIGHT, 1.0, 100.0);

    glBegin(GL_QUADS);
    glVertex2d(400.0, 100.0);
    glVertex2d(400.0, 500.0);
    glVertex2d(700.0, 500.0);
    glVertex2d(700.0, 100.0);
    glEnd();

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);    //Capturar el rat�n
    glfwSetCursorPosCallback(window, mouse_callback);


    Bullet myBullets("resources/objects/bullets/yellow/yellow.obj", 0.1);
	vector<EnemBullet> enemyBullets;

    Map pared("resources/maps/originalMap.txt", ourShader);

<<<<<<< Updated upstream
    Enemy myEnemies(0.5, 10, pared.getLab(), pared, pared.getDim());
=======
    Enemy myEnemies(0.5, 1, pared.getLab(), pared.getDim());

>>>>>>> Stashed changes
    // =====================================================================================================================
    // ==================================================== RENDER LOOP ====================================================
    // =====================================================================================================================
    while (!glfwWindowShouldClose(window))
    {
        temp = pared;

        // Datos para gestionar los fps
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
		
        countFrames++; // Actualizar los frames en el �ltimo segundo
		// Datos para gestionar los fps
        if (currentFrame - lastFrameFPS > 1.0f) {
            //cout << "FPS: " << countFrames << endl;
            showFPS(countFrames);
            cout << "FPS: " << countFrames << endl;
            //showFPS(countFrames);
			countFrames = 0;
			lastFrameFPS = currentFrame; // Actualizamos
        }
        if (currentDelay > 0) currentDelay--; // Decrementar el contador de delay para el disparo del jugador

        // input
        processInput(window);

        // render
        //glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClearColor(0.239f, 0.298f, 0.917f, 1.0f); // Los colores del juego
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

        //drawHollowCircle(SCR_WIDTH / 2, SCR_HEIGHT / 2, 100, 360);

        // activate shader
<<<<<<< Updated upstream
        ourShader.use();		
=======
        ourShader.use();

        
>>>>>>> Stashed changes
		
        pared.Draw(ourShader);


        Renderer->DrawSprite(texSprite, glm::vec2(200.0f, 200.0f), glm::vec2(300.0f, 400.0f), 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));


        // create transformations
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection); // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.

        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("view", view);

        if (newBullet) {
            newBullet = false;
            myBullets.newBullet(camera.Position, camera.Front);
        }


        

        //cout << camera.Position[0] << ", " << camera.Position[1] << ", " << camera.Position[2] << ", " << endl;
        myBullets.DrawBullets(ourShader, myEnemies, pared, deltaTime);
        
<<<<<<< Updated upstream
        int balasRecibidas = 0;
        myEnemies.DrawEnemies(ourShader, camera.Position, myEnemies, pared, deltaTime, balasRecibidas);
        if (balasRecibidas > 0) {
			vidas -= balasRecibidas;
			cout << "Vidas: " << vidas << endl;
        }
=======
        myEnemies.DrawEnemies(ourShader, camera.Position);

        // HERE we need to show the HUD
        glBegin(GL_TRIANGLES);                      // Drawing Using Triangles
        glVertex3f(0.0f, 1.0f, 0.0f);              // Top
        glVertex3f(-1.0f, -1.0f, 0.0f);              // Bottom Left
        glVertex3f(1.0f, -1.0f, 0.0f);              // Bottom Right
        glEnd();                            // Finished Drawing The Triangle
>>>>>>> Stashed changes

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

// =====================================================================================================================
// ================================================ CONFIGURE CALLBACKS ================================================
// =====================================================================================================================

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
	//cout << "width: " << width << " height: " << height << endl;
    glViewport(0, 0, width, height);
}



void processInput(GLFWwindow* window)
{
    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;

    //REPASARLO
    
    float velocity = camera.MovementSpeed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (versionModerna) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        up = temp.checkIntersections(camera.Position, (camera.Position + (camera.Front * velocity)));
        camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime,up,down,left,right);
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            down = temp.checkIntersections(camera.Position, (camera.Position - (camera.Front * velocity)));
            camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime, up, down, left, right);
        }

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            left = temp.checkIntersections(camera.Position, (camera.Position - (camera.Right * velocity)));
            camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime, up, down, left, right);
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            right = temp.checkIntersections(camera.Position, (camera.Position + (camera.Right * velocity)));
            camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime, up, down, left, right);
        }
    }
    else {
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            up = temp.checkIntersections(camera.Position, (camera.Position + (camera.Front * velocity)));
            camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            down = temp.checkIntersections(camera.Position, (camera.Position - (camera.Front * velocity)));
            camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
            newBullet = true;
            SoundEngine->play2D("resources/effects/disparo.mp3", false); //Play the sound without loop
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            float xoffset = 0.7;
            float yoffset = 0.0;
            camera.ProcessMouseMovement(xoffset, yoffset);
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            float xoffset = -0.7;
            float yoffset = 0.0;
            camera.ProcessMouseMovement(xoffset, yoffset);
        }
        
    }
    
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) { // initially set to true
        lastX = static_cast<float>(xpos); // To avoid warning messages
        lastY = static_cast<float>(ypos);
        firstMouse = false;
    }

    if (versionModerna) {
        float xoffset = static_cast<float>(xpos - lastX);
        float yoffset = static_cast<float>(lastY - ypos); // reversed since y-coordinates range from bottom to top
        lastX = static_cast<float>(xpos);
        lastY = static_cast<float>(ypos);

        camera.ProcessMouseMovement(xoffset, yoffset);
    }
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (versionModerna) {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            cout << currentDelay<<" "<<deltaTime<<" "<< reloadTime[CAD_RAPIDA] << endl;
            if (currentDelay == 0) { // Puedo disparar
                newBullet = true;
                SoundEngine->play2D("resources/effects/disparo.mp3", false); //Play the sound without loop
                currentDelay = static_cast<unsigned int>(reloadTime[CAD_RAPIDA] / deltaTime);
            }
            //cout << camera.Position[0] << "," << camera.Position[1] << "," << camera.Position[2] << endl;
            //cout << camera.Front[0] << "," << camera.Front[1] << "," << camera.Front[2] << endl;

            
        }
    }
}

void showFPS(int fps) {
    /*
    char string[5];
	sprintf_s(string, "%d", fps);
    //unsigned char string[] = to_string(fps);
    int w = glutBitmapLength(GLUT_BITMAP_8_BY_13, reinterpret_cast<unsigned char*>(string));
    glRasterPos2f(0., 0.);
    float x = .5;
    glRasterPos2f(x - (float)SCR_WIDTH / 2, 0.);
    //glColor(1., 0., 0.);
    int len = strlen(string);
    for (int i = 0; i < len; i++) {
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, string[i]);
    }
    */
}

bool primeravez = true;
