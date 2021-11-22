#include "Graphics/VertexObjects.h"

#include <iostream>

VertexBuffer::VertexBuffer(const void * data, unsigned int size, unsigned int count, unsigned int hint) 
	: drawHint(hint), m_Count(count){

	GLCall(glGenBuffers(1, &m_RendererID));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));

	// Default to static draw if no hint was given.
	if(hint != NULL && (hint == GL_DYNAMIC_DRAW || hint == GL_STREAM_DRAW)) {
		GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, hint));
	}
	else {
		GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
	}
}

VertexBuffer::~VertexBuffer() {
	GLCall(glDeleteBuffers(1, &m_RendererID));
}

void VertexBuffer::changeBufferData(const void* data, unsigned int size, unsigned int count) {
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
	GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, drawHint));
	m_Count = count;
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void VertexBuffer::bind() const {
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
}

void VertexBuffer::unBind() const {
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void VertexBuffer::setHint(unsigned int drawHint) {
	this->drawHint = drawHint;
}

///////////////////////////////////////////////

VertexArray::VertexArray() {
	GLCall(glGenVertexArrays(1, &m_RendererID));
}

VertexArray::~VertexArray() {
	GLCall(glDeleteVertexArrays(1, &m_RendererID));
}

void VertexArray::addBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout) {
	bind();
	vb.bind();
	const auto& elements = layout.getElements();
	const auto& isInstanced = layout.getInstances();
	GLuint offset = 0;
	int attribute = 0;

	for(GLuint i = 0; i < elements.size(); i++) {
		const auto& element = elements[i];
		attribute = i + numOfElements;
		GLCall(glEnableVertexAttribArray(attribute));
		GLCall(glVertexAttribPointer(attribute, element.count, element.type,
			element.normalised, layout.getStride(), (const void*)offset));
		offset += element.count * VertexBufferElement::getSizeOfType(element.type);

		glVertexAttribDivisor(attribute, isInstanced[i]);
	}

	numOfElements += elements.size();
}

void VertexArray::bind() const {
	GLCall(glBindVertexArray(m_RendererID));
}

void VertexArray::unBind() const {
	GLCall(glBindVertexArray(0));
}

//////////////////////////////////////////////

IndexBuffer::IndexBuffer(const unsigned int * data, unsigned int count, unsigned int hint)
	: m_Count(count), drawHint(hint) {

	GLCall(glGenBuffers(1, &m_RendererID));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));

	// default to static hint if no valid hint was set
	if(hint != NULL && (hint == GL_DYNAMIC_DRAW || hint == GL_STREAM_DRAW)) {
		GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLuint), data, hint));
	}
	else {
		GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLuint), data, GL_STATIC_DRAW));
	}
}

IndexBuffer::~IndexBuffer() {
	GLCall(glDeleteBuffers(1, &m_RendererID));
}

void IndexBuffer::bind() const {
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
}

void IndexBuffer::unBind() const {
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

FrameBuffer::FrameBuffer(int colorAttatchment) {
	glGenFramebuffers(1, &m_RendererID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
}

FrameBuffer::~FrameBuffer() {
	glDeleteFramebuffers(1, &m_RendererID);
	if (m_ColorTextureID != 0) glDeleteTextures(1, &m_ColorTextureID);

	if (m_DepthTextureID != 0) glDeleteTextures(1, &m_DepthTextureID);
}

void FrameBuffer::bind(int width, int height) const {
	// Make sure no texture is bound
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
	glViewport(0, 0, width, height);
}

void FrameBuffer::unBind(int width, int height) const {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, width, height);
}

void FrameBuffer::createColorBuffer(int width, int height) {
	glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

	GLCall(glGenRenderbuffers(1, &m_ColorBuffer));
	GLCall(glBindRenderbuffer(GL_RENDERBUFFER, m_ColorBuffer));
	GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, width, height));
	GLCall(glBindRenderbuffer(GL_RENDERBUFFER, 0));
	GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_ColorBuffer));
}

void FrameBuffer::createDepthBuffer(int width, int height) {
	glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

	GLCall(glGenRenderbuffers(1, &m_DepthBuffer));
	GLCall(glBindRenderbuffer(GL_RENDERBUFFER, m_DepthBuffer));
	GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width, height));
	GLCall(glBindRenderbuffer(GL_RENDERBUFFER, 0));
	GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_DepthBuffer));
}

void FrameBuffer::createTextureAttatchment(int width, int height) {
	glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

	GLCall(glGenTextures(1, &m_ColorTextureID));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_ColorTextureID));

	// Linearlly choose and scale the texture pixel
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

	// Wrap horizontally and vertically by clamp
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

	GLCall(glGenerateMipmap(GL_TEXTURE_2D));

	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0));

	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorTextureID, 0));
}

void FrameBuffer::createDepthTextureAttatchment(int width, int height) {
	glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

	GLCall(glGenTextures(1, &m_DepthTextureID));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_DepthTextureID));

	// Linearlly choose and scale the texture pixel
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

	// Wrap horizontally and vertically by clamp
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

	GLCall(glGenerateMipmap(GL_TEXTURE_2D));

	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0));

	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthTextureID, 0));
}

bool FrameBuffer::checkIfComplete() {
	return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

void FrameBuffer::checkStatus() {
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID));
	auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (fboStatus == GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "SUCCESS: FrameBuffer is complete!" << std::endl;
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
		return;
	}

	if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR: FrameBuffer is not complete" << std::endl;
	}

	if (fboStatus == GL_FRAMEBUFFER_UNSUPPORTED) {
		std::cout << "ERROR: FrameBuffer implementation is unsupported by driver" << std::endl;
	}

	if (fboStatus == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT) {
		std::cout << "ERROR: FrameBuffer attachments are missing" << std::endl;
	}

	if (fboStatus == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) {
		std::cout << "ERROR: FrameBuffer attachments are incomplete" << std::endl;
	}

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

unsigned int FrameBuffer::getID() {
	return m_RendererID;
}

unsigned int FrameBuffer::getColorBuffer() {
	return m_ColorBuffer;
}

unsigned int FrameBuffer::getDepthBuffer() {
	return m_DepthBuffer;
}

unsigned int FrameBuffer::getColorTexture() {
	return m_ColorTextureID;
}

unsigned int FrameBuffer::getDepthTexture() {
	return m_DepthTextureID;
}
