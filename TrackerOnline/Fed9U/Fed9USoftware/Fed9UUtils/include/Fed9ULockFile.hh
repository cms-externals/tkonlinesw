#ifndef H_Fed9ULockFile
#define H_Fed9ULockFile

#include <string>
#include <sstream>

#include "ICExImpl.hh"

namespace Fed9U {

  /**
   * \brief Provides a base class to open a file and lock it, such that there can only be one instance of that file open.
   *
   * The class is intended for use as a base class and as such its constructor and member functions are declared protected and only accessible by
   * derived classes. Derived classes have the ability to read/write from the file. One public member function is provided to determine if the file is new or not.
   */
  class Fed9ULockFileBase {

  public:

    /**
     * \brief  Returns true if the opened file is new.
     * \return bool
     */
    bool isNewFile() const { return _newfile; }

  protected:

    /**
     * \brief Constructor.
     *
     * \param filename Constant reference to a string that contains the file to be opened.
     *
     * Opens the file specified by filename for read/write access, determining if it is a new file or not.
     */
    Fed9ULockFileBase(const std::string & filename);

    /**
     * \brief Destructor.
     *
     * Closes any open files.
     */
    virtual ~Fed9ULockFileBase();

    /**
     * \brief  Reads the contents of the file to a string.
     * \return string
     */
    std::string readFileString() const;

    /**
     * \brief Writes the contents of a string to the file.
     * \param string Constant reference to the string that is to be written to the file.
     *
     * This will over write any existing data in the file.
     */
    void writeFileString(const std::string &) const;

    int _fd;       //!< Stores the ID of the file that has been opened.

    bool _newfile; //!< Flag to determine if the it is a new file or not.

  };//class Fed9ULockFile


  /**
   * \brief Derived from Fed9ULockFileBase, this templated class allows a file to be synchronised with any type T that has overloaded input/output stream
   *        operators.
   *
   * The type T is loaded from file upon construction of the class, if there is any data from the file to read. Otherwise the contents of type T can be used
   * to update the file. Locks the opened file so only one point of access is allowed to the file, this class.
   */
  template <typename T>
  class Fed9ULockFile : public Fed9ULockFileBase {
  public:

    /**
     * \brief Constructor.
     * \param filename Name of the file to be read/written from.
     * \param def Constant reference to the type that is to be synchronised with the file.
     *
     * Makes a local copy of the object T and opens the file specified. If the file is not new then the contents of the file will be streamed to the type T,
     * otherwise the file is just opened and T left unaffected.
     */
    Fed9ULockFile(const std::string & filename, const T & def = 0)
      : Fed9ULockFileBase(filename), _val(def) {
      if (!_newfile) {
	std::istringstream stream(readFileString());
	if (stream.str().length() > 0) {
	  stream >> _val;
	  if (!stream) THROW(ICUtils::ICException("Error reading from file"));
	} else {
	  THROW(ICUtils::ICException("File is empty"));
	}
      }
    }

    /**
     * \brief Destructor.
     *
     * Synchronises the contents of the type T with the file, overwritting any data that maybe in the file.
     */
    ~Fed9ULockFile() {
      sync();
    }

    /**
     * \brief  Returns a writable reference to the object T that contains the file contents.
     * \return T&
     */
    T & value() { return _val; }

    /**
     * \brief  Returns a read only reference to the object T that is synchronised with the file.
     * \return const T&
     */
    const T & value() const { return _val; }

    /**
     * \brief  Overload of the operator(), which returns a reference to the type T that is synchronised with the file.
     */
    operator T & () { return _val; }

    //operator const T & () const { return value(); }

    /**
     * \brief  Synchronises the type T with the file, by writing the contents of T to the file.
     */
    Fed9ULockFile & sync() {
      std::ostringstream buf;
      buf << _val << "\n";
      writeFileString(buf.str());
      return *this;
    }
  private:
    T _val;
  };

}

#endif // H_Fed9ULockFile
