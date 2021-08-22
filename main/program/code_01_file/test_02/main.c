#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file-serializer.h"

static inline bool read_data(struct serializer *s, void *data, size_t size) {
  return s_read(s, data, size) == size;
}

static inline bool write_data(struct serializer *s, const void *data,
                              size_t size) {
  return s_write(s, data, size) == size;
}

#define read_var(s, data) read_data(s, &data, sizeof(data))
#define write_var(s, data) write_data(s, &data, sizeof(data))

static bool read_str(struct serializer *s, char **p_str) {
  uint32_t size;
  char *str;

  if (!read_var(s, size)) return false;

  // str = bmalloc(size + 1);
  str = calloc(1, size + 1);
  if (size && !read_data(s, str, size)) {
    free(str);
    return false;
  }

  str[size] = 0;
  *p_str = str;
  return true;
}

static bool write_str(struct serializer *s, const char *str) {
  uint32_t size = (uint32_t)(str ? strlen(str) : 0);

  if (!write_var(s, size)) return false;
  if (size && !write_data(s, str, size)) return false;
  return true;
}

typedef struct data_head_
{
  uint8_t  w_flag;
  uint32_t version;
  uint32_t w_index;
  uint32_t r_index;

  uint16_t size;

  uint32_t start;
  uint32_t end;
} data_head_t;

#include <fcntl.h>    // creat
// #include <sys/dir.h>
#include <sys/stat.h>  // stat
// #include <sys/types.h>
static int vpk_create_file(const char *filename)
{
	if (creat(filename, 0755) < 0) {

		printf("creat \'%s\' error\n", filename);
		return -1;
	}
	return 0;
}

int vpk_isfile(const char* file)
{
	struct stat st;
	memset(&st, 0x00, sizeof(struct stat));
	if (stat(file, &st) < 0) {
		printf("fstat error, directory \'%s\': not exists or other.\n", file);
		return 0;
	}
	//printf("%07o ", st.st_mode);
	return S_ISREG(st.st_mode);
}
int vpk_isdir(const char* path)
{
	struct stat st;
	memset(&st, 0x00, sizeof(struct stat));
	if (stat(path, &st) < 0) {
		printf("fstat error, directory \'%s\': not exists or other.\n", path);
		return 0;
	}
	//printf("%07o ", st.st_mode);
	return S_ISDIR(st.st_mode);
}

