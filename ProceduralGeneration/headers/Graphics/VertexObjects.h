#pragma once
#include <vector>

#include "GL/glew.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/matrix.hpp>
#include "Vendor/ErrorLogger.h"

class VertexBuffer {

private:
	unsigned int m_RendererID = 0;
	unsigned int drawHint = 0;
	unsigned int m_Count = 0;

public:
	VertexBuffer() {};
	// Size in bytes
	VertexBuffer(const void* data, unsigned int size, unsigned int count, unsigned int drawHint);
	~VertexBuffer();

	void changeBufferData(const void* data, unsigned int size, unsigned int count, unsigned int drawHint);
	void subBufferData(const void* data, unsigned int size, unsigned int count, int offset);

	void bind() const;
	void unBind() const;

	// This method must be called before changing the buffer data to use the new hint.
	void setHint(unsigned int drawHint);

	inline unsigned int getCount() const {
		return m_Count;
	}
};

// Data contained in the buffer layout
struct VertexBufferElement {
	GLuint type;
	GLuint count;
	GLubyte normalised;

	static GLuint getSizeOfType(GLuint type) {
		switch(type) {
		case GL_FLOAT:			return 4;
		case GL_UNSIGNED_INT:	return 4;
		case GL_UNSIGNED_BYTE:	return 1;
		}
		ASSERT(false);
		return 0;
	}
};

// Easily setup the layout of the buffer for rendering
class VertexBufferLayout {
private:
	std::vector<VertexBufferElement> m_Elements;
	std::vector<int> isInstanced;
	GLuint m_Stride;
public:
	VertexBufferLayout()
		: m_Stride(0) {
	};

	template<typename T>
	void push(unsigned int count, int instanced = 0) {
		static_assert(false);
	}

	template<>
	void push<float>(unsigned int count, int instanced) {
		m_Elements.push_back({GL_FLOAT, count, GL_FALSE});
		m_Stride += count * VertexBufferElement::getSizeOfType(GL_FLOAT);
		isInstanced.push_back(instanced);
	}

	template<>
	void push<GLuint>(unsigned int count, int instanced) {
		m_Elements.push_back({GL_UNSIGNED_INT, count, GL_FALSE});
		m_Stride += count * VertexBufferElement::getSizeOfType(GL_UNSIGNED_INT);
		isInstanced.push_back(instanced);
	}

	template<>
	void push<GLubyte>(unsigned int count, int instanced) {
		m_Elements.push_back({GL_UNSIGNED_BYTE, count, GL_TRUE});
		m_Stride += count * VertexBufferElement::getSizeOfType(GL_UNSIGNED_BYTE);
		isInstanced.push_back(instanced);
	}

	template<>
	void push<glm::vec4>(unsigned int count, int instanced) {
		m_Stride += count * sizeof(glm::vec4);

		for (int i = 0; i < count; i++) {
			m_Elements.push_back({GL_FLOAT, 4, GL_FALSE});
			isInstanced.push_back(instanced);
		}
	}

	template<>
	void push<glm::mat4>(unsigned int count, int instanced) {
		m_Stride += count * sizeof(glm::mat4);

		for (int i = 0; i < 4 * count; i++) {
			m_Elements.push_back({GL_FLOAT, 4, GL_FALSE});
			isInstanced.push_back(instanced);
		}
	}

	inline const std::vector<VertexBufferElement>& getElements() const {
		return m_Elements;
	}
	inline const std::vector<int>& getInstances() const {
		return isInstanced;
	}
	inline unsigned int getStride() const {
		return m_Stride;
	}

};

class VertexArray {
private:
	GLuint m_RendererID;

	int numOfElements = 0;

public:
	VertexArray();
	~VertexArray();

	void addBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);
	void updateAttributes(GLuint index, GLint count, GLenum type, GLboolean normalised, GLsizei stride, unsigned int ptr, int divisor);

	void bind() const;
	void unBind() const;
};

// Allows vertices to be reused when drawing
class IndexBuffer {

private:
	unsigned int m_RendererID = 0;
	unsigned int m_Count = 0;
	unsigned int drawHint = 0;

public:
	IndexBuffer() {
	};
	IndexBuffer(const unsigned int * data, unsigned int count, unsigned int drawHint);
	~IndexBuffer();

	void bind() const;
	void unBind() const;

	inline unsigned int getCount() const {
		return m_Count;
	}
};

// https://www.youtube.com/watch?v=21UsMuFTN0k&list=PLRIWtICgwaX23jiqVByUs0bqhnalNTNZh&index=2
// https://learnopengl.com/Advanced-OpenGL/Framebuffers
class FrameBuffer {
	unsigned int m_RendererID = 0;

	// standard frame buffer attatchments
	unsigned int m_ColorBuffer = 0;
	unsigned int m_DepthBuffer = 0;

	// Frame buffer attatchments as textures
	unsigned int m_ColorTextureID = 0;
	unsigned int m_DepthTextureID = 0;

public:
	FrameBuffer() { };
	FrameBuffer(int colorAttatchment);
	~FrameBuffer();

	void bind(int width, int height) const;
	void unBind(int width, int height) const;

	void createColorBuffer(int width, int height);
	void createDepthBuffer(int width, int height);

	void createTextureAttatchment(int width, int height);
	void createDepthTextureAttatchment(int width, int height);

	bool checkIfComplete();

	void checkStatus();

	unsigned int getID();

	unsigned int getColorBuffer();
	unsigned int getDepthBuffer();

	unsigned int getColorTexture();
	unsigned int getDepthTexture();
};

struct Vertex {
	glm::vec3 position;
	glm::vec3 color;
	glm::vec3 normal;
	glm::vec2 textcoord;
};

struct VertexColour {
	glm::vec3 position;
	glm::vec3 color;
	glm::vec3 normal;
};

struct VertexTexture {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 textcoord;
};

struct VertexNormal {
	glm::vec3 position;
	glm::vec3 normal;
};

struct VertexUV {
	glm::vec3 position;
	glm::vec2 textcoord;
};

struct VertexBasic {
	glm::vec3 position;
};
