#include "engine/gl.hpp"

#include <GLFW/glfw3.h>

namespace glw {
    void load() {
        gladLoadGL(glfwGetProcAddress);
    }

    std::vector<std::string> GetExtensions() {
        std::vector<std::string> extensions;
        int count = GetInteger(GL_NUM_EXTENSIONS);
        extensions.reserve(count);
        for (unsigned int i = 0 ; i < count ; i++) {
            extensions.push_back(GetStringi(GL_EXTENSIONS, i));
        }

        return extensions;
    }

    std::vector<std::string> GetSpirVExtensions() {
        std::vector<std::string> extensions;
        int count = GetInteger(GL_NUM_SPIR_V_EXTENSIONS);
        extensions.reserve(count);
        for (unsigned int i = 0 ; i < count ; i++) {
            extensions.push_back(GetStringi(GL_SPIR_V_EXTENSIONS, i));
        }

        return extensions;
    }

    int GetInteger(GLenum pname) {
        int n;
        glGetIntegerv(pname, &n);
        return n;
    }

    int GetIntegeri(GLenum pname, GLuint i) {
        int n;
        glGetIntegeri_v(pname, i, &n);
        return n;
    }

    std::string GetString(GLenum pname) {
        return reinterpret_cast<const char*>(glGetString(pname));
    }

    std::string GetStringi(GLenum pname, GLuint i) {
        return reinterpret_cast<const char*>(glGetStringi(pname, i));
    }

    Buffer::Buffer(size_t size, const void *data, Usage usage) {
        glCreateBuffers(1, &m_Buffer);
        set(size, data, usage);

    }

    Buffer::~Buffer() {
        glDeleteBuffers(1, &m_Buffer);
    }

    std::shared_ptr<Buffer> Buffer::create(size_t size, const void *data, Usage usage) {
        return std::make_shared<Buffer>(size, data, usage);
    }

    std::unique_ptr<Buffer> Buffer::createUnique(size_t size, const void *data, Usage usage) {
        return std::make_unique<Buffer>(size, data, usage);
    }

    void Buffer::set(size_t size, const void *data) {
        if (size != m_CurrentSize)
            glNamedBufferData(m_Buffer, size, data, static_cast<GLenum>(m_CurrentUsage));
        else
            subdata(size, data, 0);
    }

    void Buffer::set(size_t size, const void *data, Buffer::Usage usage) {
        if (size != m_CurrentSize || usage != m_CurrentUsage)
            glNamedBufferData(m_Buffer, size, data, static_cast<GLenum>(usage));
        else
            subdata(size, data, 0);
    }

    void Buffer::subdata(size_t size, const void *data, size_t offset) {
        glNamedBufferSubData(m_Buffer, offset, size, data);
    }

    void Buffer::bind(GLenum target) {
        glBindBuffer(target, m_Buffer);
    }

    void Buffer::bind(Buffer::Target target) {
        bind(static_cast<GLenum>(target));
    }

    void Buffer::bindBase(GLenum target, unsigned int index) {
        glBindBufferBase(target, index, m_Buffer);
    }

    void Buffer::bindBase(Buffer::Target target, unsigned int index) {
        bindBase(static_cast<GLenum>(target), index);
    }

    void Buffer::bindRange(GLenum target, unsigned int index, const engine::range<size_t> &range) {
        glBindBufferRange(target, index, m_Buffer, range.offset, range.size);
    }

    void Buffer::bindRange(Buffer::Target target, unsigned int index, const engine::range<size_t> &range) {
        bindRange(static_cast<GLenum>(target), index, range);
    }

    void Buffer::storage(size_t size, const void *data, Buffer::Usage usage) {
        m_CurrentUsage = usage;
        glNamedBufferStorage(m_Buffer, size, data, static_cast<GLenum>(usage));
    }

    void Buffer::clear(GLenum internalFormat, GLenum format, GLenum type, const void *data) {
        glClearNamedBufferData(m_Buffer, internalFormat, format, type, data);
    }

    void Buffer::clear(size_t offset, size_t size, GLenum internalFormat, GLenum format, GLenum type, const void *data) {
        glClearNamedBufferSubData(m_Buffer, internalFormat, offset, size, format, type, data);
    }

    void Buffer::copyTo(Buffer *destination) {
        size_t s = std::min(m_CurrentSize, destination->m_CurrentSize);
        copyTo(destination, {0, s}, 0);
    }

    void Buffer::copyTo(Buffer *destination, size_t size) {
        copyTo(destination, {0, size}, 0);
    }

    void Buffer::copyTo(Buffer *destination, const engine::range<size_t> &sourceRange, size_t destinationOffset) {
        glCopyNamedBufferSubData(m_Buffer, destination->m_Buffer, sourceRange.offset, destinationOffset, sourceRange.size);
    }

    void Buffer::copyTo(const std::shared_ptr<Buffer> &destination) {
        copyTo(destination.get());
    }

    void Buffer::copyTo(const std::shared_ptr<Buffer> &destination, size_t size) {
        copyTo(destination.get(), size);
    }

