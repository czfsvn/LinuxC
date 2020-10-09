//#define BOOST_SYSTEM_NO_LIB

#include <iostream>
#include <set>

#include <boost/algorithm/hex.hpp>
#include <boost/filesystem.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/uuid/detail/md5.hpp>
#include <boost/uuid/detail/sha1.hpp>
#include <openssl/md5.h>

using namespace std;
using namespace boost::filesystem;

namespace ns_test1
{
  void logging(const char* msg) { cout << msg << endl; }

  void test1()
  {
    std::string filename = "log.txt";
    logging("log start");
    cout << "log start" << endl;

    if(!filename.empty())
    {
      std::ofstream fs(filename.c_str());
      cout.rdbuf(fs.rdbuf());
      logging("fatal msg");
    }
    logging("log end");
    cout << "log end" << endl;
  }

  void main() { test1(); }
} // namespace ns_test1

namespace ns_path
{
  void test1()
  {
    path p("/home/cn/work/cpp/LinuxC/linux-cpp/Boost/FileSystem/Main.cpp");

    std::cout << "p.string(): " << p.string() << std::endl;
    // std::cout << "p.directory_string()" << p.directory_string() << std::endl;
    std::cout << "p.parent_path(): " << p.parent_path() << std::endl;
    std::cout << "p.stem(): " << p.stem() << std::endl;
    std::cout << "p.filename(): " << p.filename() << std::endl;
    std::cout << "p.extension(): " << p.extension() << std::endl;

    assert(p.is_complete());
    assert(system_complete(p).is_complete());

    std::cout << "p.root_name(): " << p.root_name() << std::endl;
    std::cout << "p.root_path(): " << p.root_path() << std::endl;
    std::cout << "p.root_directory(): " << p.root_directory() << std::endl;

    std::cout << "p.relative_path(): " << p.relative_path() << std::endl;

    assert(!p.has_root_name());
    assert(p.has_root_path());
    assert(p.has_parent_path());

    std::cout << "p.replace_extension(): " << p.replace_extension()
              << std::endl;
    std::cout << "p.replace_extension(\"hxx\"): " << p.replace_extension("hxx")
              << std::endl;
    std::cout << "p.remove_filename(): " << p.remove_filename() << std::endl;
  }

  void print_type(const std::string& path)
  {
    std::cout << "type: " << status(path).type() << " ---- " << path
              << std::endl;
  }

  void test_file_status()
  {
    print_type("/home/cn/work/cpp/LinuxC/linux-cpp/Boost/FileSystem/Main.cpp");
    print_type("/home/cn/work/cpp/LinuxC/linux-cpp/Boost/FileSystem/");
    print_type("/home/cn/work/cpp/LinuxC/linux-cpp/Boost");
  }

  void is_file_status()
  {
    std::cout << "is_directory: "
              << is_directory(
                     "/home/cn/work/cpp/LinuxC/linux-cpp/Boost/FileSystem/")
              << std::endl;
    std::cout << "is_regular: "
              << is_regular(
                     "/home/cn/work/cpp/LinuxC/linux-cpp/Boost/FileSystem/")
              << std::endl;

    std::cout
        << "is_directory: "
        << is_directory(
               "/home/cn/work/cpp/LinuxC/linux-cpp/Boost/FileSystem/Main.cpp")
        << std::endl;
    std::cout
        << "is_regular: "
        << is_regular(
               "/home/cn/work/cpp/LinuxC/linux-cpp/Boost/FileSystem/Main.cxx")
        << std::endl;
    std::cout
        << "is_regular: "
        << is_regular(
               "/home/cn/work/cpp/LinuxC/linux-cpp/Boost/FileSystem/Main.cpp")
        << std::endl;
  }

  void test_directory()
  {
    std::string        path = "/home/cn/work/cpp/LinuxC/linux-cpp/";
    directory_iterator end;
    for(directory_iterator pos(path); pos != end; ++pos)
      std::cout << *pos << std::endl;
  }

  void recursive_dir(const path& dir)
  {
    directory_iterator end;
    for(directory_iterator pos(dir); pos != end; ++pos)
    {
      if(is_directory(*pos))
        recursive_dir(*pos);
      else
        std::cout << *pos << std::endl;
    }
  }

  bool is_ignore_file(const path& str_path)
  {
    const std::set<path> ignore_file = {path("build")};
    for(const auto& item : ignore_file)
    {
      if(item == str_path)
        return true;
    }
    return false;
  }

