#include <MyCompositor.h>
#include <math.h>
#include <WOpenGL.h>
#include <unistd.h>
#include <WWayland.h>

MyCompositor::MyCompositor(){}

GLuint LoadShader(GLenum type, const char *shaderSrc)
{
    GLuint shader;
    GLint compiled;

    // Create the shader object
    shader = glCreateShader(type);

    if (shader == 0)
        return 0;

    // Load the shader source
    glShaderSource(shader, 1, &shaderSrc, nullptr);

    // Compile the shader
    glCompileShader(shader);

    // Check the compile status
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

    if (!compiled)
    {
        GLint infoLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

void MyCompositor::initializeGL()
{
    /*************************************************
     * Here you initialize the OpenGL ES 2 context
     *************************************************/

    GLchar *vShaderStr = openShader("../MyCompositor/shaders/Vertex.glsl");

    GLchar *fShaderStr = openShader("../MyCompositor/shaders/Fragment.glsl");

    GLuint vertexShader,fragmentShader,programObject;

    // Load the vertex/fragment shaders
    vertexShader = LoadShader(GL_VERTEX_SHADER, vShaderStr);
    fragmentShader = LoadShader(GL_FRAGMENT_SHADER, fShaderStr);

    delete [] vShaderStr;
    delete [] fShaderStr;

    // Create the program object
    programObject = glCreateProgram(); 

    glAttachShader(programObject, vertexShader);
    glAttachShader(programObject, fragmentShader);

    // Bind vPosition to attribute 0
    glBindAttribLocation(programObject, 0, "vertexPosition");

    // Link the program
    glLinkProgram(programObject);

    GLint linked;

    // Check the link status
    glGetProgramiv(programObject, GL_LINK_STATUS, &linked);

    /*
    if (!linked)
    {
        GLint infoLen = 0;
        glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLen);
        glDeleteProgram(programObject);
        return exit(-1);
    }
    */

    // Use the program object
    glUseProgram(programObject);

    // Set the viewport
    glViewport(0, 0, screenWidth(), screenHeight());

    // Load the vertex data
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, square);

    // Enables the vertex array
    glEnableVertexAttribArray(0);

    // Enable alpha blending
    glEnable(GL_BLEND);

    // Set blend mode
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set clear screen color
    glClearColor(0.0f, 0.3f, 0.5f, 1.0f);

    // Get Uniform Variables
    screenUniform = glGetUniformLocation(programObject, "screen"); // (width,height) Screen dimensions
    rectUniform = glGetUniformLocation(programObject, "rect");     // (left,top,with,height) App window pos and size

    // Set screen size
    glUniform2f(screenUniform,screenWidth(),screenHeight());
}

float phase = 0.f;
void MyCompositor::paintGL()
{
    /*************************************************
     *  Here you do your drawing
     *************************************************/

    glClearColor(abs(sinf(phase)), 0.3f, 0.5f, 1.0f);
    phase+=0.1;

    // Clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT);


    for(list<WClient*>::iterator client = clients.begin(); client != clients.end(); ++client)
    {
        for(list<WSurface*>::iterator surface = (*client)->surfaces.begin(); surface != (*client)->surfaces.end(); ++surface)
        {
            if((*surface)->texture->textureId())
            {
                glBindTexture(GL_TEXTURE0,(*surface)->texture->textureId());
                glUniform4i(rectUniform,0,0,(*surface)->texture->width(),(*surface)->texture->height());
                glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
            }
        }
    }

}

void MyCompositor::libinputEvent(struct libinput_event *ev)
{
    /*************************************************
     * If you want to process libinput events manually
     *************************************************/

    libinput_event_type eventType = libinput_event_get_type(ev);

    //printf("Event type:%i\n",eventType);

    if(eventType == LIBINPUT_EVENT_KEYBOARD_KEY)
    {

        libinput_event_keyboard *keyEvent = libinput_event_get_keyboard_event(ev);
        libinput_key_state keyState = libinput_event_keyboard_get_key_state(keyEvent);

        if(keyState == libinput_key_state::LIBINPUT_KEY_STATE_RELEASED)
        {
            int keyCode = libinput_event_keyboard_get_key(keyEvent);

            //printf("Key Code:%i\n",keyCode);

            // Ends compositor if Q is pressed
            if(keyCode == 16)
            {
                terminateDisplay();
                exit(0);
            }


            if(keyCode == 17)
            {
                pid_t pid = fork();
                if (pid==0)
                {
                    system("/home/eduardo/Escritorio/build-wiggly-Desktop_Qt_6_2_1_GCC_64bit-Debug/./wiggly --platform wayland");
                    exit(0);
                }
            }
        }


    }

    repaint();
}
