#include "vz_data.h"

namespace lds {

Data::Data() : bytes_(nullptr), size_(0) {
  printf("In the empty constructor of Data.\n");
}

Data::Data(const Data &other) : bytes_(nullptr), size_(0) {
  printf("In the copy constructor of Data.\n");
  if (other.bytes_ && other.size_) {
    copy(other.bytes_, other.size_);
  }
}

Data::Data(Data &&other) : bytes_(nullptr), size_(0) {
  printf("In the move constructor of Data.\n");
  move(other);
}

Data::~Data() {
  printf("deallocing data: %p\n", this);
  clear();
}

Data &Data::operator=(const Data &other) {
  if (this != &other) {
    printf("In the copy assignment of data.\n");
    copy(other.bytes_, other.size_);
  }
  return *this;
}

Data &Data::operator=(Data &&other) {
  if (this != &other) {
    printf("In the move assignment of data.\n");
    move(other);
  }
  return *this;
}

unsigned char *Data::get_bytes() const { return bytes_; }

ssize_t Data::get_size() const { return size_; }

void Data::fast_set(unsigned char *bytes, const ssize_t size) {
  // assert(size >= 0, "fast_set size should be non-negative");
  bytes_ = bytes;
  size_ = size;
}

bool Data::is_null() const { return (bytes_ == nullptr || size_ == 0); }

unsigned char *Data::take_buffer(ssize_t *size);

void Data::clear() {
  if (bytes_) free(bytes_);
  bytes_ = nullptr;
  size_ = 0;
}

ssize_t Data::copy(const unsigned char *bytes, const ssize_t size) {
  // assert(size >= 0, "copy size should be non-negative");
  // assert(bytes, "bytes should not be nullptr");

  if (size_ <= 0) return 0;

  if (bytes_ != bytes) {
    clear();
    bytes_ = (unsigned char *)malloc(sizeof(unsigned char) * size);
    memcpy(bytes_, bytes, size);
  }

  size_ = size;
  return size_;
}

void Data::move(Data &other) {
  if (bytes_ != other.bytes_) clear();
  bytes_ = other.bytes_;
  size_ = other.size_;
}

}  // namespace lds
