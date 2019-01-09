// glew must be first
#include <GL/glew.h>
#include <GL/glut.h>
#include <sys/time.h>

#ifdef DEBUG
#include <iostream>
#endif

//#include "smol-v/source/smolv.h"

//#include "shader.smv.h"
#include "shader.min.frag.h"

/*
 * Variables
 */

timeval start;
timeval now;
GLuint uniformTime;

/*
 * Debugging
 */

#ifdef DEBUG
void GLAPIENTRY debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
        GLsizei length, const GLchar* message, const void* userParam) {
    fprintf(stderr, "[GL] type: 0x%x, severity: 0x%x, message: %s\n", type, severity, message);
}
#endif

void initGL() {
    // Initialize GLEW to setup the OpenGL function pointers
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

    GLuint shader = glCreateProgram();
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

    uniformTime = glGetUniformLocation(shader, "time");
}

void render() {
    // Set time uniform
    gettimeofday(&now, NULL);
    glUniform1f(uniformTime, (float) (now.tv_sec - start.tv_sec) + (float) (now.tv_usec - start.tv_usec) / 1000000);

    glRecti(-1, -1, 1, 1);

    glFlush();
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutCreateWindow("");
    glutFullScreen();

    glutDisplayFunc(render);
    glutIdleFunc(render);

    initGL();

    gettimeofday(&start, NULL);

    glutMainLoop();

    return 0;
}
