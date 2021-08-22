#ifndef _OPERATION_FILE_SERIALIZER_H_
#define _OPERATION_FILE_SERIALIZER_H_

#include "operation_typedef.h"
#include <string>

namespace vzopera {

enum SerializeSeekType {
  SERIALIZE_SEEK_START = 0,
  SERIALIZE_SEEK_CURRENT,
  SERIALIZE_SEEK_END,
};

class FileSerializer {
 public:
  FileSerializer();
  ~FileSerializer();

 public:
  virtual size_t Read(void *file, void *buffer, size_t size);
  virtual size_t Write(void *file, void *buffer, size_t size);
  virtual int64_t Seek(void *file, int64_t offset, int origin);
  virtual int64_t GetPos(void *file);
};

/** reader **/
class FileReader {
 public:
  FileReader(FileSerializer *reader);
  ~FileReader();

 public:
  size_t Read(void *buffer, size_t size);
  size_t Write(void *buffer, size_t size);
  int64_t Seek(int64_t offset, int origin);
  int64_t GetPosition();

  int Open(std::string &path);
  // void Close(void *file);
  void Close(void);

public:
  bool ReadData(void *data, size_t size);
  bool ReadString(char *str, size_t &max);

 private:
  FILE *file_;
  std::string path_;

  FileSerializer *read_ops_;
};

/** writer **/
class FileWriter {
 public:
  FileWriter(FileSerializer *writer);
  ~FileWriter();

 public:
  size_t Read(void *buffer, size_t size);
  size_t Write(void *buffer, size_t size);
  int64_t Seek(int64_t offset, int origin);
  int64_t GetPosition();

  int Open(std::string &path);
  void Close(void);
  // void Close(void *file);

 public:
  bool ReadData(void *data, size_t size);
  bool WriteData(void *data, size_t size);
  bool ReadString(char *str, size_t &max);
  bool WriteString(char *str);

 public:
  int ExistFile(const char *filename);
  int CreatFile(const char *filename);
  int CreatOpen(const char *filename);

 private:
  FILE *file_;
  std::string path_;

  FileSerializer *write_ops_;
};

}  // namespace vzopera

#endif /* _OPERATION_FILE_SERIALIZER_H_ */
