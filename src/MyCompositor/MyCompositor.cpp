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
    checkGLError("1");

    glAttachShader(programObject, vertexShader);
    checkGLError("2");

    glAttachShader(programObject, fragmentShader);
    checkGLError("3");


    // Bind vPosition to attribute 0
    glBindAttribLocation(programObject, 0, "vertexPosition");
    checkGLError("4");

    // Link the program
    glLinkProgram(programObject);
    checkGLError("5");

    GLint linked;

    // Check the link status
    glGetProgramiv(programObject, GL_LINK_STATUS, &linked);
    checkGLError("6");

    if (!linked)
    {
        GLint infoLen = 0;
        glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLen);
        checkGLError("7");

        glDeleteProgram(programObject);
        checkGLError("8");
        return exit(-1);
    }

    // Enable alpha blending
    glEnable(GL_BLEND);
    checkGLError("9");

    // Set blend mode
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    checkGLError("10");

    // Set clear screen color
    glClearColor(0.0f, 0.3f, 0.5f, 1.0f);
    checkGLError("11");

    // Use the program object
    glUseProgram(programObject);
    checkGLError("12");

    // Set the viewport
    glViewport(0, 0, screenWidth(), screenHeight());
    checkGLError("13");

    // Load the vertex data
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, square);
    checkGLError("14");

    // Enables the vertex array
    glEnableVertexAttribArray(0);
    checkGLError("15");

    // Get Uniform Variables
    screenUniform = glGetUniformLocation(programObject, "screen"); // (width,height) Screen dimensions
    checkGLError("16");

    rectUniform = glGetUniformLocation(programObject, "rect");     // (left,top,with,height) App window pos and size
    checkGLError("17");

    glUniform1i(glGetUniformLocation(programObject, "application"), 0);
    checkGLError("18");

    // Set screen size
    glUniform2f(screenUniform,screenWidth(),screenHeight());
    checkGLError("19");

    glActiveTexture(GL_TEXTURE0);
    checkGLError("20");


    // Create cursor texture
    unsigned char cursorPixels[4*32*32];
    for(int i = 0; i < 4*32*32; i++)
        cursorPixels[i] = 255;

    cursorTexture = new WTexture();
    cursorTexture->setData(32,32,&cursorPixels);
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

            if(!(*surface)->xdg_shell) continue;
            glBindTexture(GL_TEXTURE_2D,(*surface)->texture->textureId());
            checkGLError("21");

            glUniform4f(rectUniform,0,0,(*surface)->texture->width(),(*surface)->texture->height());
            checkGLError("22");

            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
            checkGLError("23");

            //wl_pointer_send
            //wl_pointer_send_enter((*client)->pointer,0,(*surface)->resource,100,300);
            //wl_keyboard_send_key((*client)->keyboard, 0, n, 16, wl_keyboard_key_state::WL_KEYBOARD_KEY_STATE_PRESSED);
            //wl_keyboard_send_key((*client)->keyboard, 0, n, 16, wl_keyboard_key_state::WL_KEYBOARD_KEY_STATE_RELEASED);


            /*
            if ((*surface)->frame_callback)
            {

                wl_callback_send_done((*surface)->frame_callback,getMilliseconds());
                wl_resource_destroy((*surface)->frame_callback);
                (*surface)->frame_callback = nullptr;
            }
            */

        }
    }

    //if(focusSurface)
        //wl_pointer_send_frame(focusSurface->client->pointer);

    drawCursor();
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

            if(focusSurface)
            {

            }
            printf("Key Code:%i\n",keyCode);

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
                    system("/home/eduardo/Escritorio/build-cube-Desktop_Qt_6_2_1_GCC_64bit-Debug/./cube --platform wayland");
                    exit(0);
                }
            }
            if(keyCode == 3)
            {
                pid_t pid = fork();
                if (pid==0)
                {
                    system("/home/eduardo/Escritorio/build-chip-Desktop_Qt_6_2_1_GCC_64bit-Debug/./chip --platform wayland");
                    exit(0);
                }
            }
        }
    }

    //repaint();
}

void MyCompositor::pointerPosChanged(double x, double y)
{
    for(list<WClient*>::iterator client = clients.begin(); client != clients.end(); ++client)
    {
        for(list<WSurface*>::iterator surface = (*client)->surfaces.begin(); surface != (*client)->surfaces.end(); ++surface)
        {

            if(!(*surface)->xdg_shell) continue;

            // Mouse move event
            if((*surface)->containsPoint(x,y) && focusSurface == (*surface))
            {
                wl_pointer_send_motion(
                            (*client)->pointer,
                            getMilliseconds(),
                            wl_fixed_from_double((*surface)->mapXtoLocal(x)),
                            wl_fixed_from_double((*surface)->mapYtoLocal(y)));

                wl_pointer_send_frame((*client)->pointer);


            }

            // Mouse leave surface
            if(!(*surface)->containsPoint(x,y) && focusSurface == (*surface))
            {
                wl_pointer_send_leave((*client)->pointer,0,(*surface)->resource);
                wl_pointer_send_frame((*client)->pointer);
                focusSurface = nullptr;
                printf("Mouse left surface\n");
            }

            // Mouse enter surface
            if((*surface)->containsPoint(x,y) && focusSurface != (*surface))
            {
                wl_pointer_send_enter(
                            (*client)->pointer,
                            0,
                            (*surface)->resource,
                            wl_fixed_from_double((*surface)->mapXtoLocal(x)),
                            wl_fixed_from_double((*surface)->mapYtoLocal(y)));


                wl_pointer_send_frame((*client)->pointer);
                focusSurface = (*surface);
                printf("Mouse entered surface\n");
            }
        }
    }

    repaint();
}

void MyCompositor::pointerClickEvent(int x, int y, uint32_t button, libinput_button_state state)
{
    if(focusSurface)
    {
        wl_pointer_send_button(focusSurface->client->pointer,0,getMilliseconds(),button,state);
        wl_pointer_send_frame(focusSurface->client->pointer);
    }
    printf("Mouse button: %i state %i\n",BTN_LEFT,state);
}

void MyCompositor::drawCursor()
{
    glBindTexture(GL_TEXTURE_2D,cursorTexture->textureId());
    glUniform4f(rectUniform,getPointerX(),getPointerY(),5,5);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}
