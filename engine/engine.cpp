#include <iostream>

// GLM:
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Glew
#include <GL/glew.h>

// FreeGLUT:
#include <GL/freeglut.h>
#include "engine.h"
// FreeImage
#include "Shader.h"
#include "FreeImage.h"
#include "ObjectLoader.h"


//Id finestra
int windowId;

//Matrici di proiezione
glm::mat4 projection;
glm::mat4 ortho;

//Fps
int fps;
int frames;

static Engine::Handler handler;
static ShaderSettings shader;

////////////////////////////
static const char* vertShader = R"(
   #version 440 core

   uniform mat4 projection;
   uniform mat4 modelview;
   uniform mat3 normalMatrix;


   layout(location = 0) in vec3 in_Position;
   layout(location = 1) in vec3 in_Normal;
   layout (location = 2) in vec2 aTexCoord;


   // Varying:
   out vec4 fragPosition;
   out vec3 normal; 
   out vec2 TexCoord;


   void main(void)
   {
      fragPosition = modelview * vec4(in_Position, 1.0f);
      gl_Position = projection * fragPosition; 
      normal = normalMatrix * in_Normal;
      TexCoord = vec2(aTexCoord.x, aTexCoord.y);
   }
)";

////////////////////////////
static const char* fragShader = R"(
   #version 440 core

   
   out vec4 frag_Output;
   in vec4 fragPosition;
   in vec3 normal;   
   in vec2 TexCoord;   

   // Material properties:
   uniform vec3 matEmission;
   uniform vec3 matAmbient;
   uniform vec3 matDiffuse;
   uniform vec3 matSpecular;
   uniform float matShininess;

   // Light properties:
   uniform vec3 lightPosition; 
   uniform vec3 lightAmbient; 
   uniform vec3 lightDiffuse; 
   uniform vec3 lightSpecular;

   layout(binding = 0) uniform sampler2D texture1;
   void main(void)
   {
      
      // Ambient term:
      vec3 fragColor = matEmission + matAmbient * lightAmbient;

      // Diffuse term:
      vec3 _normal = normalize(normal);
      vec3 lightDirection = normalize(lightPosition - fragPosition.xyz);      
      float nDotL = dot(lightDirection, _normal);   
      if (nDotL > 0.0f)
      {
         fragColor += matDiffuse * nDotL * lightDiffuse;
      
         // Specular term:
         vec3 halfVector = normalize(lightDirection + normalize(-fragPosition.xyz));                     
         float nDotHV = dot(_normal, halfVector);         
         fragColor += matSpecular * pow(nDotHV, matShininess) * lightSpecular;
      } 
      frag_Output = texture(texture1, TexCoord)*vec4(fragColor, 1.0f);
   }
)";

#ifdef _WINDOWS
#include <Windows.h>

int APIENTRY DllMain(HANDLE instDLL, DWORD reason, LPVOID _reserved)
{
    switch (reason)
    {
    case DLL_PROCESS_ATTACH:
        break;


    case DLL_PROCESS_DETACH:
        break;
    }

    return true;
}
#endif

void displayCallback()
{

}

void reshapeCallback(int width, int height)
{
    //Setto matrice di proiezione
    glViewport(0, 0, width, height);
    //glMatrixMode(GL_PROJECTION);

    projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 1.0f, 1000.0f);
    ortho = glm::ortho(0.0f, (float)width, 0.0f, (float)height, -1.0f, 1.0f);

    //glLoadMatrixf(glm::value_ptr(projection));
    shader.m_shader->setMatrix(shader.projection, projection);

    //Reimposto in ModelView
    //glMatrixMode(GL_MODELVIEW);
}

void specialCallback(int key, int mouseX, int mouseY)
{
    handler.special(key, mouseX, mouseY);
}

void keyboardCallback(unsigned char key, int mouseX, int mouseY) {
    handler.keyboard(key, mouseX, mouseY);
}

void closeCallback()
{
    handler.close();
}

void mouseMove(int mouseX, int mouseY) {
    handler.mouse(mouseX, mouseY);
}

void __stdcall DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, GLvoid* userParam)
{
    std::cout << "OpenGL says: \"" << std::string(message) << "\"" << std::endl;
}