    void Buffer::copyTo(const std::shared_ptr<Buffer> &destination, const engine::range<size_t> &sourceRange, size_t destinationOffset) {
        copyTo(destination.get(), sourceRange, destinationOffset);
    }

    void Buffer::copyTo(const std::unique_ptr<Buffer> &destination) {
        copyTo(destination.get());
    }

    void Buffer::copyTo(const std::unique_ptr<Buffer> &destination, size_t size) {
        copyTo(destination.get(), size);
    }

    void Buffer::copyTo(const std::unique_ptr<Buffer> &destination, const engine::range<size_t> &sourceRange, size_t destinationOffset) {
        copyTo(destination.get(), sourceRange, destinationOffset);
    }

    void Buffer::flushMappedRange(const engine::range<size_t> &range) const {
        glFlushMappedNamedBufferRange(m_Buffer, range.offset, range.size);
    }

    int Buffer::getBufferAccessParameter() const {
        int i;
        glGetNamedBufferParameteriv(m_Buffer, GL_BUFFER_ACCESS_FLAGS, &i);
        return i;
    }

    bool Buffer::isImmutableStorage() const {
        int b;
        glGetNamedBufferParameteriv(m_Buffer, GL_BUFFER_IMMUTABLE_STORAGE, &b);
        return b == GL_TRUE;
    }

    bool Buffer::isMapped() const {
        int b;
        glGetNamedBufferParameteriv(m_Buffer, GL_BUFFER_MAPPED, &b);
        return b == GL_TRUE;
    }

    int64_t Buffer::getMappedLength() const {
        int64_t i;
        glGetNamedBufferParameteri64v(m_Buffer, GL_BUFFER_MAP_LENGTH, &i);
        return i;
    }

    int64_t Buffer::getMappedOffset() const {
        int64_t i;
        glGetNamedBufferParameteri64v(m_Buffer, GL_BUFFER_MAP_OFFSET, &i);
        return i;
    }

    void *Buffer::getMappedPointer() const {
        void* p;
        glGetNamedBufferPointerv(m_Buffer, GL_BUFFER_MAP_POINTER, &p);
        return p;
    }

    engine::cpu_memory Buffer::getSubData(const engine::range<size_t> &range) {
        auto mem = engine::cpu_memory(range.size);
        glGetNamedBufferSubData(m_Buffer, range.offset, range.size, mem.data);
        return mem;
    }

    void *Buffer::map(GLenum access) {
        return glMapNamedBuffer(m_Buffer, access);
    }

    void *Buffer::map(GLenum access, const engine::range<size_t> &range) {
        return glMapNamedBufferRange(m_Buffer, range.offset, range.size, access);
    }

    VertexArray::VertexArray() {
        glCreateVertexArrays(1, &m_VertexArray);
    }

    VertexArray::~VertexArray() {
        glDeleteVertexArrays(1, &m_VertexArray);
    }

    void VertexArray::bindVertexBuffer(const Buffer *buffer, const std::vector<size_t> &sizes, size_t offset) {
        size_t stride = 0;
        unsigned int binding = m_NextBinding++;

        for (const auto& asize : sizes) {
            unsigned int attrib = m_NextAttribute++;
            glVertexArrayAttribBinding(m_VertexArray, attrib, binding);
            glVertexArrayAttribFormat(m_VertexArray, attrib, asize, GL_FLOAT, GL_FALSE, stride);
            glEnableVertexArrayAttrib(m_VertexArray, attrib);
            stride += asize * sizeof(float);
        }

        glVertexArrayVertexBuffer(m_VertexArray, binding, buffer->getHandle(), offset, stride);
    }

    void VertexArray::bindVertexBuffer(const Buffer *buffer, const std::vector<Attribute> &attribs, size_t stride, size_t offset) {
        unsigned int binding = m_NextBinding++;

        for (const auto& a : attribs) {
            unsigned int attrib = m_NextAttribute++;
            glVertexArrayAttribBinding(m_VertexArray, attrib, binding);
            glVertexArrayAttribFormat(m_VertexArray, attrib, a.size, GL_FLOAT, GL_FALSE, a.offset);
            glEnableVertexArrayAttrib(m_VertexArray, attrib);
        }

        glVertexArrayVertexBuffer(m_VertexArray, binding, buffer->getHandle(), offset, stride);
    }

    void VertexArray::bindElementBuffer(const Buffer *buffer) {
        glVertexArrayElementBuffer(m_VertexArray, buffer->getHandle());
        m_ElementBufferBound = true;
    }

    void VertexArray::bind() const {
        glBindVertexArray(m_VertexArray);
    }

    GenericTexture::GenericTexture(GenericTexture::Type type) {
        glCreateTextures(static_cast<GLenum>(type), 1, &m_Texture);
    }

    GenericTexture::~GenericTexture() {
        glDeleteTextures(1, &m_Texture);
    }

    void GenericTexture::bind(GLenum type) const {
        glBindTexture(type, m_Texture);
    }

    void GenericTexture::bind(GenericTexture::Type type) const {
        bind(static_cast<GLenum>(type));
    }

    void GenericTexture::setActiveTextureUnit(uint8_t n) {
        glActiveTexture(GL_TEXTURE0 + n);
    }
}
