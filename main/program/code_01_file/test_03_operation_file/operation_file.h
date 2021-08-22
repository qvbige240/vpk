#ifndef _OPERATION_FILE_MANAGER_H_
#define _OPERATION_FILE_MANAGER_H_

#include <stdint.h>
#include <unistd.h>
#include <map>
#include <mutex>
#include <string>
#include <vector>

#include "singleton.h"

// #include "eventservice/base/criticalsection.h"

// #include "file_protocol.h"

namespace vzopera {

class FileProtocol;

typedef std::map<int, FileProtocol*> MapFileProtocol;

enum FileProtoType { 
  OPERA_FILE_PROTO_LOG = 0,
  OPERA_FILE_PROTO_SNAP,
  // OPERA_FILE_MAX_NUM
};

// class OperaFileManager {
class OperaFileManager : public vzes::Singleton<OperaFileManager> {
  friend class vzes::Singleton<OperaFileManager>;
  
 private:
  OperaFileManager() : is_init_(false) {}
  OperaFileManager(const OperaFileManager &o) : is_init_(false) {}
  OperaFileManager &operator=(const OperaFileManager &o) = default;
  virtual ~OperaFileManager();

 public:
  /* this function not thread safe */
  // static OperaFileManager *Instance();
  
  int Init();

  /**
   * Write string data to type file.
   *
   * @param type	 The file type within enum FileProtoType.
   * @param data   The target string data to write.
   *
   * @return -1: error; 0: successful.
   */
  int32_t Write(int type, std::string &data);

  /**
   * Read string data from type file.
   *
   * @param type   The file type.
   * @param buffer Read data to this pointer.
   * @param size   buffer max size
   *
   * @return -1: error, 0: no data, >0: successful.
   */
  int32_t Read(int type, void *buffer, size_t size);

  /**
   * Read vector string data from type file.
   *
   * @param type   The file type.
   * @param n      The expect number of data to read.
   * @param result Vector string data, read from type file to fill it.
   *
   * @return -1: error, 0: no data, >0: data numbers, equal to vector size().
   */
  int32_t ReadArrayString(int type, int n, std::vector<std::string> &result);

  /**
   * Write and update the current read index in type file to the latest unread data.
   *
   * @param queue	 The file type within enum FileProtoType.
   * @param count  The num of which have been sent.
   *
   * @return -1: error; 0: successful.
   */
  int32_t UpdateReadIndex(int type, uint32_t count);

 protected:
  FileProtocol *FindProtocol(int type);
  int TouchTypeFile(int type, std::string &filename);

//  private:
//   OperaFileManager();
//   virtual ~OperaFileManager();

 private:
  // vzes::CriticalSection mutex_;
  bool is_init_;
  std::mutex mutex_;
  char folder_path_[256];
  MapFileProtocol file_protocol_;
};

}  // namespace vzopera

#endif /* _OPERATION_FILE_MANAGER_H_ */
