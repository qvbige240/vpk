#include "file_serializer.h"

#include <fcntl.h>
#include <unistd.h>  // fsync
#include <sys/stat.h>

namespace vzopera {

FileSerializer::FileSerializer() {}
FileSerializer::~FileSerializer() {}

size_t FileSerializer::Read(void *file, void *buffer, size_t size) {
  return fread(buffer, 1, size, (FILE *)file);
}

size_t FileSerializer::Write(void *file, void *buffer, size_t size) {
  return fwrite(buffer, 1, size, (FILE *)file);
}

int64_t FileSerializer::Seek(void *file, int64_t offset, int seek) {
  int origin = SEEK_SET;
  switch (seek) {
    case SERIALIZE_SEEK_START:
      origin = SEEK_SET;
      break;
    case SERIALIZE_SEEK_CURRENT:
      origin = SEEK_CUR;
      break;
    case SERIALIZE_SEEK_END:
      origin = SEEK_END;
    default:
      break;
  }

  if (fseeko((FILE *)file, offset, origin) == -1) return -1;

  return ftello((FILE *)file);
}

int64_t FileSerializer::GetPos(void *file) { return ftello((FILE *)file); }

/** reader **/
FileReader::FileReader(FileSerializer *reader) {
  file_ = NULL;
  read_ops_ = reader;
}
FileReader::~FileReader() {
  // if (read_ops_) {
  //   delete read_ops_;
  // }
  Close();
}

size_t FileReader::Read(void *buffer, size_t size) {
  return_val_if_fail(read_ops_ && buffer, 0);

  return read_ops_->Read(file_, buffer, size);
}

size_t FileReader::Write(void *buffer, size_t size) {
  return_val_if_fail(read_ops_ && buffer, 0);

  return read_ops_->Write(file_, buffer, size);
}

int64_t FileReader::Seek(int64_t offset, int origin) {
  return_val_if_fail(read_ops_ && file_, -1);

  return read_ops_->Seek(file_, offset, origin);
}

int64_t FileReader::GetPosition() {
  return_val_if_fail(read_ops_ && file_, -1);

  return read_ops_->GetPos(file_);
}

bool FileReader::ReadData(void *data, size_t size) {
  return (Read(data, size) == size);
}

bool FileReader::ReadString(char *str, size_t &max) {
  uint32_t len = 0;
  if (!ReadData(&len, sizeof(len))) {
    return false;
  }

  if (len > max) {
    printf("### read(%u) is greater than (%lu)dst buffer\n", len, max);
    return false;
  }
  max = (size_t)len;
  return ReadData(str, len);
}

int FileReader::Open(std::string &path) {
  file_ = fopen(path.c_str(), "rb");
  if (!file_) {
    printf("fopen file: \'%s\' failed!\n", path.c_str());
    return -1;
  }
  path_ = path;
  return 0;
}

// void FileReader::Close(void *file) { fclose((FILE *)file); }

void FileReader::Close(void) {
  if (file_) {
    // fflush((FILE *)file_);
    fclose((FILE *)file_);
  }
}

/** writer **/
FileWriter::FileWriter(FileSerializer *writer) {
  file_ = NULL;
  write_ops_ = writer;
}
FileWriter::~FileWriter() {
  // if (write_ops_) {
  //   delete write_ops_;
  // }

  Close();
}

size_t FileWriter::Read(void *buffer, size_t size) {
  return_val_if_fail(write_ops_ && file_ && buffer, 0);

  return write_ops_->Read(file_, buffer, size);
}

size_t FileWriter::Write(void *buffer, size_t size) {
  return_val_if_fail(write_ops_ && file_ && buffer, 0);

  if (size <= 0) {
    printf("warning: write size %lu\n", size);
    return 0;
  }

  return write_ops_->Write(file_, buffer, size);
}

int64_t FileWriter::Seek(int64_t offset, int origin) {
  return_val_if_fail(write_ops_ && file_, -1);

  return write_ops_->Seek(file_, offset, origin);
}

int64_t FileWriter::GetPosition() {
  return_val_if_fail(write_ops_ && file_, -1);

  return write_ops_->GetPos(file_);
}

bool FileWriter::ReadData(void *data, size_t size) {
  return (Read(data, size) == size);
}
bool FileWriter::WriteData(void *data, size_t size) {
  return (Write(data, size) == size);
}

bool FileWriter::ReadString(char *str, size_t &max) {
  uint32_t len = 0;
  if (!ReadData(&len, sizeof(len))) {
    return false;
  }

  if (len > max) {
    printf("### read(%u) is greater than (%lu)dst buffer\n", len, max);
    return false;
  }
  max = (size_t)len;
  return ReadData(str, len);
}

bool FileWriter::WriteString(char *str) {
  uint32_t len = (uint32_t)(str ? strlen(str) : 0);

  if (!WriteData(&len, sizeof(len))) {
    return false;
  }

  if (len && !WriteData(str, len)) {
    return false;
  }
  return true;
}

int FileWriter::CreatFile(const char *filename) {
  if (creat(filename, 0755) < 0) {
    printf("create file \'%s\' error\n", filename);
    return -1;
  }
  printf("create file: %s success\n", filename);
  return 0;
}
int FileWriter::ExistFile(const char *filename) {
  struct stat st;
  memset(&st, 0x00, sizeof(struct stat));

  if (stat(filename, &st) < 0) {
    printf("stat error, '\%s\' not exists or other\n", filename);
    return 0;
  }
  return S_ISREG(st.st_mode);
}
int FileWriter::CreatOpen(const char *filename) {
  int ret = -1;
  return_val_if_fail(filename, -1);

  if (!ExistFile(filename)) {
    ret = CreatFile(filename);
  }

  FILE *file = fopen(filename, "rb+");
  if (!file) {
    printf("fopen file: \'%s\' failed!\n", filename);
    return -1;
  }
  file_ = file;
  return ret;
}
int FileWriter::Open(std::string &path) {
  int ret = 0;
  file_ = fopen(path.c_str(), "rb+");
  // if (!file_) {
  //   ret = CreatOpen(path.c_str());
  // }
  if (!file_) {
    printf("FileWriter: fopen file \'%s\' failed!\n", path.c_str());
    return -1;
  }
  path_ = path;
  return ret;
}
// void FileWriter::Close(void *file) { fclose((FILE *)file); }

void FileWriter::Close(void) {
  if (file_) {
    fflush((FILE *)file_);
    // int fd = fileno((FILE *)file_);
    // fsync(fd);
    fclose((FILE *)file_);
  }
}

}  // namespace vzopera
