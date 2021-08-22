#ifndef _VZ_FILE_UTILS_H_
#define _VZ_FILE_UTILS_H_

#include <stdint.h>

#include <string>
#include <vector>
// #include <mutex>

#include "vz_data.h"

namespace lds {

class FileUtils {
 public:
  static FileUtils *Instance();

  // static void SetDelegate(FileUtils *delegate);

  ~FileUtils();

 protected:
  FileUtils();

 private:
  static FileUtils *shared_file_;
};

}  // namespace lds

#endif  // _VZ_FILE_UTILS_H_