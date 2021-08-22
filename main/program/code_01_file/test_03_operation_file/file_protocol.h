#ifndef _OPERATION_FILE_PROTOCOL_H_
#define _OPERATION_FILE_PROTOCOL_H_

#include <unistd.h>
#include <stdint.h>
#include <string>
#include <vector>

namespace vzopera {

class FileProtocol {
 public:
  FileProtocol();
  virtual ~FileProtocol();

 public:
  virtual int32_t UpdateReadIndex(void *ctx, uint32_t count) = 0;
  virtual int32_t WriteString(void *ctx, char *str) = 0;
  virtual int32_t ReadString(void *ctx, char *str, size_t size) = 0;
  virtual int32_t ReadArrayString(void *ctx, int n, std::vector<std::string> &result) = 0;
  virtual int32_t Write(void *ctx, void *data, size_t size) = 0;
  virtual int32_t Read(void *ctx, void *data, size_t size) = 0;
};

}  // namespace vzopera

#endif /* _OPERATION_FILE_PROTOCOL_H_ */
