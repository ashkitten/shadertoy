#include <GL/glxew.h>
#include <GL/glx.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xrandr.h>
#include <sys/time.h>
#include <sys/syscall.h>
#include <unistd.h>

#ifdef DEBUG
#include <iostream>
#endif

//#include "smol-v/source/smolv.h"

//#include "shader.smv.h"
#include "shader.min.frag.h"

/*
 * Variables
 */

// Relating to the X protocol and GLX context
Display *display;
Window root;
GLint attributes[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
XVisualInfo *visualInfo;
Colormap colormap;
XSetWindowAttributes xSetWindowAttributes;
Window window;

timeval start;

GLuint VBO, VAO, EBO;
GLuint shader;

const int windowWidth = 1920;
const int windowHeight = 1080;

void initWindow() {
    display = XOpenDisplay(NULL);

    root = DefaultRootWindow(display);

    // set video mode to 1920x1080 (thanks blackle)
    int num_sizes;
    XRRScreenSize* sizes = XRRSizes(display, 0, &num_sizes);
    for (int i = 0; i < num_sizes; i++) {
        if (sizes[i].width == windowWidth && sizes[i].height == windowHeight) {
            XRRScreenConfiguration* conf = XRRGetScreenInfo(display, root);
            XRRSetScreenConfig(display, conf, root, i, RR_Rotate_0, CurrentTime);
            break;
        }
    }

    visualInfo = glXChooseVisual(display, 0, attributes);
    colormap = XCreateColormap(display, root, visualInfo->visual, AllocNone);
    xSetWindowAttributes.colormap = colormap;

    window = XCreateWindow(display, root, 0, 0, windowWidth, windowHeight, 0, visualInfo->depth, InputOutput, visualInfo->visual, CWColormap, &xSetWindowAttributes);

    Atom wmState = XInternAtom(display, "_NET_WM_STATE", true);
    Atom wmFullscreen = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", true);
    XChangeProperty (display, window, wmState, XA_ATOM, 32, PropModeReplace, (unsigned char *) &wmFullscreen, 1);

    XMapWindow(display, window);
}

#ifdef DEBUG
void GLAPIENTRY debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
        GLsizei length, const GLchar* message, const void* userParam) {
    fprintf(stderr, "[GL] type: 0x%x, severity: 0x%x, message: %s\n", type, severity, message);
}
#endif

void initGL() {
    GLXContext glXContext = glXCreateContext(display, visualInfo, NULL, GL_TRUE);
    glXMakeCurrent(display, window, glXContext);

    // Needed for blending the text and main shader quads
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    glewInit();

    #ifdef DEBUG
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(debugCallback, NULL);
    #endif

    //size_t spvSize = smolv::GetDecodedBufferSize(shader_smv, shader_smv_len);
    //uint8_t *spvBuf[spvSize];
    //smolv::Decode(shader_smv, shader_smv_len, spvBuf, spvSize);

    //GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
    //glShaderBinary(1, &frag, GL_SHADER_BINARY_FORMAT_SPIR_V_ARB, spvBuf, spvSize);
    //glSpecializeShaderARB(frag, "main", 0, 0, 0);

    GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag, 1, &shader_min_frag, NULL);
    glCompileShader(frag);

    #ifdef DEBUG
    {
        GLint logLength = 0;
        glGetShaderiv(frag, GL_INFO_LOG_LENGTH, &logLength);
        if(logLength > 0) {
            GLchar infoLog[logLength];
            glGetShaderInfoLog(frag, logLength, &logLength, infoLog);
            std::cout << "--- fragment compile output ---" << std::endl;
            std::cout.write(infoLog, logLength) << std::endl;
            std::cout << "--- fragment compile output ---" << std::endl;
            return;
        }
    }
    #endif

    shader = glCreateProgram();
    glAttachShader(shader, frag);
    glLinkProgram(shader);

    #ifdef DEBUG
    {
        GLint logLength = 0;
        glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        if(logLength > 0) {
            GLchar infoLog[logLength];
            glGetProgramInfoLog(shader, logLength, &logLength, infoLog);
            std::cout << "--- program link output ---" << std::endl;
            std::cout.write(infoLog, logLength) << std::endl;
            std::cout << "--- program link output ---" << std::endl;
            return;
        }
    }
    #endif

    glUseProgram(shader);
    glDeleteShader(frag);
}

void render() {
    // Set time uniform
    timeval now;
    syscall(SYS_gettimeofday, &now, NULL);
    glUniform1f(glGetUniformLocation(shader, "time"), (float) (now.tv_sec - start.tv_sec) + (float) (now.tv_usec - start.tv_usec) / 1000000);

    glRecti(-1, -1, 1, 1);

    // Swap the screen buffers
    glXSwapBuffers(display, window);
}

int main() {
    initWindow();
    initGL();

    syscall(SYS_gettimeofday, &start, NULL);

    // Main render loop
    while(true) {
        render();
    }

    return 0;
}
