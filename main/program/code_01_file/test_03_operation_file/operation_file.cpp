#include "singleton.h"

#include "operation_file.h"

#include "file_protocol.h"
#include "operation_file_impl.h"

namespace vzopera {

#define PATH_MAX_LEN 256

typedef struct FileTypeMap_ {
  enum FileProtoType    type;
  const char           *filename;
} FileTypeMap;

static FileTypeMap type_mapping[] =
{
  { OPERA_FILE_PROTO_LOG,  "app_log.vz" },
  { OPERA_FILE_PROTO_SNAP, "app_snap.vz" }
};

//OperaFileManager::OperaFileManager() {}
OperaFileManager::~OperaFileManager() {
  MapFileProtocol::iterator it;
  for (it=file_protocol_.begin(); it != file_protocol_.end(); ++it) {
    FileProtocol *p = it->second;
    vz_freep(p);
  }
  file_protocol_.clear();
}

// TODO use Singleton
// OperaFileManager *OperaFileManager::Instance() {
//   static OperaFileManager *instance_mgr = NULL;
//   if (instance_mgr == NULL) {
//     // std::lock_guard<std::mutex> lock(mutex_);
//     if (instance_mgr == NULL) {
//       instance_mgr = new OperaFileManager();
//       OPERA_LOGE("OperaFileManager create %p\n", instance_mgr);
//       instance_mgr->Init();
//     }
//   }
//   return instance_mgr;
// }

int OperaFileManager::Init() {
  if (is_init_) return 0;
  is_init_ = true;
  OPERA_LOGD("##### OperaFileManager Init %p\n", this);

  const char *path = "./";  // need check path exist
  // folder_path_
  char fullname[PATH_MAX_LEN] = {0};
  for (int i = 0; i < sizeof(type_mapping) / sizeof(type_mapping[0]); i++) {
    snprintf(fullname, sizeof(fullname) - 1, "%s/%s", path, type_mapping[i].filename);
    OPERA_LOGD("[%d]fullname: %s\n", i, fullname);
    // make_dir_p
    std::string filename(fullname, strlen(fullname));
    TouchTypeFile(type_mapping[i].type, filename);
  }
  return 0;
}

FileProtocol *OperaFileManager::FindProtocol(int type) {
  FileProtocol *ruler = NULL;
  MapFileProtocol::iterator it;
  // lock
  // std::lock_guard<std::mutex> lock(mutex_);
  if ((it = file_protocol_.find(type)) != file_protocol_.end()) {
    ruler = it->second;
  }

  return ruler;
}

int OperaFileManager::TouchTypeFile(int type, std::string &filename) {
  // std::string filename = "./log_01.txt";
  // OperaFileProtocol *log =
  // dynamic_cast<OperaFileProtocol*>(FindProtocol(type));
  std::lock_guard<std::mutex> lock(mutex_);
  if (FindProtocol(type) == NULL) {
    OperaFileProtocol *proto = new OperaFileProtocol(filename);
    file_protocol_.insert(std::make_pair(type, proto));

    proto->Init();
  }
  return 0;
}

int32_t OperaFileManager::Write(int type, std::string &data) {
  std::lock_guard<std::mutex> lock(mutex_);
  FileProtocol *ruler = FindProtocol(type);
  if (ruler == NULL) {
    OPERA_LOGE("Cant't find file type %d\n", type);
    return -1;
  }

  return ruler->WriteString(NULL, (char *)data.c_str());
}

int32_t OperaFileManager::Read(int type, void *buffer, size_t size) {
  std::lock_guard<std::mutex> lock(mutex_);
  FileProtocol *ruler = FindProtocol(type);
  if (ruler == NULL) {
    OPERA_LOGE("Cant't find file type %d\n", type);
    return -1;
  }

  return ruler->ReadString(NULL, (char *)buffer, size);
}

int32_t OperaFileManager::ReadArrayString(int type, int n, std::vector<std::string> &result) {
  std::lock_guard<std::mutex> lock(mutex_);
  FileProtocol *ruler = FindProtocol(type);
  if (ruler == NULL) {
    OPERA_LOGE("Cant't find file type %d\n", type);
    return -1;
  }
  return ruler->ReadArrayString(NULL, n, result);
}

int32_t OperaFileManager::UpdateReadIndex(int type, uint32_t count) {
  std::lock_guard<std::mutex> lock(mutex_);
  FileProtocol *ruler = FindProtocol(type);
  if (ruler == NULL) {
    OPERA_LOGE("Cant't find file type %d\n", type);
    return -1;
  }
  return ruler->UpdateReadIndex(NULL, count);
}

}  // namespace vzopera
