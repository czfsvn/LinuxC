#include <iostream>

#include "FileManager.h"

using namespace std;

int main()
{
  FileManager* file = new FileManager();
  file->init("/home/cn/work/cpp/LinuxC/linux-cpp/misc");
  return 0;
}