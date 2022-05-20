#include<iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include<fstream>
#include<string>
#include<sstream>

//macro
#define ASSERT(x) if (!(x)) __debugbreak(); //breaks exaclty where error occurs
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__)) //says exaclty in which file, function and line the error is


static void GLClearError() 
{
    while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line) 
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] (" << error <<"): " << function <<
            " " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}

struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragementSource;
};

//two different strings of sourcecode (vertex shader and fragment shader) for CreateShader function
static ShaderProgramSource ParseShader(const std::string& filepath) // previously we returend void, now we are returning ShaderProgramSource Struct to return two different strings

{
    std::ifstream stream(filepath);

    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT=1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;

    while (getline(stream, line)) //getline used from string
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
                //set mode to vertex
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                //set mode to fragment
                type = ShaderType::FRAGMENT;
        }
        else
        {
            ss[(int)type] << line << '\n';
        }
    }
    return { ss[0].str(), ss[1].str() };

}


static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type); 
    const char* src = source.c_str(); //pointer to the source, returns a pointer inside the std::string, source needs to exist
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    // error handling
    // 
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length*sizeof(char)); 
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to complie " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment")<< "shader!" << 
            std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;

    }

    return id;

    //unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER); //fragmentShader object

}

//shader //new function CreateShader
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
   
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;
    
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSwapInterval(2);

    //glewInit();
    if (glewInit() != GLEW_OK)
        std::cout << "Error!" << std::endl;
    std::cout << glGetString(GL_VERSION) << std::endl;

    float positions[] = {
       -0.5f, -0.5f, //0
        0.5f, -0.5f, //1
        0.5f,  0.5f, //2
       -0.5f,  0.5f  //3
       
    };

    //index buffuer , removes redundency //TODO -> instead of keeping them in CPU, send to GPU and tell them to render
    unsigned int indices[] = { 
        0,1,2,
        2,3,0
    };
    unsigned int buffer;
    GLCall( glGenBuffers(1, &buffer));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
    GLCall(glBufferData(GL_ARRAY_BUFFER, 6 * 2* sizeof(float), positions,GL_STATIC_DRAW));

    GLCall(glEnableVertexAttribArray(0));
    GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));

    unsigned int ibo; //ibo = index buffer object
    GLCall(glGenBuffers(1, &ibo)); //generates buffer
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW));

    ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");

 /* std::cout << "Vertex!" << std::endl;
    std::cout << source.VertexSource << std::endl;
    std::cout << "Fragement!" << std::endl;
    std::cout << source.FragementSource<< std::endl; */

    unsigned int shader = CreateShader(source.VertexSource, source.FragementSource); // CreateShader function takes two strings of sourcecode
    GLCall(glUseProgram(shader)); //binds shader, shader = shader id

    GLCall( int location = glGetUniformLocation(shader, "u_Color")); //retriving the actual location of "u_Color" variable
    ASSERT(location != -1);
    GLCall (glUniform4f(location, 0.8f, 0.1f, 0.8f,1.0f));

  

    float r = 0.0f;
    float increment = 0.05f;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        
        GLCall(glClear(GL_COLOR_BUFFER_BIT));

        //drawcall
       // glDrawArrays(GL_TRIANGLES, 0, 6); 
        //GLClearError();

        GLCall(glUniform4f(location, r, 0.1f, 0.8f, 1.0f)); //unifors are set per draw
        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr)); //always use unsigned int to avoid invalid enum

        if (r > 1.0f)
        {
            increment = -0.05f;
        }
        else if (r < 0.0f)
        {
            increment = 0.05f;
        }

        r += increment;

        //ASSERT(GLLogCall());
        //glGetError();
        /* Swap front and back buffers */

        GLCall(glfwSwapBuffers(window));

        /* Poll for and process events */
        GLCall(glfwPollEvents());
    }

    GLCall(glDeleteProgram(shader));

 

    glfwTerminate();
    return 0;
}