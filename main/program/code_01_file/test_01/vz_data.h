#ifndef _VZ_DATA_H_
#define _VZ_DATA_H_

#include <stdint.h>

#include <string>  // for ssize_t on linux

#include "vz_typedef.h"

namespace lds {

class Data {
 public:
  Data();
  Data(const Data &other);
  Data(Data &&other);
  ~Data();

  Data &operator= (const Data &other);
  Data &operator= (Data &&other);

  unsigned char *get_bytes() const;
  //   unsigned char* GetBytes() const;

  ssize_t get_size() const;

  ssize_t copy(const unsigned char *bytes, const ssize_t size);

  void fast_set(unsigned char *bytes, const ssize_t size);

  void clear();

  bool is_null() const;

  unsigned char *take_buffer(ssize_t *size);

 private:
  void move(Data &other);

 private:
  unsigned char *bytes_;
  ssize_t size_;
};

}  // namespace lds

#endif  // _VZ_DATA_H_