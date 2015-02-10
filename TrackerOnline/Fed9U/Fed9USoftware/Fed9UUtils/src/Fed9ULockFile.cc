#include "Fed9ULockFile.hh"
#include "Fed9UFileHelpers.hh"

namespace Fed9U {

  Fed9ULockFileBase::Fed9ULockFileBase(const std::string & filename) {
    _fd = Fed9UFile_open(filename, _newfile);
  }

  Fed9ULockFileBase::~Fed9ULockFileBase() {
    Fed9UFile_close(_fd);
  }

  std::string Fed9ULockFileBase::readFileString() const {
    return Fed9UFile_read(_fd);
  }

  void Fed9ULockFileBase::writeFileString(const std::string & str) const {
    Fed9UFile_overwrite(_fd, str);
  }
  
}
