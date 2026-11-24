class Buffer {
public:
    Buffer() = default;
    Buffer(unsigned buffer_size);
    Buffer(Buffer &&);
    Buffer(const Buffer &) = delete;
    Buffer &operator=(Buffer &&);
    Buffer &operator=(const Buffer &) = delete;
    ~Buffer();

    void bind(unsigned target);
    void write(unsigned offset, unsigned size, const void* data);
    void bind_buffer_base(unsigned binding);


private:
    void delete_buffer();

    unsigned buffer = 0;
    unsigned buffer_size_ = 0;
};
