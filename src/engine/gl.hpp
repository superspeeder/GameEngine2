#pragma once

#include <glad/gl.h>

#include <vector>
#include <array>
#include <string>
#include <memory>

#include "engine/engine.hpp"

namespace glw {
    void load();

    std::vector<std::string> GetExtensions();
    std::vector<std::string> GetSpirVExtensions();

    int GetInteger(GLenum pname);

    template<size_t N>
    std::array<int, N> GetIntegerN(GLenum pname) {
        static_assert(N > 1, "For single integers, use glw::GetInteger(GLenum)");
        std::array<int, N> arr;
        glGetIntegerv(pname, arr.data());
        return arr;
    };

    int GetIntegeri(GLenum pname, GLuint i);

    std::string GetString(GLenum pname);
    std::string GetStringi(GLenum pname, GLuint i);

    enum class AccessMode : GLenum {
        ReadOnly = GL_READ_ONLY,
        WriteOnly = GL_WRITE_ONLY,
        ReadWrite = GL_READ_WRITE,
    };

    enum class InternalFormat : GLenum {
        RGBA32F = GL_RGBA32F,
        RGBA16F = GL_RGBA16F,
        RG32F = GL_RG32F,
        RG16F = GL_RG16F,
        R11F_G11F_B10F = GL_R11F_G11F_B10F,
        R32F = GL_R32F,
        R16F = GL_R16F,
        RGBA32UI = GL_RGBA32UI,
        RGBA16UI = GL_RGBA16UI,
    };

    class Buffer {
      public:
        enum class Target : GLenum {
            Array = GL_ARRAY_BUFFER,
            ElementArray = GL_ELEMENT_ARRAY_BUFFER,
            Texture = GL_TEXTURE_BUFFER,
            PixelPack = GL_PIXEL_PACK_BUFFER,
            PixelUnpack = GL_PIXEL_UNPACK_BUFFER,
            AtomicCounter = GL_ATOMIC_COUNTER_BUFFER,
            CopyRead = GL_COPY_READ_BUFFER,
            CopyWrite = GL_COPY_WRITE_BUFFER,
            DispatchIndirect = GL_DISPATCH_INDIRECT_BUFFER,
            DrawIndirect = GL_DRAW_INDIRECT_BUFFER,
            Query = GL_QUERY_BUFFER,
            ShaderStorage = GL_SHADER_STORAGE_BUFFER,
            Uniform = GL_UNIFORM_BUFFER,
            TransformFeedback = GL_TRANSFORM_FEEDBACK_BUFFER,
        };

        enum class Usage : GLenum {
            StaticDraw = GL_STATIC_DRAW,
            StaticRead = GL_STATIC_READ,
            StaticCopy = GL_STATIC_COPY,
            DynamicDraw = GL_DYNAMIC_DRAW,
            DynamicRead = GL_DYNAMIC_READ,
            DynamicCopy = GL_DYNAMIC_COPY,
            StreamDraw = GL_STREAM_DRAW,
            StreamRead = GL_STREAM_READ,
            StreamCopy = GL_STREAM_COPY,
            Unset = 0,
        };

        Buffer(size_t size, const void* data = nullptr, Usage usage = Usage::DynamicDraw);
        ~Buffer();

        static std::shared_ptr<Buffer> create(size_t size, const void* data = nullptr, Usage usage = Usage::DynamicDraw);
        static std::unique_ptr<Buffer> createUnique(size_t size, const void* data = nullptr, Usage usage = Usage::DynamicDraw);

        template<typename T>
        static std::shared_ptr<Buffer> create(const std::vector<T>& data, Usage usage = Usage::DynamicDraw) {
            return create(data.size() * sizeof(T), data.data(), usage);
        };

        template<typename T>
        static std::unique_ptr<Buffer> create_unique(const std::vector<T>& data, Usage usage = Usage::DynamicDraw) {
            return createUnique(data.size() * sizeof(T), data.data(), usage);
        };

        void set(size_t size, const void* data);
        void set(size_t size, const void* data, Usage usage);

        template<typename T>
        void set(const std::vector<T>& data) {
            set(data.size() * sizeof(T), data.data());
        };

        template<typename T>
        void set(const std::vector<T>& data, Usage usage) {
            set(data.size() * sizeof(T), data.data(), usage);
        };

        void storage(size_t size, const void* data, Usage usage);

        template<typename T>
        void storage(const std::vector<T>& data, Usage usage) {
            storage(data.size() * sizeof(T), data.data(), usage);
        };

        void subdata(size_t size, const void* data, size_t offset = 0);

        template<typename T>
        void subdata(const std::vector<T>& data, size_t offset = 0) {
            subdata(data.size() * sizeof(T), data.data(), offset);
        };

        void bind(GLenum target);
        void bind(Target target);

        void bindBase(GLenum target, unsigned int index);
        void bindBase(Target target, unsigned int index);

        void bindRange(GLenum target, unsigned int index, const engine::range<size_t> &range);
        void bindRange(Target target, unsigned int index, const engine::range<size_t> &range);

        void clear(GLenum internalFormat, GLenum format, GLenum type, const void* data = nullptr);
        void clear(size_t offset, size_t size, GLenum internalFormat, GLenum format, GLenum type, const void* data = nullptr);

        void copyTo(Buffer* destination);
        void copyTo(Buffer* destination, size_t size);
        void copyTo(Buffer* destination, const engine::range<size_t> &sourceRange, size_t destinationOffset = 0);

        void copyTo(const std::shared_ptr<Buffer> &destination);
        void copyTo(const std::shared_ptr<Buffer> &destination, size_t size);
        void copyTo(const std::shared_ptr<Buffer> &destination, const engine::range<size_t> &sourceRange, size_t destinationOffset = 0);

