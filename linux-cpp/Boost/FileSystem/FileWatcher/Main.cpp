#include <iostream>
#include <sstream>

#include "FileManager.h"

using namespace std;

void test_md5()
{
  std::string file = "/home/cn/work/cpp/LinuxC/linux-cpp/misc/daemon.cpp";

  std::stringstream oss;
  oss << "md5sum " << file;
  char data[33] = {};
  FILE *fp = popen(oss.str().c_str(), "r");
  if (!fp)
    return;

  while (fgets(data, sizeof(data), fp) != nullptr)
  {
    std::cout << "data: " << data << std::endl;
    break;
  }

  pclose(fp);
}

int main()
{
  test_md5();
  /*
  FileManager *file = new FileManager();
  file->init("/home/cn/work/cpp/LinuxC/linux-cpp/misc");
  file->run();
  */
  return 0;
}