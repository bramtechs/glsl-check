#include <cstddef>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "cxxopts.hpp"
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "spdlog/spdlog.h"

constexpr std::size_t MAX_LOG_LEN = 4000;

namespace howl
{

class formatted_error : public std::runtime_error
{
public:
    template<typename... Args>
    constexpr formatted_error(std::string_view fmt, Args&&... args)
        : std::runtime_error(fmt::vformat(fmt, fmt::make_format_args(std::forward<Args>(args)...)))
    {
    }
};

}

static std::string loadShaderSource(const std::string& filePath)
{
    std::fstream file(filePath);
    if (!file.good())
    {
        throw howl::formatted_error("Failed to open shader file: {}", filePath);
    }

    std::stringstream str;
    str << file.rdbuf();

    return str.str();
}

static GLuint compileShader(const std::string& source, GLenum shaderType)
{
    GLuint shader = glCreateShader(shaderType);
    auto sourceRaw = source.c_str();
    glShaderSource(shader, 1, &sourceRaw, NULL);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[MAX_LOG_LEN];
        glGetShaderInfoLog(shader, MAX_LOG_LEN, NULL, infoLog);
        throw howl::formatted_error("Shader Compilation Failed: {}", infoLog);
    }

    return shader;
}

static GLuint createShaderProgram(const std::string& vertexSource, const std::string& fragmentSource)
{
    GLuint vertexShader = compileShader(vertexSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fragmentSource, GL_FRAGMENT_SHADER);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    GLint success;
    char infoLog[MAX_LOG_LEN];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, MAX_LOG_LEN, NULL, infoLog);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    if (!success)
    {
        throw howl::formatted_error("Shader Program Linking Failed: {}", infoLog);
    }

    return shaderProgram;
}

void execute(int argc, char** argv)
{
    cxxopts::Options options("glsl-check", "GLSL shader compilation check");
    spdlog::set_pattern("[glsl-check:%^%l%$] %v");

    options.add_options()("v,vert", "Vertex file path", cxxopts::value<std::string>())("f,frag", "Fragment file path", cxxopts::value<std::string>())("h,help", "Print usage");

    auto result = options.parse(argc, argv);

    if (result.count("help"))
    {
        spdlog::info(options.help());
        return;
    }

    auto vertexSource = loadShaderSource(result["vert"].as<std::string>());
    auto fragmentSource = loadShaderSource(result["frag"].as<std::string>());

    if (!glfwInit())
    {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_FOCUSED, GLFW_FALSE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(1, 1, "glsl-check", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
    {
        glfwTerminate();
        throw std::runtime_error("Failed to initialize GLEW");
    }

    GLuint shaderProgram = createShaderProgram(vertexSource, fragmentSource);
    glDeleteProgram(shaderProgram);
}

int main(int argc, char** argv)
{
    try
    {
        execute(argc, argv);
        spdlog::info("No shader compile errors");
        return EXIT_SUCCESS;
    }
    catch (std::exception& ex)
    {
        spdlog::error("Failure: {}", ex.what());
        return EXIT_FAILURE;
    }
}
