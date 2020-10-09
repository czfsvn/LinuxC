#include "FileManager.h"
#include <boost/algorithm/hex.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include <iomanip>
#include <iostream>
#include <openssl/md5.h>
#include <sstream>

#include <sys/epoll.h>

using namespace std;

void FileManager::init(const boost::filesystem::path &parent)
{
  parent_path = parent;
}

void FileManager::run()
{
  recursive_dirs(parent_path);
  epoll_watch();
}

void FileManager::recursive_dirs(const path &dir)
{
  directory_iterator end;
  for (directory_iterator pos(dir); pos != end; ++pos)
  {
    path tmp(*pos);
    if (is_directory(tmp) /* && !isIgnoreDir(tmp)*/)
    {
      if (!isIgnoreDir(tmp))
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

bool FileManager::isIgnoreDir(const path &dir)
{
  std::set<path> ignor_dir = {
      path("build"),
  };

  for (const auto &item : ignor_dir)
  {
    if (item == dir.filename())
      return true;
  }
  return false;
}

bool FileManager::isWatchingFile(const path &dir)
{
  std::set<path> watch_extensions = {
      path(".h"),
      path(".cpp"),
      path(".xml"),
      path(".proto"),
      path(".txt"),
  };

  for (const auto &item : watch_extensions)
  {
    if (item == dir.extension())
      return true;
  }
  return false;
}

void FileManager::addWatchMap(const path &dir)
{
  if (!isWatchingFile(dir))
    return;

  file_md5_map.emplace(dir.string(), md5_from_file(dir.string()));
  path_set.emplace(dir.parent_path());
}

std::string FileManager::md5_from_file(const std::string &path) const
{
  //try
  {
    if (path.empty())
    {
      /* code */
    }

    std::cout << "file: " << path << std::endl;
    unsigned char result[MD5_DIGEST_LENGTH] = {};
    boost::iostreams::mapped_file_source src(path);
    MD5((unsigned char *)src.data(), src.size(), result);

    std::ostringstream sout;
    sout << std::hex << std::setfill('0');
    for (auto c : result)
      sout << std::setw(2) << (int)c;

    return sout.str();
  }
  //catch (const std::exception &e)
  {
    //std::cerr << e.what() << '\n';
  }
}

void FileManager::epoll_watch()
{
  if (path_set.empty())
    return;

  for (const auto &item : path_set)
  {
    std::cout << "epoll_watch: " << item.string() << std::endl;
  }

#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN (1024 * (EVENT_SIZE + 16))

  struct epoll_event ev, events[20];
  int32_t fd = inotify_init();

  const uint32_t mask = IN_ALL_EVENTS;
  std::map<uint32_t, path> wd_map = {};
  for (const path &item : path_set)
  {
    const uint32_t wd = inotify_add_watch(fd, item.string().c_str(), mask);
    wd_map[wd] = item;
  }

  ev.data.fd = fd;
  ev.events = EPOLLIN | EPOLLET;

  int32_t epfd = epoll_create(256);
  epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);

  while (1)
  {
    int32_t nfds = epoll_wait(epfd, events, 20, 1000);

    std::map<uint32_t, uint32_t> tmp_set = {};
    for (int32_t i = 0; i < nfds; i++)
    {
      if (events[i].data.fd != fd)
        continue;

      char buf[BUF_LEN] = {};
      int32_t length = read(fd, buf, BUF_LEN);
      if (length < 0)
        return;

      while (i < length)
      {
        struct inotify_event *event = (struct inotify_event *)&buf[i];
        if (event->len && event->mask & IN_MODIFY)
        {
          if (!tmp_set.count(event->wd))
          {
            tmp_set.emplace(event->wd, 1);
            try
            {
              checkAndTouch(wd_map, event);
            }
            catch (const std::exception &e)
            {
              std::cerr << e.what() << '\n';
            }
          }
          else
          {
            tmp_set[event->wd] += 1;
          }
        }

        i += (EVENT_SIZE + event->len);
      }
    }
  }
}

void FileManager::checkAndTouch(const std::map<uint32_t, path> &wd_map, const inotify_event *event)
{
  if (!event)
    return;

  auto iter = wd_map.find(event->wd);
  if (iter == wd_map.end())
    return;

  std::stringstream oss;
  oss << iter->second.string() << "/" << event->name;

  path tmp_path(oss.str());

  std::string old_md5 = "";
  auto it = file_md5_map.find(oss.str());
  if (it != file_md5_map.end())
    old_md5 = it->second;

  const std::string &new_md5 = md5_from_file(tmp_path.string());
  if (new_md5 == old_md5)
    return;

  std::stringstream touch;
  touch << "touch " << oss.str();

  system(touch.str().c_str());
  file_md5_map[oss.str()] = new_md5;
}