int save_file() {
  const char *file_name = "./test_data.txt";
  struct serializer s;
  bool ret = file_output_serializer_init(&s, file_name);
  // if (!ret) {
  //   printf("file_output_serializer_init failed: %s\n", file_name);
  //   return -1;
  // }
  if (!ret) {
    printf("file_output_serializer_init failed: %s\n", file_name);
    printf("create file..\n");
    vpk_create_file(file_name);
    if (vpk_isdir(file_name)) {
      printf("%s is dir\n", file_name);
    } else if (vpk_isfile(file_name)) {
      printf("%s is file\n", file_name);
    } else {
      printf("%s is other\n", file_name);
    }
    ret = file_output_serializer_init(&s, file_name);
  }
  if (!ret) {
    printf("file_output_serializer_init failed: %s\n", file_name);
    return -1;
  }

  int64_t pos = serializer_get_pos(&s);
  printf("current pos is %ld\n", pos);

  // uint32_t version = 1;
  // ret = write_var(&s, version);
  // return_val_if_fail(ret, -1);
  
  data_head_t head = {1, 12, 5000, 600, 1000,  sizeof(head), 4 << 20};

  printf("write head\n");
  printf("flag: %u, version: %u, w_index: %u, r_index: %u, size: %u, start: %u, end: %u\n",
          head.w_flag, head.version, head.w_index, head.r_index, head.size, head.start, head.end);

#if 0
  ret = write_var(&s, head.w_flag);
  return_val_if_fail(ret, -1);

  ret = write_var(&s, head.version);
  return_val_if_fail(ret, -1);

  ret = write_var(&s, head.w_index);
  return_val_if_fail(ret, -1);

  ret = write_var(&s, head.r_index);
  return_val_if_fail(ret, -1);

  ret = write_var(&s, head.size);
  return_val_if_fail(ret, -1);

  ret = write_var(&s, head.start);
  return_val_if_fail(ret, -1);

  ret = write_var(&s, head.end);
  return_val_if_fail(ret, -1);
#else
  ret = write_data(&s, &head, sizeof(head));
  return_val_if_fail(ret, -1);
#endif
  pos = serializer_get_pos(&s);
  printf("current pos is %ld\n", pos);

#if 1
  printf("close and re-open..\n");
  file_output_serializer_free(&s);
  ret = file_output_serializer_init(&s, file_name);
  if (!ret) {
    printf("file_output_serializer_init failed: %s\n", file_name);
    return -1;
  }
  int64_t offset = sizeof(head);
  serializer_seek(&s, offset, SERIALIZE_SEEK_START);
#endif
  pos = serializer_get_pos(&s);
  printf("current pos is %ld\n", pos);

  // write 1st string
  const char *str = "hello world, 1st it's test string!";
  printf("write string(%ld): %s\n", strlen(str), str);
  write_str(&s, str);
  pos = serializer_get_pos(&s);
  printf("current pos is %ld\n", pos);

  // write 2nd string
  offset = sizeof(head) + 1000;
  serializer_seek(&s, offset, SERIALIZE_SEEK_START);
  pos = serializer_get_pos(&s);
  printf("current pos is %ld\n", pos);

  const char *str2 = "hello world, 2nd it's test string!";
  printf("write string(%ld): %s\n", strlen(str2), str2);
  write_str(&s, str2);
  pos = serializer_get_pos(&s);
  printf("current pos is %ld\n", pos);

  file_output_serializer_free(&s);

  return 0;
}

int read_file() {
  const char *file_name = "./test_data.txt";
  struct serializer s;
  bool ret = file_input_serializer_init(&s, file_name);
  if (!ret) {
    printf("file_input_serializer_init failed: %s\n", file_name);
    return -1;
  }

  int64_t pos = serializer_get_pos(&s);
  printf("current pos is %ld\n", pos);

  data_head_t head = {0};
#if 0
  read_var(&s, head.w_flag);
  read_var(&s, head.version);
  read_var(&s, head.w_index);
  read_var(&s, head.r_index);
  read_var(&s, head.size);
  read_var(&s, head.start);
  read_var(&s, head.end);

  printf("####### read: \n");
  printf("flag: %u, version: %u, w_index: %u, r_index: %u, size: %u, start: %u, end: %u\n",
          head.w_flag, head.version, head.w_index, head.r_index, head.size, head.start, head.end);
  
  char *string = NULL;
  read_str(&s, &string);
  printf("read string: %s\n", string);
#else

  read_data(&s, &head, sizeof(head));

  printf("flag: %u, version: %u, w_index: %u, r_index: %u, size: %u, start: %u, end: %u\n",
          head.w_flag, head.version, head.w_index, head.r_index, head.size, head.start, head.end);
  pos = serializer_get_pos(&s);
  printf("current pos is %ld\n", pos);
#endif
  // int64_t offset = sizeof(head);
  // serializer_seek(&s, offset, SERIALIZE_SEEK_START);

  // 1st string
  char *string = NULL;
  read_str(&s, &string);
  printf("read string(%ld): %s\n", strlen(string), string);
  pos = serializer_get_pos(&s);
  printf("current pos is %ld\n", pos);

  // second string
  int64_t offset = sizeof(head) + 1000;
  serializer_seek(&s, offset, SERIALIZE_SEEK_START);
  pos = serializer_get_pos(&s);
  printf("current pos is %ld\n", pos);

  // char *string = NULL;
  read_str(&s, &string);
  printf("read 2nd string(%ld): %s\n", strlen(string), string);
  pos = serializer_get_pos(&s);
  printf("current pos is %ld\n", pos);

  return 0;
}

int main(int argc, char *argv[]) {
  printf("start..\n");

  save_file();
  
  printf("####### read: \n");

  read_file();
  return 0;
}
