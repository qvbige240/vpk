#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <pthread.h>
#include <iostream>

using namespace std;

#if 1 /* test_01 */
#include "operation_file_impl.h"
void test_01() {
  vzopera::OperaFileProtocol *log = new vzopera::OperaFileProtocol("./log.txt");
  printf("start write file...\n");
  log->Write(NULL, NULL, 0);
  printf("start read file...\n");
  log->Read(NULL, NULL, 0);
  delete log;
}
#endif

#if 0 /* test_02 */
#include "operation_file.h"

void test_02() {
  vzopera::OperaFileManager *manager = vzopera::OperaFileManager::GetInstance();
  // manager->OpenFile(vzopera::OPERA_FILE_PROTO_LOG);
  // manager->Init();

  std::string data1 = "json string, 1st data!";
  std::string data2 = "json string, 2nd data!";
  manager->Write(vzopera::OPERA_FILE_PROTO_LOG, data1);
  manager->Write(vzopera::OPERA_FILE_PROTO_LOG, data2);

  char buffer[1024] = {0};
  manager->Read(vzopera::OPERA_FILE_PROTO_LOG, buffer, sizeof(buffer));
  printf("read string: %s\n", buffer);
  manager->UpdateReadIndex(vzopera::OPERA_FILE_PROTO_LOG, 1);

  memset(buffer, 0x00, sizeof(buffer));
  manager->Read(vzopera::OPERA_FILE_PROTO_LOG, buffer, sizeof(buffer));
  printf("read string: %s\n", buffer);
  manager->UpdateReadIndex(vzopera::OPERA_FILE_PROTO_LOG, 1);
  // close release
}
#endif

#if 1/* test 03 */
#include "operation_file.h"
#define TEST_NUM 500
void *write_file_thread(void *arg) {
  vzopera::OperaFileManager *manager = vzopera::OperaFileManager::GetInstance();
  manager->Init();
  // std::string data1 = "json string, 1st data!";
  // std::string data2 = "json string, 2nd data!";
  int index = 0;
  int32_t ret = 0;
  char buffer[256] = {0};
  while (index < TEST_NUM) {
    snprintf(buffer, sizeof(buffer) - 1, "json string, %d data!", index++);
    std::string data = buffer;
    ret = manager->Write(vzopera::OPERA_FILE_PROTO_LOG, data);
    if (ret < 0) {
      printf("============== Write failed\n\n");
    }
    usleep(10000);
  }
  return NULL;
}
void *read_file_thread(void *arg) {
  vzopera::OperaFileManager *manager = vzopera::OperaFileManager::GetInstance();
  manager->Init();
  usleep(10000);

  int32_t ret = 0;
  int loop = TEST_NUM;
  char buffer[1024] = {0};
  while (loop > 0) {
    memset(buffer, 0x00, sizeof(buffer));
    ret = manager->Read(vzopera::OPERA_FILE_PROTO_LOG, buffer, sizeof(buffer));
    if (ret < 0) {
      printf("============== Read failed\n\n");
      sleep(1);
      continue;
    }
    loop--;
    printf("read string: %s\n", buffer);
    ret = manager->UpdateReadIndex(vzopera::OPERA_FILE_PROTO_LOG, 1);
    if (ret < 0) {
      printf("============== UpdateReadIndex failed\n\n");
      sleep(2);
    }
    usleep(10000);
  }

  return NULL;
}
void test_03() {
  int ret = 0;
  void *thread_result;
  pthread_t pth_test1, pth_test2;

  printf("run test_03()\n");
  // vzopera::OperaFileManager::GetInstance()->Init();

  ret = pthread_create(&pth_test1, NULL, write_file_thread, (void *)NULL);
  if (ret != 0) printf("create thread \'write_file_thread\' failed\n");

  ret = pthread_create(&pth_test2, NULL, read_file_thread, (void *)NULL);
  if (ret != 0) printf("create thread \'read_file_thread\' failed\n");

  pthread_join(pth_test2, &thread_result);
  pthread_join(pth_test1, &thread_result);
}
#endif

