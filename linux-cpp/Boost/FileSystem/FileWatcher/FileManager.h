#ifndef __FILE_MANAGER_H__
#define __FILE_MANAGER_H__

#include <map>
#include <set>

#include <boost/filesystem.hpp>
#include <sys/inotify.h>

using namespace std;
using namespace boost::filesystem;

class FileManager
{
public:
  void init(const boost::filesystem::path &parent);
  void run();

private:
  bool isWatchingFile(const path &dir);
  bool isIgnoreDir(const path &dir);
  std::string md5_from_file(const std::string &path) const;

  void recursive_dirs(const path &dir);
  void onFileEvent();
  void epoll_watch();
  void addWatchMap(const path &dir);
  void checkAndTouch(const std::map<uint32_t, path> &wd_map, const inotify_event *event);

  void CheckMtr(const int *pointy);

private:
  boost::filesystem::path parent_path;
  std::set<boost::filesystem::path> path_set = {};
  std::map<std::string, std::string> file_md5_map = {};
};

class File
{
public:
private:
};

#endif