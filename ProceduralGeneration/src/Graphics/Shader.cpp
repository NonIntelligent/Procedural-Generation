#include "Graphics/Shader.h"
#include <iostream>
#include <fstream>
#include <sstream>

#include "Vendor/ErrorLogger.h"

Shader::Shader() : m_RendererID(0) {
}

Shader::Shader(const std::string & filepath, GLuint type) : m_RendererID(0) {
	parseShader(filepath, type);
}

Shader::~Shader() {
	
}

void Shader::bind() const {
	GLCall(glUseProgram(m_RendererID));
}

void Shader::unBind() const {
	GLCall(glUseProgram(0));
}

void Shader::deleteProgram() {
	GLCall(glDeleteProgram(m_RendererID));
}

GLuint Shader::getID() const {
	return m_RendererID;
}

void Shader::parseShader(const std::string& filePath, GLuint type) {
	std::ifstream stream(filePath);

	// Stream failed to open file
	if (!stream.is_open()) {
		std::cout << "Problem in loading file, skipping" << std::endl;
		return;
	}

	std::string line;
	std::stringstream ss;

	while (getline(stream, line)) {
		ss << line << '\n';
	}

	sources.push_back({ ss.str(), filePath, type });
}

GLuint Shader::compileShader(GLuint type, const std::string& sourceCode) {
	GLuint id = glCreateShader(type);
	const char* src = sourceCode.c_str();
	GLCall(glShaderSource(id, 1, &src, nullptr));
	GLCall(glCompileShader(id));


	// get compile status
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);

	// If compile failed the print out error
	if(result == GL_FALSE) {
		std::string shader = type == GL_VERTEX_SHADER ? "vertex" : "fragment";
		shader = type == GL_GEOMETRY_SHADER ? "geometry" : "compute";

		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile " << shader << " shader" << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}

	return id;
}

GLuint Shader::createShader() {
	unsigned int program = glCreateProgram();
	GLuint* compiled = new GLuint[sources.size()];

	for (int i = 0; i < sources.size(); i++) {
		compiled[i] = compileShader(sources[i].type, sources[i].shaderSource);
		GLCall(glAttachShader(program, compiled[i]));
	}

	GLCall(glLinkProgram(program));
	GLCall(glValidateProgram(program));

	// Delete individual shaders
	for (int i = 0; i < sources.size(); i++) {
		glDeleteShader(compiled[i]);
	}

	m_RendererID = program;

	return program;
}


void Shader::setUniform1i(const std::string & name, int value) {
	GLCall(glUniform1i(getUniformLocation(name), value));
}

void Shader::setUniform1f(const std::string & name, float value) {
	GLCall(glUniform1f(getUniformLocation(name), value));
}

void Shader::setUniform3f(const std::string & name, vec3 floats){
	GLCall(glUniform3f(getUniformLocation(name), floats.x, floats.y, floats.z));
}

void Shader::setUniform4f(const std::string & name, vec4 floats) {
	GLCall(glUniform4f(getUniformLocation(name), floats.x, floats.y, floats.z, floats.w));
}

void Shader::setUniformMatrix4fv(const std::string & name, GLsizei count, GLboolean transpose, mat4 matrix) {
	float* ptr = (float*) &matrix[0];
	GLCall(glUniformMatrix4fv(getUniformLocation(name), count, transpose, ptr));
}

void Shader::setUniformMatrix3fv(const std::string& name, GLsizei count, GLboolean transpose, mat3 matrix) {
	float* ptr = (float*)&matrix[0];
	GLCall(glUniformMatrix3fv(getUniformLocation(name), count, transpose, ptr));
}

void Shader::setUniformBlock(const std::string &blockName, GLuint blockBindingPt) {
	GLuint uniformBlock;
	GLCall(uniformBlock = glGetUniformBlockIndex(m_RendererID, blockName.c_str()));
	GLCall(glUniformBlockBinding(m_RendererID, uniformBlock, blockBindingPt));
}


GLint Shader::getUniformLocation(const std::string & name) {
	// Caching disabled for now as multiple objects using the same shader will have different cache values.

	/*if(m_UniformLocationCache.find(name) != m_UniformLocationCache.end()) {

		return m_UniformLocationCache[name];
	}*/

	GLint location = -1;

	GLCall(location = glGetUniformLocation(m_RendererID, name.c_str()));
	if(location == -1 && missingUniformCounter++ < 10) {
		std::cout << "Warning: uniform doesn't exist or is not in use '" << name << "'" << " RendererID: " << m_RendererID << std::endl;
	}
	m_UniformLocationCache[name] = location;
	return location;
}