/* test 04 */
#if 0
#include <semaphore.h>
#define WRITE_MAX_NUM 1000
static sem_t queue_sem;
static pthread_mutex_t queue_mutex_;
static int write_flag = 0;
static int task_end = 0;
static void write_mutex_init() {
  sem_init(&queue_sem, 0, 0);
  pthread_mutex_init(&queue_mutex_, NULL);
}
static void write_send() {
  pthread_mutex_lock(&queue_mutex_);
  write_flag += 1;
  pthread_mutex_unlock(&queue_mutex_);
  sem_wait(&queue_sem);
}
static int write_get() {
  int flag = 0;
  pthread_mutex_lock(&queue_mutex_);
  if (write_flag > 0) {
    write_flag -= 1;
    flag = 1;
    // sem_post(&queue_sem);
  }
  pthread_mutex_unlock(&queue_mutex_);
  return flag;
}
void *write_file_queue(void *arg) {
  vzopera::OperaFileManager *manager = vzopera::OperaFileManager::GetInstance();
  // manager->OpenFile(vzopera::OPERA_FILE_PROTO_LOG);
  // manager->Init();
  // std::string data1 = "json string, 1st data!";
  // std::string data2 = "json string, 2nd data!";
  int index = 0;
  int32_t ret = 0;
  char buffer[256] = {0};

  while (index < WRITE_MAX_NUM || !task_end) {
    if (index < WRITE_MAX_NUM) {
      snprintf(buffer, sizeof(buffer) - 1, "json string, %d data!", index++);
      std::string data = buffer;
      ret = manager->Write(vzopera::OPERA_FILE_PROTO_LOG, data);
      if (ret < 0) {
        printf("============== Write failed\n\n");
      }
    }

    if (write_get()) {
      ret = manager->UpdateReadIndex(vzopera::OPERA_FILE_PROTO_LOG, 1);
      if (ret < 0) {
        printf("============== UpdateReadIndex failed\n\n");
        sleep(2);
      }
      sem_post(&queue_sem);
    }
    usleep(10000);
  }
  if (task_end) {
    printf("write thread end, index = %d\n\n", index);
  }
  // close
  return NULL;
}
void *read_file_queue(void *arg) {
  vzopera::OperaFileManager *manager = vzopera::OperaFileManager::GetInstance();
  // manager->OpenFile(vzopera::OPERA_FILE_PROTO_LOG);
  // manager->Init();

  int32_t ret = 0;
  int num = WRITE_MAX_NUM;
  char buffer[1024] = {0};
  while (num > 0) {
    memset(buffer, 0x00, sizeof(buffer));
    ret = manager->Read(vzopera::OPERA_FILE_PROTO_LOG, buffer, sizeof(buffer));
    if (ret < 0) {
      printf("============== Read failed\n\n");
      sleep(1);
      continue;
    }
    num--;
    printf("read string: %s\n", buffer);
    // ret = manager->UpdateReadIndex(vzopera::OPERA_FILE_PROTO_LOG, 1);
    // if (ret < 0) {
    //   printf("============== UpdateReadIndex failed\n\n");
    //   sleep(2);
    // }
    write_send();

    usleep(10000);
  }
  printf("read thread end, num = %d\n\n", num);

  task_end = 1;

  // close
  return NULL;
}
void test_04() {
  int ret = 0;
  void *thread_result;
  pthread_t pth_test1, pth_test2;
  printf("run test_03()\n");

  vzopera::OperaFileManager *manager = vzopera::OperaFileManager::GetInstance();
  write_mutex_init();

  ret = pthread_create(&pth_test1, NULL, write_file_queue, (void *)NULL);
  if (ret != 0) printf("create thread \'write_file_queue\' failed\n");

  ret = pthread_create(&pth_test2, NULL, read_file_queue, (void *)NULL);
  if (ret != 0) printf("create thread \'read_file_queue\' failed\n");

  pthread_join(pth_test2, &thread_result);
  pthread_join(pth_test1, &thread_result);
}
#endif // test 04

/* test 05 */
#if 1
#include "operation_file.h"
struct thread_data_ {
  int id;
  pthread_t pth_id;
};
static void *singleton_thread(void *arg) {
  struct thread_data_ *data = (struct thread_data_ *)arg;
  vzopera::OperaFileManager *manager = vzopera::OperaFileManager::GetInstance();
  printf("############### [%d]OperaFileManager %p\n", data->id, manager);
  manager->Init();
  sleep(1);
  return NULL;
}

void test_05() {
  int ret = 0;
  void *thread_result;
  pthread_t pth_test1, pth_test2;

  int i = 5;
  printf("run test_05()\n");
  OPERA_LOGE(" hello world i = %d", i);
#define THREAD_NUM 10
  struct thread_data_ data[THREAD_NUM] = {0};
  for (int i = 0; i < THREAD_NUM; i++) {
    data[i].id = i;
    ret = pthread_create(&data[i].pth_id, NULL, singleton_thread, (void *)&data[i]);
    if (ret != 0) printf("create thread \'singleton_thread %d\' failed\n", i);
  }

  for (int i = 0; i < THREAD_NUM; i++) {
    pthread_join(data[i].pth_id, &thread_result);
  }
}
#endif


