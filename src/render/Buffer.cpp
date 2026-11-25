#include "Buffer.h"
#include <cassert>
#include <glad/gl.h>
#include <utility>

Buffer::Buffer(unsigned buffer_size): buffer_size_(buffer_size) {
    glCreateBuffers(1, &buffer);
    glNamedBufferStorage(
        buffer, buffer_size, nullptr, GL_DYNAMIC_STORAGE_BIT
    );
}

Buffer::Buffer(Buffer && rhs) {
    buffer = std::exchange(rhs.buffer, 0);
    buffer_size_ = std::exchange(rhs.buffer_size_, 0);
}

Buffer& Buffer::operator=(Buffer && rhs) {
    delete_buffer();
    buffer = std::exchange(rhs.buffer, 0);
    buffer_size_ = std::exchange(rhs.buffer_size_, 0);

    return *this;
}

void Buffer::bind(unsigned target) {
    assert(buffer);
    glBindBuffer(target, buffer);
}

void Buffer::write(unsigned offset, unsigned size, const void* data) {
    assert(buffer);
    assert(offset + size <= buffer_size_);
    glNamedBufferSubData(
        buffer, offset, size, data
    );
}
void Buffer::read(unsigned offset, unsigned size, void* data) {
    assert(buffer);
    assert(offset + size <= buffer_size_);
    glGetNamedBufferSubData(
        buffer, offset, size, data
    );
}

void Buffer::bind_buffer_base(unsigned binding) {
    assert(buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, buffer);
}

Buffer::~Buffer() {
    delete_buffer();
}

void Buffer::delete_buffer() {
    if (buffer) {
        glDeleteBuffers(1, &buffer);
        buffer = 0;
        buffer_size_ = 0;
    }
}

