#include <MyCompositor.h>
#include <math.h>
#include <WOpenGL.h>

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

    // Enable alpha blending
    glEnable( GL_BLEND );

    // Set blend mode
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set clear screen color
    glClearColor(0.0f, 0.3f, 0.5f, 1.0f);

    GLchar *vShaderStr = openShader("../MyCompositor/shaders/Vertex.glsl");

    GLchar *fShaderStr = openShader("../MyCompositor/shaders/Fragment.glsl");

    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint programObject;
    GLint linked;

    // Load the vertex/fragment shaders
    vertexShader = LoadShader(GL_VERTEX_SHADER, vShaderStr);
    fragmentShader = LoadShader(GL_FRAGMENT_SHADER, fShaderStr);

    delete [] vShaderStr;
    delete [] fShaderStr;

    // Create the program object
    programObject = glCreateProgram();


    if (programObject == 0)
        exit(0);

    glAttachShader(programObject, vertexShader);
    glAttachShader(programObject, fragmentShader);

    // Bind vPosition to attribute 0
    glBindAttribLocation(programObject, 0, "vPosition");

    // Link the program
    glLinkProgram(programObject);

    // Check the link status
    glGetProgramiv(programObject, GL_LINK_STATUS, &linked);

    if (!linked)
    {
        GLint infoLen = 0;
        glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLen);
        glDeleteProgram(programObject);
        return exit(1);
    }

    // Use the program object
    glUseProgram(programObject);
}

float phase = 0.0;
void MyCompositor::paintGL()
{
    /*************************************************
     *  Here you do your drawing
     *************************************************/

    GLfloat vVertices[] =
    { 0.0f, sinf(phase), 0.0f, -0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f };

    // Set the viewport
    glViewport(0, 0, screenWidth(), screenHeight());

    // Clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT);

    // Load the vertex data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vVertices);
    glEnableVertexAttribArray(0);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    phase+=0.01;

}

void MyCompositor::libinputEvent(struct libinput_event *ev)
{
    /*************************************************
     * If you want to process libinput events manually
     *************************************************/

    libinput_event_type eventType = libinput_event_get_type(ev);

    printf("Event type:%i\n",eventType);

    if(eventType == LIBINPUT_EVENT_KEYBOARD_KEY)
    {
        libinput_event_keyboard *keyEvent = libinput_event_get_keyboard_event(ev);
        int keyCode = libinput_event_keyboard_get_key(keyEvent);
        printf("Key Code:%i\n",keyCode);

        // Ends compositor if Q is pressed
        if(keyCode == 16)
            exit(0);
    }

    repaint();
}
