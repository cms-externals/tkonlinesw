#include <cerrno>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/file.h>
#include <unistd.h>

#include "ICAssert.hh"
#include "Fed9UFileHelpers.hh"

namespace Fed9U {

  int Fed9UFile_open(const std::string & filename, bool & newfile) {
    int fd = open(filename.c_str(), O_RDONLY | O_NONBLOCK,
		  S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
    if (fd >= 0) {
      close(fd);
    }
    newfile = (fd < 0);
    fd = open(filename.c_str(), O_RDWR | O_CREAT | O_NONBLOCK,
	      S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
    int errornum = errno;
    ICUTILS_VERIFY(fd >= 0)(filename)(errornum)(fd).msg("Unable to open file").error();
    if (flock(fd, LOCK_EX | LOCK_NB) < 0) {
      errornum = errno;
      close(fd);
      ICUTILS_VERIFY(false)(filename)(errornum)(fd).msg("Unable to lock file").error();
    }
    return fd;
  }

  void Fed9UFile_close(int fd) {
    close(fd);
  }

  std::string Fed9UFile_read(int fd) {
    std::string str;
    char buf[1024];
    ssize_t count;
    while ((count = read(fd, buf, sizeof buf)) != 0) {
      ICUTILS_VERIFY(count >= 0)(errno)(fd).msg("Error reading from file").error();
      str.append(buf, count);
    }
    return str;
  }

  void Fed9UFile_overwrite(int fd, const std::string & data) {
    off_t temp = lseek(fd, 0, SEEK_SET);
    ssize_t written = write(fd, data.c_str(), data.length());
    int errornum = errno;
    ftruncate(fd, written);
    fsync(fd);
    ICUTILS_VERIFY((unsigned int)written == data.length())(written)(data.length())(errornum)(fd)
      .msg("Error writing to file").error();
  }

}
