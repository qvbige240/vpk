
#include "operation_file_impl.h"

namespace vzopera {

OperaFileProtocol::OperaFileProtocol(std::string filename) {
  filename_ = filename;
  file_reader_ = NULL;
  file_writer_ = NULL;
  fops_ = NULL;
}
OperaFileProtocol::~OperaFileProtocol() {
  vz_freep(file_reader_);
  vz_freep(file_writer_);
  vz_freep(fops_);
}

int32_t OperaFileProtocol::Init() {
  int ret = -1;
  // return_val_if_fail(filename_, -1);

  if (fops_ == NULL) fops_ = new FileSerializer();
  if (file_writer_ == NULL) file_writer_ = new FileWriter(fops_);
  if (file_reader_ == NULL) file_reader_ = new FileReader(fops_);

  // pthread_mutex_init(&file_mutex_, NULL);

  do {
    // lock
    if (!file_writer_->ExistFile(filename_.c_str())) {
      ret = file_writer_->CreatFile(filename_.c_str());
      if (ret < 0) {
        OPERA_LOGE("Init create file \'%s\' error!\n", filename_.c_str());
        break;
      }

      // proto_head_ = {0};
      memset(&proto_head_, 0x00, sizeof(proto_head_));
      ProtocolHead *head = &proto_head_.head;
      head->w_flag = 0;
      head->version = 1;
      head->cover = 0;
      head->num = FILE_DATA_MAX_NUM;
      head->size = FILE_DATA_MAX_LEN;
      head->total = sizeof(proto_head_) + head->num * head->size;
      head->r_index = 0;
      head->w_index = 0;
      head->carry = 0;
      head->r_pos = sizeof(proto_head_) + (head->r_index % head->num) * head->size;
      head->w_pos = sizeof(proto_head_) + (head->w_index % head->num) * head->size;
      head->start = sizeof(proto_head_);
      head->end = head->total;
      // proto_head_.timestamp

      ret = file_writer_->Open(filename_);
      if (ret < 0) {
        OPERA_LOGE("Init open file \'%s\' failed\n", filename_.c_str());
        break;
      }
      if (!file_writer_->WriteData(&proto_head_, sizeof(proto_head_))) {
        file_writer_->Close();
        OPERA_LOGE("Init write file \'%s\' head failed\n", filename_.c_str());
        break;
      }
      file_writer_->Close();
    }

    return 0;
  } while (0);

  vz_freep(file_reader_);
  vz_freep(file_writer_);
  vz_freep(fops_);
  return -1;
}

// todo
int32_t OperaFileProtocol::Write(void *ctx, void *data, size_t size) {
  return_val_if_fail(file_writer_ && data, -1);
  return 0;
}

int32_t OperaFileProtocol::WriteString(void *ctx, char *str) {
  return_val_if_fail(file_writer_ && str, -1);

  std::lock_guard<std::mutex> lock(mutex_);
  // pthread_mutex_lock(&file_mutex_);
  int ret = file_writer_->Open(filename_);
  if (ret < 0) {
    OPERA_LOGE("Writer open file \'%s\' failed\n", filename_.c_str());
    return -1;
  }

  do {
    /** seek to position **/
    memset(&proto_head_, 0x00, sizeof(proto_head_));
    if (!file_writer_->ReadData(&proto_head_.head, sizeof(proto_head_.head))) {
      OPERA_LOGE("Read \'%s\' head failed\n", filename_.c_str());
      ret = -1;
      break;
    }

    ProtocolHead *head = &proto_head_.head;
    if (head->r_index >= head->num && head->w_index >= head->num) {
      head->r_index = head->r_index % head->num;
      head->w_index = head->w_index % head->num;
      head->carry++;
    }
    int64_t pos = sizeof(proto_head_) + (head->w_index % head->num) * (int64_t)head->size;
    if (pos != head->w_pos) {
      OPERA_LOGE("###### Warning position (%ld != %ld) has some error: %s\n",
                 pos, (int64_t)head->w_pos, filename_.c_str());
    }

    OPERA_LOGI("write: index r: %u(%u), w: %u(%u)", head->r_index,
            head->r_index % head->num, head->w_index, head->w_index % head->num);
    if (file_writer_->Seek(pos, SERIALIZE_SEEK_START) < 0) {
      OPERA_LOGE("Seek \'%s\' failed!!\n", filename_.c_str());
      ret = -1;
      break;
    }
    if (!file_writer_->WriteString(str)) {
      OPERA_LOGE("Write file \'%s\' failed\n", filename_.c_str());
      ret = -1;
      break;
    }

    /**
     *  0                    num                  2*num
     *  |r____________________|_____________________|
     *  |w____________________|_____________________|
     *  |r____________________|w____________________|
     *  |_ _ _ _ _ _ _r_______|_____________w_ _ _ _|
     *
     *     0 <= r_index <= w_index
     *    |w_index - r_index| < num
     */
    head->w_index++;
    if (head->w_index >= (head->r_index + head->num)) {
      /** over written data **/
      head->cover++;
      head->r_index++;
    }
    head->w_pos = pos + head->size;
    if (head->w_pos >= head->end) {
      // head->w_pos = sizeof(proto_head_) + (head->w_index % head->num) * (int64_t)head->size;
      head->w_pos -= head->size * head->num;
    }
    
    /** seek to head **/
    if (file_writer_->Seek(0, SERIALIZE_SEEK_START) < 0) {
      OPERA_LOGE("Seek \'%s\' failed!!\n", filename_.c_str());
      ret = -1;
      break;
    }
    if (!file_writer_->WriteData(&proto_head_.head, sizeof(proto_head_.head))) {
      OPERA_LOGE("Write head \'%s\' failed\n", filename_.c_str());
      ret = -1;
      break;
    }
    OPERA_LOGD("WriteString: %s\n", str);

    // TODO write timestamp
  } while (0);

  file_writer_->Close();
  // pthread_mutex_unlock(&file_mutex_);

  return ret;
}

// todo
int32_t OperaFileProtocol::Read(void *ctx, void *data, size_t size) { return 0;}

int32_t OperaFileProtocol::ReadString(void *ctx, char *str, size_t size) {
  return_val_if_fail(file_reader_ && str, -1);
  
  if (size <= 0) {
    OPERA_LOGW("### ReadString read size invalid!\n");
    return 0;
  }

  std::lock_guard<std::mutex> lock(mutex_);
  // pthread_mutex_lock(&file_mutex_);

  int ret = file_reader_->Open(filename_);
  if (ret < 0) {
    OPERA_LOGE("Reader open file \'%s\' failed\n", filename_.c_str());
    return -1;
  }

  do {
    /** seek to position **/
    memset(&proto_head_, 0x00, sizeof(proto_head_));
    if (!file_reader_->ReadData(&proto_head_.head, sizeof(proto_head_.head))) {
      OPERA_LOGE("Read \'%s\' head failed\n", filename_.c_str());
      ret = -1;
      break;
    }

    ProtocolHead *head = &proto_head_.head;
    int64_t pos = sizeof(proto_head_) + (head->r_index % head->num) * (int64_t)head->size;
    if (pos != head->r_pos) {
      OPERA_LOGE("Position (%ld != %ld) has some error: %s\n", pos, (int64_t)head->r_pos, filename_.c_str());
    }

    OPERA_LOGI("read: index r: %u(%u), w: %u(%u)\n", head->r_index,
               head->r_index % head->num, head->w_index, head->w_index % head->num);
    if (head->r_index >= head->w_index) {
      OPERA_LOGW("There is no data which has not been read\n");
      size = 0;
      ret = 0;
      break;
    }
    if (file_reader_->Seek(pos, SERIALIZE_SEEK_START) < 0) {
      OPERA_LOGE("Seek \'%s\' failed!!\n", filename_.c_str());
      ret = -1;
      break;
    }
    if (!file_reader_->ReadString(str, size)) {
      OPERA_LOGE("Read file \'%s\' failed\n", filename_.c_str());
      ret = -1;
    }

  } while (0);

  file_reader_->Close();
  // pthread_mutex_unlock(&file_mutex_);

  return (ret == 0 ? (int32_t)size : ret);
}

int32_t OperaFileProtocol::ReadArrayString(void *ctx, int n, std::vector<std::string> &result) {
  return_val_if_fail(file_reader_ && n > 0, -1);

  if (n <= 0) {
    OPERA_LOGW("Read n(%d) string invalid!\n", n);
    return 0;
  }

  std::lock_guard<std::mutex> lock(mutex_);
  // pthread_mutex_lock(&file_mutex_);

  int ret = file_reader_->Open(filename_);
  if (ret < 0) {
    OPERA_LOGE("Reader open file \'%s\' failed\n", filename_.c_str());
    return -1;
  }

  int count = 0;
  do {
    memset(&proto_head_, 0x00, sizeof(proto_head_));
    if (!file_reader_->ReadData(&proto_head_.head, sizeof(proto_head_.head))) {
      OPERA_LOGE("Read \'%s\' head failed\n", filename_.c_str());
      ret = -1;
      break;
    }

    ProtocolHead *head = &proto_head_.head;
    int64_t pos = sizeof(proto_head_) + (head->r_index % head->num) * (int64_t)head->size;
    if (pos != head->r_pos) {
      OPERA_LOGE("Position (%ld != %ld) has some error: %s\n", pos, (int64_t)head->r_pos, filename_.c_str());
    }

    OPERA_LOGI("read: index r: %u(%u), w: %u(%u)\n", head->r_index,
               head->r_index % head->num, head->w_index, head->w_index % head->num);
    if (head->r_index >= head->w_index) {
      n = 0;
      ret = 0;
      OPERA_LOGW("There is no data that has not been read\n");
      break;
    }

    count = head->w_index - head->r_index;
    count = n > count ? count : n;

    char tmp[1024] = {0};
    for (int i = 0; i < count; i++) {
      pos = sizeof(proto_head_) + (head->r_index % head->num) * (int64_t)head->size;
      if (file_reader_->Seek(pos, SERIALIZE_SEEK_START) < 0) {
        ret = -1;
        OPERA_LOGE("Seek file \'%s\' failed!\n", filename_.c_str());
        break;
      }

      memset(tmp, 0x00, sizeof(tmp));
      size_t slen = sizeof(tmp);
      if (!file_reader_->ReadString(tmp, slen)) {
        ret = -1;
        OPERA_LOGE("Read file \'%s\' failed\n", filename_.c_str());
        break;
      } else {
        std::string data = tmp;
        result.push_back(data);
      }

      head->r_index++;
    }

    if (result.size() > 0) {
      ret = 0;
      int size = (int)result.size();
      if (size != count) {
        count = size;
        OPERA_LOGW("result size %d, expect: %d", size, count);
      }
    }
  } while (0);

  file_reader_->Close();
  // pthread_mutex_unlock(&file_mutex_);

  return (ret == 0 ? (int32_t)count : ret);
}

int32_t OperaFileProtocol::UpdateReadIndex(void *ctx, uint32_t count) {
  return_val_if_fail(file_writer_, -1);

  if (count <= 0 || count > FILE_DATA_MAX_NUM) {
    OPERA_LOGE("count(%u) is invalid\n", count);
    return 0;
  }

  std::lock_guard<std::mutex> lock(mutex_);
  // pthread_mutex_lock(&file_mutex_);

  int ret = file_writer_->Open(filename_);
  if (ret < 0) {
    OPERA_LOGE("Open file \'%s\' failed\n", filename_.c_str());
    return -1;
  }

  do {
    /** seek to position **/
    memset(&proto_head_.head, 0x00, sizeof(proto_head_.head));
    if (!file_writer_->ReadData(&proto_head_.head, sizeof(proto_head_.head))) {
      OPERA_LOGE("UpdateReadIndex \'%s\' read head failed\n", filename_.c_str());
      ret = -1;
      break;
    }

    ProtocolHead *head = &proto_head_.head;
    int64_t pos = sizeof(proto_head_) + (head->r_index % head->num) * (int64_t)head->size;
    if (pos != head->r_pos) {
      OPERA_LOGE("###### Warning position (%ld != %ld) has some error: %s\n",
                 pos, (int64_t)head->r_pos, filename_.c_str());
    }

    head->r_index += count;
    if (head->r_index > head->w_index) {
      /** read index must be less than or equal to write index **/
      count -= (head->r_index - head->w_index);
      head->r_index = head->w_index;
    }
    head->r_pos = pos + (uint64_t)head->size * count;
    if (head->r_pos >= head->end) {
      // head->r_pos = sizeof(proto_head_) + (head->r_index % head->num) *
      // (int64_t)head->size;
      head->r_pos -= head->size * head->num;
    }
    if (head->r_index >= head->num && head->w_index >= head->num) {
      head->r_index = head->r_index % head->num;
      head->w_index = head->w_index % head->num;
      head->carry++;
    }

    /** seek to head **/
    int64_t val = -1;
    if ((val = file_writer_->Seek(0, SERIALIZE_SEEK_START)) < 0) {
      OPERA_LOGE("Seek \'%s\' failed!!\n", filename_.c_str());
      ret = -1;
      break;
    }

    if (!file_writer_->WriteData(&proto_head_.head, sizeof(proto_head_.head))) {
      OPERA_LOGE("Write head \'%s\' failed\n", filename_.c_str());
      ret = -1;
      break;
    }
  } while (0);

  file_writer_->Close();
  // pthread_mutex_unlock(&file_mutex_);

  return ret;
}

// test local
#if 0
int32_t OperaFileProtocol::Write(void *ctx, void *data, size_t size) {
  FileHeadData head = {0};
  head.version = 1;
  head.num = 4000;
  head.size = 1000;
  head.total = head.num * head.size + sizeof(head);
  head.start = sizeof(head);
  head.end = head.total;
  printf("write head:\n");
  printf("version: %u, num: %u, size: %u, total: %lu, start: %u, end: %lu\n", 
          head.version, head.num, head.size, head.total, head.start, head.end);

  std::string path = "./test_data.txt";
  FileSerializer *fs = new FileSerializer();
  FileWriter *writer = new FileWriter(fs);
  if (writer->Open(path) < 0) {
    printf("open file \'%s\' failed!\n", path.c_str());
    return -1;
  }
  bool ret = writer->WriteData(&head, sizeof(head));
  return_val_if_fail(ret, -1);
  int64_t pos = writer->GetPosition();
  printf("current pos: %ld\n", pos);

  const char *str1 = "hello world, 1st it's test string";
  const char *str2 = "hello world, 2nd it's test string";
  printf("write string: %s\n", str1);
  ret = writer->WriteString((char *)str1);
  return_val_if_fail(ret, -1);
  pos = writer->GetPosition();
  printf("current pos: %ld\n", pos);
  
  printf("write string: %s\n", str2);
  ret = writer->WriteString((char *)str2);
  return_val_if_fail(ret, -1);
  pos = writer->GetPosition();
  printf("current pos: %ld\n", pos);

  writer->Close();

  delete writer;
  delete fs;
}

int32_t OperaFileProtocol::Read(void *ctx, void *data, size_t size) {
  FileHeadData head = {0};
  printf("read head:\n");
  
  std::string path = "./test_data.txt";
  FileSerializer *fs = new FileSerializer();
  FileReader *reader = new FileReader(fs);
  if (reader->Open(path) < 0) {
    printf("open file \'%s\' failed!\n", path.c_str());
    return -1;
  }

  int64_t pos = reader->GetPosition();
  printf("current pos: %ld\n", pos);
  bool ret = reader->ReadData(&head, sizeof(head));
  return_val_if_fail(ret, -1);
  printf("version: %u, num: %u, size: %u, total: %lu, start: %u, end: %lu\n",
          head.version, head.num, head.size, head.total, head.start, head.end);
  pos = reader->GetPosition();
  printf("current pos: %ld\n", pos);

  char str[256] = {0};
  ret = reader->ReadString(str, sizeof(str));
  return_val_if_fail(ret, -1);
  printf("read string: %s\n", str);
  pos = reader->GetPosition();
  printf("current pos: %ld\n", pos);

  memset(str, 0x00, sizeof(str));
  ret = reader->ReadString(str, sizeof(str));
  return_val_if_fail(ret, -1);
  printf("read string: %s\n", str);
  pos = reader->GetPosition();
  printf("current pos: %ld\n", pos);

  reader->Close();

  delete reader;
  delete fs;
  return 0;
}
#endif

}  // namespace vzopera