bool LIB_API Engine::init(Handler t_handler) {
    //Inizializzazioni di glut
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitContextVersion(4, 4);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitContextFlags(GLUT_DEBUG); // <-- Debug flag required by the OpenGL debug callback      

    glutInitWindowPosition(150, 150);
    glutInitWindowSize(handler.width, handler.height);

    //default per inizializzazione glut
    char* argv[1];
    int argc = 1;

#ifdef _WINDOWS
    argv[0] = _strdup("Application");
#else
    argv[0] = strdup("Application");
#endif

    //Inizializzazione glut
    glutInit(&argc, argv);

    //Flag opzionali
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);

    //Settaggio handler per callbacks
    handler = t_handler;

    //Creazione window e contesto OpenGL
    windowId = glutCreateWindow("VR Robotic Arm");

    //Utilizzo versione pi� aggiornata OpenGL
    glewExperimental = GL_TRUE;
    glewInit();

    if (GLEW_VERSION_4_4)
        std::cout << "Driver supports OpenGL 4.4\n" << std::endl;
    else
    {
        std::cout << "[ERROR] OpenGL 4.4 not supported\n" << std::endl;
        return -1;
    }


    // Register OpenGL debug callback:
    glDebugMessageCallback((GLDEBUGPROC)DebugCallback, nullptr);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

    //Lighting
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
   /* glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 1.0f);
    glm::vec4 gAmbient(0.2f, 0.2f, 0.2f, 1.0f);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, glm::value_ptr(gAmbient));*/

    //OpenGL enables
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_NORMALIZE);
    //glEnable(GL_LIGHTING);
    //glEnable(GL_TEXTURE_2D);

    //Callbacks
    glutDisplayFunc(displayCallback);
    glutReshapeFunc(reshapeCallback);
    glutSpecialFunc(specialCallback);
    glutKeyboardFunc(keyboardCallback);
    glutCloseFunc(closeCallback);
    glutPassiveMotionFunc(mouseMove);

    //Inizializza lettore texture
    FreeImage_Initialise();

    std::shared_ptr<Shader> m_vertex_shader;
    std::shared_ptr<Shader> m_fragment_shader;

    // Compile vertex shader:
    m_vertex_shader = std::make_shared<Shader>();
    m_vertex_shader->loadFromMemory(Shader::TYPE_VERTEX, vertShader);

    // Compile fragment shader:
    m_fragment_shader = std::make_shared<Shader>();
    m_fragment_shader->loadFromMemory(Shader::TYPE_FRAGMENT, fragShader);

    shader.m_shader = std::make_shared<Shader>();

    shader.m_shader->build(m_vertex_shader.get(), m_fragment_shader.get());

    shader.m_shader->render();
    shader.m_shader->bind(0, "in_Position");
    shader.m_shader->bind(1, "in_Normal");
    shader.m_shader->bind(2, "aTexCoord");

    // Get shader variable locations:
    shader.modelview = shader.m_shader->getParamLocation("modelview");
    shader.projection = shader.m_shader->getParamLocation("projection");
    shader.normalMatLoc = shader.m_shader->getParamLocation("normalMatrix");

    shader.matEmissionLoc = shader.m_shader->getParamLocation("matEmission");
    shader.matAmbientLoc = shader.m_shader->getParamLocation("matAmbient");
    shader.matDiffuseLoc = shader.m_shader->getParamLocation("matDiffuse");
    shader.matSpecularLoc = shader.m_shader->getParamLocation("matSpecular");
    shader.matShininessLoc = shader.m_shader->getParamLocation("matShininess");

    shader.lightPositionLoc = shader.m_shader->getParamLocation("lightPosition");
    shader.lightAmbientLoc = shader.m_shader->getParamLocation("lightAmbient");
    shader.lightDiffuseLoc = shader.m_shader->getParamLocation("lightDiffuse");
    shader.lightSpecularLoc = shader.m_shader->getParamLocation("lightSpecular");
    shader.texture = shader.m_shader->getParamLocation("texture");

    return true;
}

void LIB_API Engine::clear()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void LIB_API Engine::render(const List& list, std::shared_ptr<Camera> camera)
{
    //glMatrixMode(GL_MODELVIEW);
    for (int i = 0; i < list.size(); i++) {
        shader.m_shader->render();
        shader.m_shader->setMatrix(shader.modelview, camera->inverseCamera() * list[i].second);
        list[i].first->render(camera->inverseCamera() * list[i].second,shader);

        Mesh* mesh = dynamic_cast<Mesh*>(list[i].first.get());
        if (mesh) {
            if (mesh->shadow()) {
                //mesh->render_shadow(camera->inverseCamera() * mesh->get_shadow_mat() * list[i].second);
            }
        }
    }
}

void LIB_API Engine::swap()
{
    glutSwapBuffers();
}

std::shared_ptr<Node LIB_API> Engine::load(std::string file) {
    ObjectLoader object_loader(shader);
    return object_loader.LoadScene(file);
}

void LIB_API Engine::free()
{
    FreeImage_DeInitialise();
}

void LIB_API Engine::update()
{
    glutMainLoopEvent();
}

void LIB_API Engine::drawText(const std::string& text, float x, float y)
{
    //Setto matrice di proiezione
   /* glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(ortho));

    //Setto matrice di model view
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(glm::value_ptr(glm::mat4(1.0f)));

    // Disable lighting before rendering 2D text:
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    // Write some text:
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(x, y);
    glutBitmapString(GLUT_BITMAP_8_BY_13, (unsigned char*)text.c_str());

    // Reactivate lighting:
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);

    // Reimposto proiezione
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(projection));*/
}