#if 1/* test 06 */
#include "operation_file.h"
#define TEST_06_NUM 50
static void *write_file_06_thread(void *arg) {
  vzopera::OperaFileManager *manager = vzopera::OperaFileManager::GetInstance();
  manager->Init();
  // std::string data1 = "json string, 1st data!";
  // std::string data2 = "json string, 2nd data!";
  int index = 0;
  int32_t ret = 0;
  char buffer[256] = {0};
  while (index < TEST_06_NUM) {
    snprintf(buffer, sizeof(buffer) - 1, "json string, %d data!", index++);
    std::string data = buffer;
    ret = manager->Write(vzopera::OPERA_FILE_PROTO_LOG, data);
    if (ret < 0) {
      printf("============== Write failed\n\n");
    }
    usleep(10000);
  }
  return NULL;
}
static void *read_file_06_thread(void *arg) {
  vzopera::OperaFileManager *manager = vzopera::OperaFileManager::GetInstance();
  manager->Init();
  usleep(50000);

  int32_t ret = 0;
  int loop = TEST_06_NUM;
  std::vector<std::string> result;
  while (loop > 0) {
    result.clear();
    cout << "vector size: " << result.size() << " capacity: " << result.capacity() << endl;
    ret = manager->ReadArrayString(vzopera::OPERA_FILE_PROTO_LOG, 10, result);
    if (ret < 0) {
      OPERA_LOGE("============== Read failed\n");
      sleep(1);
      continue;
    } else if (ret > 0) {
      loop -= ret;

      uint32_t size = result.size();
      if (ret != size) {
        OPERA_LOGE("====== ret(%d) != result.size() %u\n", ret, size);
      }

      cout << "vector size: " << size << " capacity: " << result.capacity() << endl;
      for (std::string &str : result) {
        cout << "read string: " << str << endl;
      }

      ret = manager->UpdateReadIndex(vzopera::OPERA_FILE_PROTO_LOG, size);
      if (ret < 0) {
        OPERA_LOGE("============== UpdateReadIndex failed\n");
        sleep(2);
      }
    } else {
      loop--;
      usleep(100000);
    }

    usleep(100000);
  }

  return NULL;
}
void test_06() {
  int ret = 0;
  void *thread_result;
  pthread_t pth_test1, pth_test2;

  printf("run test_06()\n");
  // vzopera::OperaFileManager::GetInstance()->Init();

  // ret = pthread_create(&pth_test1, NULL, write_file_06_thread, (void *)NULL);
  // if (ret != 0) printf("create thread \'write_file_06_thread\' failed\n");

  ret = pthread_create(&pth_test2, NULL, read_file_06_thread, (void *)NULL);
  if (ret != 0) printf("create thread \'read_file_06_thread\' failed\n");

  pthread_join(pth_test2, &thread_result);
  // pthread_join(pth_test1, &thread_result);
}
#endif

/**
 * g++ -g -O0 -o test *.cpp -lpthread
 * ./test -t write
 * ./test -t read
 */
int main(int argc, char *argv[]) {
  // test_01();
  // test_02();
  // test_03();
  // test_04();
  // test_05();
  test_06();

  // char *type = "write";
  // int o;
  // static const struct option long_options[] = {
  //     {"help", no_argument, NULL, 'h'},
  //     {"version", no_argument, NULL, 'V'},
  //     {"sample", required_argument, NULL, 'd'},
  //     {"type", required_argument, NULL, 't'},
  //     {"remoteip", required_argument, NULL, 'i'},
  //     {"remoteport", required_argument, NULL, 'p'},
  //     {"localport", required_argument, NULL, 'l'},
  //     {NULL, 0, NULL, 0}};

  // optind = 1;
  // // LOG_I("22 optind = %d, argc = %d", optind, argc);
  // while ((o = getopt_long(argc, argv, "hVd:t:i:p:t:l:", long_options, NULL)) >= 0) {
  //   // printf("opt = %c\n", o);
  //   // printf("optarg = %s\n", optarg);
  //   // printf("optind = %d\n", optind);
  //   // printf("argv[optind - 1] = %s\n",  argv[optind - 1]);
  //   switch (o) {
  //     case 't':
  //       type = optarg;
  //       break;
  //     default:
  //       break;
  //   }
  // }
  // printf("type = %s\n", type);
  // don't support multi-process
  // if (strcasecmp(type, "write") == 0) {
  //   write_file();
  // } else if (strcasecmp(type, "read") == 0) {
  //   read_file();
  // } else {
  //   read_file();
  // }
  return 0;
}
