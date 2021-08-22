#include <stdio.h>

#include <string>

static void func_a(std::string str) {
  static std::string a = str;
  static std::string b = a;
  printf("str addr: %p\n", &str);
  printf("a addr: %p\n", &a);
  printf("b addr: %p\n", &b);
}

static void func_b(std::string &str) {
  static std::string a = str;
  static std::string b = a;
  printf("str addr: %p\n", &str);
  printf("a addr: %p\n", &a);
  printf("b addr: %p\n", &b);
}

static void test_01() {
  int ret = 0;
  void *thread_result;
  pthread_t pth_test1, pth_test2;

  std::string data1 = "json string, 1st data!";
  std::string data2 = "json string, 2nd data!json string, 2nd data!json string, 2nd data!";
  printf("data1 addr: %p\n", &data1);
  printf("data2 addr: %p\n", &data2);
  func_a(data1);
  func_a(data2);

  func_b(data1);
  func_b(data2);

  return;
}

int main(int argc, char *argv[]) {
    printf("run test_01\n");
    test_01();
    return 0;
}
