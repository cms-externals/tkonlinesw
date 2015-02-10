#ifndef H_Fed9UFileHelpers
#define H_Fed9UFileHelpers

#include <string>

namespace Fed9U {

  /**
   * \brief  Function in the "global" Fed9U namespace to open a file for reading/writting.
   * \param  filename String containing the name and path of the file to be opened.
   * \param  newfile Returned by reference, the opened file is a new file if true.
   * \return int An identifier for the file that was opened.
   * \throw  ICException This error is thrown if there is any problems opening the file.
   */
  int Fed9UFile_open(const std::string & filename, bool & newfile);

  /**
   * \brief Function in the "global" Fed9U namespace to close a file opened by Fed9UFile_open.
   * \param fd Identifier of the file to be closed.
   */
  void Fed9UFile_close(int fd);

  /**
   * \brief  Function in the "global" Fed9U namespace to read from a file opened by Fed9UFile_open and returns its contents as a string.
   * \param  fd Identifier of the file to be read from.
   * \return string Contains the file data.
   */
  std::string Fed9UFile_read(int fd);

  /**
   * \brief Function in the "global" Fed9U namespace to overwrite the data in a file opened by Fed9UFile_open with data specified by the User.
   * \param fd Identifier of the file to be written to.
   * \param data Constant reference to the data that is to be written.
   */
  void Fed9UFile_overwrite(int fd, const std::string & data);

}

#endif // H_Fed9UFileHelpers