        void copyTo(const std::unique_ptr<Buffer> &destination);
        void copyTo(const std::unique_ptr<Buffer> &destination, size_t size);
        void copyTo(const std::unique_ptr<Buffer> &destination, const engine::range<size_t> &sourceRange, size_t destinationOffset = 0);

        void flushMappedRange(const engine::range<size_t>& range) const;

        [[nodiscard]] int getBufferAccessParameter() const;

        [[nodiscard]] bool isImmutableStorage() const;
        [[nodiscard]] bool isMapped() const;

        [[nodiscard]] int64_t getMappedLength() const;
        [[nodiscard]] int64_t getMappedOffset() const;

        [[nodiscard]] inline size_t getCurrentSize() const noexcept { return m_CurrentSize; };
        [[nodiscard]] inline Usage getCurrentUsage() const noexcept { return m_CurrentUsage; };
        [[nodiscard]] inline unsigned int getHandle() const noexcept { return m_Buffer; };

        [[nodiscard]] void* getMappedPointer() const;

        [[nodiscard]] engine::cpu_memory getSubData(const engine::range<size_t>& range);

        [[nodiscard]] void* map(GLenum access);
        [[nodiscard]] void* map(GLenum access, const engine::range<size_t>& range);

      private:
        Usage m_CurrentUsage = Usage::Unset;
        size_t m_CurrentSize = 0;
        unsigned int m_Buffer = 0;
    };

    class VertexArray {
      public:
        struct Attribute {
            size_t offset;
            size_t size;
        };

        VertexArray();
        ~VertexArray();

        inline static std::shared_ptr<VertexArray> create() { return std::make_shared<VertexArray>(); };
        inline static std::unique_ptr<VertexArray> create_unique() { return std::make_unique<VertexArray>(); };

        void bindVertexBuffer(const Buffer* buffer, const std::vector<size_t>& sizes, size_t offset = 0);
        void bindVertexBuffer(const Buffer* buffer, const std::vector<Attribute>& attribs, size_t stride, size_t offset = 0);

        inline void bindVertexBuffer(const std::shared_ptr<Buffer> &buffer, const std::vector<size_t>& sizes, size_t offset = 0) {
            bindVertexBuffer(buffer.get(), sizes, offset);
        };

        inline void bindVertexBuffer(const std::shared_ptr<Buffer> &buffer, const std::vector<Attribute>& attribs, size_t stride, size_t offset = 0) {
            bindVertexBuffer(buffer.get(), attribs, stride, offset);
        };

        inline void bindVertexBuffer(const std::unique_ptr<Buffer> &buffer, const std::vector<size_t>& sizes, size_t offset = 0) {
            bindVertexBuffer(buffer.get(), sizes, offset);
        };

        inline void bindVertexBuffer(const std::unique_ptr<Buffer> &buffer, const std::vector<Attribute>& attribs, size_t stride, size_t offset = 0) {
            bindVertexBuffer(buffer.get(), attribs, stride, offset);
        };

        void bindElementBuffer(const Buffer* buffer);

        inline void bindElementBuffer(const std::shared_ptr<Buffer>& buffer) {
            bindElementBuffer(buffer.get());
        };

        inline void bindElementBuffer(const std::unique_ptr<Buffer>& buffer) {
            bindElementBuffer(buffer.get());
        };

        void bind() const;

        [[nodiscard]] inline unsigned int getHandle() const noexcept { return m_VertexArray; };

        [[nodiscard]] inline bool isElementBufferBound() const noexcept { return m_ElementBufferBound; };

        [[nodiscard]] inline unsigned int getNextBinding() const noexcept { return m_NextBinding; };

        [[nodiscard]] inline unsigned int getNextAttribute() const noexcept { return m_NextAttribute; };

      private:
        unsigned int m_VertexArray = 0;
        unsigned int m_NextAttribute = 0;
        unsigned int m_NextBinding = 0;
        bool m_ElementBufferBound = false;
    };

    class GenericTexture {
      public:
        enum class Type : GLenum {
            Texture1D = GL_TEXTURE_1D,
            Texture2D = GL_TEXTURE_2D,
            Texture3D = GL_TEXTURE_3D,
            Texture2DMS = GL_TEXTURE_2D_MULTISAMPLE,
            TextureCubemap = GL_TEXTURE_CUBE_MAP,
            TextureRectangle = GL_TEXTURE_RECTANGLE,
            TextureBuffer = GL_TEXTURE_BUFFER,
            Texture1DArray = GL_TEXTURE_1D_ARRAY,
            Texture2DArray = GL_TEXTURE_2D_ARRAY,
            Texture2DMSArray = GL_TEXTURE_2D_MULTISAMPLE_ARRAY,
            TextureCubemapArray = GL_TEXTURE_CUBE_MAP_ARRAY,
        };

        explicit GenericTexture(Type type);
        ~GenericTexture();

        void bind(GLenum type) const;
        void bind(Type type) const;

        static void setActiveTextureUnit(unsigned int n);

        void bindImage(unsigned int unit, int level, bool layered, int layer, AccessMode accessMode, GLenum format);
        void bindImage(unsigned int unit, int level, bool layered, int layer, AccessMode accessMode, Format format);

      private:
        unsigned int m_Texture;


    };

    class Texture1D {

    };

    class Texture2D {

    };

    class Texture3D {

    };

    class TextureCubemap {

    };

    class Texture1DArray {

    };

    class Texture2DArray {

    };

    class TextureCubemapArray {

    };

    class Texture2DMS {

    };

    class Texture2DMSArray {

    };

    class TextureRectangle {

    };
    class Framebuffer {

    };

    class Renderbuffer {

    };
}