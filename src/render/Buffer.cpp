#include "Buffer.h"
#include <cassert>
#include <glad/gl.h>

Buffer::Buffer(unsigned buffer_size): buffer_size_(buffer_size) {
    glCreateBuffers(1, &buffer);
    glNamedBufferStorage(
        buffer, buffer_size, nullptr, GL_DYNAMIC_STORAGE_BIT
    );
}

void Buffer::bind(unsigned target) {
    glBindBuffer(target, buffer);
}

void Buffer::write(unsigned offset, unsigned size, const void* data) {
    assert(offset + size <= buffer_size_);
    glNamedBufferSubData(
        buffer, offset, size, data
    );
}
void Buffer::bind_buffer_base(unsigned binding) {
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