  void recursive_dir2(const path& dir)
  {
    typedef recursive_directory_iterator rd_iterator;
    rd_iterator                          end;
    for(rd_iterator pos(dir); pos != end; ++pos)
    {
      path p(*pos);

      if(is_directory(p))
      {
        if(is_ignore_file(p.filename()))
          continue;

        recursive_dir(p);
      }
      else
      {
        // std::cout << "pos level: "<< pos.level() << " : " << *pos << ": " <<
        // current_path() << std::endl; std::cout << "pos level: "<< pos.level()
        // << " path : " << current_path() << std::endl;

        std::cout << "file infos: ======================================\n";
        std::cout << "name: \t" << p.filename() << "\n";
        std::cout << "extention: \t" << p.extension() << "\n";
        // std::cout << "path: \t" << current_path(p) << "\n";
        std::cout << "size: \t" << file_size(p) << "\n";
        std::cout << "time: \t" << last_write_time(p) << "\n";
      }
    }
  }

  void main()
  {
    // test1();
    // test_file_status();
    // is_file_status();
    // test_directory();
    recursive_dir("/home/cn/work/cpp/LinuxC/linux-cpp/Boost");
    // recursive_dir2("/home/cn/work/cpp/LinuxC/linux-cpp/Boost/FileSystem");
  }
} // namespace ns_path

namespace ns_file_crypto
{
  bool
  GetMd5(std::string& str_md5, const char* const buffer, size_t buffer_size)
  {
    if(buffer == nullptr)
    {
      return false;
    }

    boost::uuids::detail::md5 boost_md5;
    boost_md5.process_bytes(buffer, buffer_size);
    boost::uuids::detail::md5::digest_type digest;
    boost_md5.get_digest(digest);
    const auto char_digest = reinterpret_cast<const char*>(&digest);
    str_md5.clear();
    boost::algorithm::hex(char_digest,
                          char_digest +
                              sizeof(boost::uuids::detail::md5::digest_type),
                          std::back_inserter(str_md5));

    return true;
  }

  bool
  GetSHA1(std::string& str_sha1, const char* const buffer, size_t buffer_size)
  {
    char                       hash[20];
    boost::uuids::detail::sha1 boost_sha1;
    boost_sha1.process_bytes(buffer, buffer_size);
    boost::uuids::detail::sha1::digest_type digest;
    boost_sha1.get_digest(digest);
    for(int i = 0; i < 5; ++i)
    {
      const char* tmp = reinterpret_cast<char*>(digest);
      hash[i * 4]     = tmp[i * 4 + 3];
      hash[i * 4 + 1] = tmp[i * 4 + 2];
      hash[i * 4 + 2] = tmp[i * 4 + 1];
      hash[i * 4 + 3] = tmp[i * 4];
    }

    str_sha1.clear();
    std::ostringstream buf;
    for(int i = 0; i < 20; ++i)
    {
      buf << setiosflags(ios::uppercase) << std::hex
          << ((hash[i] & 0x0000000F0) >> 4);
      buf << setiosflags(ios::uppercase) << std::hex << (hash[i] & 0x00000000F);
    }

    str_sha1 = buf.str();
    return true;
  }
  const std::string md5_from_file(const std::string& path)
  {
    unsigned char                        result[MD5_DIGEST_LENGTH];
    boost::iostreams::mapped_file_source src(path);
    MD5((unsigned char*)src.data(), src.size(), result);

    std::ostringstream sout;
    sout << std::hex << std::setfill('0');
    for(auto c : result)
      sout << std::setw(2) << (int)c;

    return sout.str();
  }

  void test1()
  {
    string str_md5;
    string str_sha1;
    string str = "1@948-3*9:-=!@#$%^&**()_+?><~`{}[]|;,.";

    if(GetMd5(str_md5, str.c_str(), str.length()) == true)
    {
      cout << "str_md5 : " << str_md5 << endl;
    }
    else
    {
      cout << "error .." << endl;
    }

    cout << "----------------------------------------------" << endl;

    if(GetSHA1(str_sha1, str.c_str(), str.length()) == true)
    {
      cout << "str_sha1 : " << str_sha1 << endl;
    }
    else
    {
      cout << "error .." << endl;
    }
  }

  void test2()
  {
    std::cout << "file prop"
              << md5_from_file("/home/cn/work/cpp/LinuxC/linux-cpp/Boost/"
                               "FileSystem/CMakeLists.txt")
              << std::endl;
  }

  void main()
  {
    // test1();
    test2();
  }
} // namespace ns_file_crypto

int main()
{
  // ns_test1::main();
  // ns_path::main();
  // return 0;
  // ns_path::main();
  ns_file_crypto::main();
  return 0;
}