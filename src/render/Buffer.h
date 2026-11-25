#include <vector>

class Buffer {
public:
    Buffer() = default;
    Buffer(unsigned buffer_size);
    Buffer(Buffer &&);
    Buffer(const Buffer &) = delete;
    Buffer &operator=(Buffer &&);
    Buffer &operator=(const Buffer &) = delete;
    ~Buffer();

    template<typename T>
    void write(const std::vector<T> data) {
        write(0, data.size() * sizeof(T), data.data());
    }

    void write(unsigned offset, unsigned size, const void* data);
    void read(unsigned offset, unsigned size, void* data);

    void bind(unsigned target);
    void bind_buffer_base(unsigned binding);

    unsigned size() { return buffer_size_; }


private:
    void delete_buffer();

    unsigned buffer = 0;
    unsigned buffer_size_ = 0;
};
