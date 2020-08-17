#include "FileManager.h"

#include <iomanip>
#include <iostream>
#include <sstream>

#include <boost/iostreams/device/mapped_file.hpp>
#include <openssl/md5.h>

using namespace std;

void FileManager::init(const boost::filesystem::path& parent)
{
  parent_path = parent;
}

void FileManager::run()
{
  recursive_dirs(parent_path);
  epoll_watch();
}

void FileManager::recursive_dirs(const path& dir)
{
  directory_iterator end;
  for(directory_iterator pos(dir); pos != end; ++pos)
  {
    path tmp(*pos);
    if(is_directory(tmp) /* && !isIgnoreDir(tmp)*/)
    {
      if(!isIgnoreDir(tmp))
        recursive_dirs(tmp);
    }
    else
    {
      addWatchMap(tmp);
      // if(!isWatchingFile(tmp))
      //  continue;

      std::cout << "[files: ]" << tmp << std::endl;
    }
  }
}

bool FileManager::isIgnoreDir(const path& dir)
{
  std::set<path> ignor_dir = {
      path("build"),
  };

  for(const auto& item : ignor_dir)
  {
    if(item == dir.filename())
      return true;
  }
  return false;
}

bool FileManager::isWatchingFile(const path& dir)
{
  std::set<path> watch_extensions = {
      path(".h"),
      path(".cpp"),
      path(".xml"),
      path(".proto"),
      path(".txt"),
  };

  for(const auto& item : watch_extensions)
  {
    if(item == dir.extension())
      return true;
  }
  return false;
}

void FileManager::addWatchMap(const path& dir)
{
  if(!isWatchingFile(dir))
    return;

  file_md5_map.emplace(dir, md5_from_file(dir));
  path_set.emplace(dir.parent_path());
}

std::string FileManager::md5_from_file(const path& path) const
{
  unsigned char                        result[MD5_DIGEST_LENGTH];
  boost::iostreams::mapped_file_source src(path.string());
  MD5((unsigned char*)src.data(), src.size(), result);

  std::ostringstream sout;
  sout << std::hex << std::setfill('0');
  for(auto c : result)
    sout << std::setw(2) << (int)c;

  return sout.str();
}

void FileManager::epoll_watch()
{
  if(path_set.empty())
    return;

  for()
}
