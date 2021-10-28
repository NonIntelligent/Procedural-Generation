#pragma once
#include <string>
#include <unordered_map>
#include <vector>

#include <GL/glew.h>
#include <glm/vec4.hpp>
#include <glm/matrix.hpp>

using namespace glm;

enum class UniformType {
	NONE = -99,
	INT1 = 40,
	FLOAT1 = 45,
	VEC3 = 52,
	VEC4 = 53,
	MAT4 = 54,
	TEXTURE = 60,
	CUBEMAP = 61
};

// All data required for different types of uniforms can be stored in the matrix.
// The first row is the vector and individual values starting from top left [0][0]
struct ShaderUniform {
	std::string name = "setup";
	std::string resourceName = "setup";
	mat4 dataMatrix;

	UniformType type = UniformType::NONE;
};

enum class ShaderType {
	NONE = -1,
	VERTEX = 0,
	FRAGMENT = 1,
	// Compute requires opengl 4.3 to work
	COMPUTE = 2
};

struct ShaderProgramData {
	std::string shaderSource;
	std::string filePath;
	GLuint type;
};

class Shader {
private:
	std::vector<ShaderProgramData> sources;
	GLuint m_RendererID;
	std::unordered_map<std::string, GLint> m_UniformLocationCache;
	unsigned int missingUniformCounter = 0;

public:
	//std::vector<ShaderUniform> uniforms;

public:

	Shader();
	Shader(const std::string& filepath, GLuint type);
	~Shader();

	void bind() const;
	void unBind() const;

	void deleteProgram();

	GLuint getID() const;

	// set uniforms
	void setUniform1i(const std::string& name, int value);
	void setUniform1f(const std::string & name, float value);
	void setUniform3f(const std::string & name, vec3 floats);
	void setUniform4f(const std::string& name, vec4 floats);
	void setUniformMatrix4fv(const std::string& name, GLsizei count, GLboolean transpose, mat4 matrix);

	void setUniformBlock(const std::string &blockName, GLuint blockBindingPt);

	// Reads the GLSL code from a file and pushes it to the list of sources 
	void parseShader(const std::string& filePath, GLuint type);

	// Compiles and attaches the shaders to the program 
	GLuint createShader();

private:
	// Compiles shader into program
	GLuint compileShader(GLuint type, const std::string& sourceCode);

	GLint getUniformLocation(const std::string& name);
};
