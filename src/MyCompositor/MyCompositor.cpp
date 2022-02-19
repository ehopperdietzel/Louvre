#include <MyCompositor.h>
#include <math.h>
#include <WOpenGL.h>
#include <unistd.h>
#include <WWayland.h>
#include <linux/input-event-codes.h>

MyCompositor::MyCompositor(){}


GLuint LoadShader(GLenum type, const char *shaderSrc)
{
    GLuint shader;
    GLint compiled;

    // Create the shader object
    shader = glCreateShader(type);
    checkGLError("21");

    // Load the shader source
    glShaderSource(shader, 1, &shaderSrc, nullptr);
    checkGLError("22");

    // Compile the shader
    glCompileShader(shader);
    checkGLError("23");

    // Check the compile status
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    checkGLError("24");

    if (!compiled)
    {
        GLint infoLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
        checkGLError("25");

        GLchar *errorLog = new GLchar(infoLen);

        glGetShaderInfoLog(shader, infoLen, &infoLen, errorLog);
        checkGLError("26");

        printf("%s",errorLog);

        delete errorLog;

        glDeleteShader(shader);
        checkGLError("27");
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

    if (!linked)
    {
        GLint infoLen = 0;
        glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLen);
        glDeleteProgram(programObject);
        return exit(-1);
    }

    // Enable alpha blending
    glEnable(GL_BLEND);

    // Set blend mode
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set clear screen color
    glClearColor(0.0f, 0.3f, 0.5f, 1.0f);

    // Use the program object
    glUseProgram(programObject);

    // Set the viewport
    glViewport(0, 0, screenWidth(), screenHeight());

    // Load the vertex data
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, square);

    // Enables the vertex array
    glEnableVertexAttribArray(0);

    // Get Uniform Variables
    screenUniform = glGetUniformLocation(programObject, "screen"); // (width,height) Screen dimensions

    rectUniform = glGetUniformLocation(programObject, "rect");     // (left,top,with,height) App window pos and size

    glUniform1i(glGetUniformLocation(programObject, "application"), 0);

    // Set screen size
    glUniform2f(screenUniform,screenWidth(),screenHeight());

    glActiveTexture(GL_TEXTURE0);

    // Create cursor texture
    unsigned char cursorPixels[4*32*32];
    for(int i = 0; i < 4*32*32; i++)
        cursorPixels[i] = 255;

    cursorTexture = new WTexture();
    cursorTexture->setData(32,32,&cursorPixels);

}

void MyCompositor::paintGL()
{
    /*************************************************
     *  Here you do your drawing
     *************************************************/

    glClear(GL_COLOR_BUFFER_BIT);


    for(list<WSurface*>::iterator surface = surfacesList.begin(); surface != surfacesList.end(); ++surface)
    {
        if(!(*surface)->xdg_shell) continue;
        glBindTexture(GL_TEXTURE_2D,(*surface)->texture->textureId());
        checkGLError("21");

        glUniform4f(rectUniform,(*surface)->getX(),(*surface)->getY(),(*surface)->texture->width(),(*surface)->texture->height());
        checkGLError("22");

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        checkGLError("23");
    }

    drawCursor();
}

void MyCompositor::newClient(WClient *)
{
    /*******************************************************************************
     * Notify a new client connection, it's automatically added to an internal list
     * you can access with 'compositor->clients'
     *******************************************************************************/

    printf("New client connected.\n");
}

void MyCompositor::clientDisconnected(WClient *)
{
    /*******************************************************************************
     * Notify a client disconnection, it's automatically removed from the internal
     * 'compositor->clients' list.
     * All destroy events from resources related to the client (like surfaces, regions, etc)
     * are prevously notified.
     *******************************************************************************/

    printf("Client disconnected.\n");
}

void MyCompositor::newSurface(WSurface *surface)
{
    surfacesList.push_back(surface);
}

void MyCompositor::surfaceDestroyed(WSurface *surface)
{
    surfacesList.remove(surface);
}

void MyCompositor::libinputEvent(libinput_event *)
{

    /*************************************************
     * If you wish to process libinput events manually
     *************************************************/

    // libinput_event_type eventType = libinput_event_get_type(event);
}

void MyCompositor::pointerPosChanged(double x, double y, UInt32 milliseconds)
{
    WSurface *surface;

    for(list<WSurface*>::reverse_iterator surfaceIt = surfacesList.rbegin(); surfaceIt != surfacesList.rend(); ++surfaceIt)
    {

        surface =  *surfaceIt;

        if(!surface->xdg_shell) continue;

        // Mouse move event
        if(surface->containsPoint(x,y) && getPointerFocusSurface() == surface)
        {
            surface->sendPointerMotionEvent(surface->mapXtoLocal(x),surface->mapYtoLocal(y),milliseconds);
            break;
        }

        // Mouse leave surface
        if(!surface->containsPoint(x,y) && getPointerFocusSurface() == surface)
        {
            surface->sendPointerLeaveEvent();
            printf("Mouse left surface\n");
            continue;
        }

        // Mouse enter surface
        if(surface->containsPoint(x,y) && getPointerFocusSurface() != surface)
        {

            surface->sendPointerEnterEvent(surface->mapXtoLocal(x),surface->mapXtoLocal(y));
            printf("Mouse entered surface\n");
            break;
        }
    }

    repaint();
}

void MyCompositor::pointerClickEvent(int x, int y, UInt32 button, UInt32 state, UInt32 milliseconds)
{
    (void)x;(void)y;
    if(getPointerFocusSurface())
    {
        getPointerFocusSurface()->sendPointerButtonEvent(button,state,milliseconds);

        if(getKeyboardFocusSurface() != getPointerFocusSurface())
            getPointerFocusSurface()->sendKeyboardEnterEvent();

        // Raise view
        surfacesList.remove(getPointerFocusSurface());
        surfacesList.push_back(getPointerFocusSurface());
    }
}

void MyCompositor::keyModifiersEvent(UInt32 depressed, UInt32 latched, UInt32 locked, UInt32 group)
{
    if(getKeyboardFocusSurface())
        getKeyboardFocusSurface()->sendKeyModifiersEvent(depressed,latched,locked,group);
}

void MyCompositor::keyEvent(UInt32 keyCode, UInt32 keyState, UInt32 milliseconds)
{
    if(getKeyboardFocusSurface())
        getKeyboardFocusSurface()->sendKeyEvent(keyCode,keyState,milliseconds);


    if(keyState == LIBINPUT_KEY_STATE_RELEASED)
    {
        // Ends compositor if Q is pressed
        if(keyCode == 16)
        {
            terminateDisplay();
            exit(0);
        }
        if(keyCode == 2)
        {
            pid_t pid = fork();
            if (pid==0)
            {
                system("gnome-calculator");
                exit(0);
            }
        }
        if(keyCode == 3)
        {
            pid_t pid = fork();
            if (pid==0)
            {
                system("xed");
                exit(0);
            }
        }
    }
}

void MyCompositor::drawCursor()
{
    glBindTexture(GL_TEXTURE_2D,cursorTexture->textureId());
    glUniform4f(rectUniform,getPointerX(),getPointerY(),5,5);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}
