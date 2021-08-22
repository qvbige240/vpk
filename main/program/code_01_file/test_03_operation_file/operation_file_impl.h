#ifndef _OPERATION_LOG_IMPL_H_
#define _OPERATION_LOG_IMPL_H_

#include <mutex>
#include <string>
#include "file_protocol.h"
#include "file_serializer.h"

namespace vzopera {

// #define FILE_DATA_MAX_NUM  4000
#define FILE_DATA_MAX_NUM  400
#define FILE_DATA_MAX_LEN  1000

typedef struct ProtocolHead_ {
  uint8_t w_flag;
  uint8_t version;
  uint16_t reserve1;
  uint32_t cover;
  uint32_t num;  /* FILE_DATA_MAX_NUM 4000 */
  uint32_t size; /* FILE_DATA_MAX_LEN 1000 */
  uint64_t total;
  uint32_t r_index;
  uint32_t w_index;
  uint64_t carry;
  uint64_t r_pos;
  uint64_t w_pos;
  uint32_t start;
  uint32_t reserve2;
  uint64_t end;
  uint64_t reserve3;
} ProtocolHead;

typedef struct FileHeadData_ {
  ProtocolHead head;
  uint32_t     timestamp[FILE_DATA_MAX_NUM];
} FileHeadData;

class OperaFileProtocol : virtual public FileProtocol {
 public:
  OperaFileProtocol(std::string filename);
  virtual ~OperaFileProtocol();

 public:
  int32_t Init();

  virtual int32_t WriteString(void *ctx, char *str);
  virtual int32_t ReadString(void *ctx, char *str, size_t size);
  virtual int32_t ReadArrayString(void *ctx, int n, std::vector<std::string> &result);
  virtual int32_t Write(void *ctx, void *data, size_t size);
  virtual int32_t Read(void *ctx, void *data, size_t size);

  virtual int32_t UpdateReadIndex(void *ctx, uint32_t count);

 private:
  std::mutex mutex_;
  std::string filename_;
  FileHeadData proto_head_;

  FileWriter *file_writer_;
  FileReader *file_reader_;
  FileSerializer *fops_;
};

}  // namespace vzopera

#endif /* _OPERATION_LOG_IMPL_H_ */
