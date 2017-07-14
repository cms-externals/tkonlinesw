#ifndef H_ICException
#define H_ICException

#include <stdexcept>
#include <string>

namespace ICUtils {

#define IC_GENERATE_ENUMS(name, contents) name,
#define IC_GENERATE_STRINGS(name, contents) contents,

#define IC_EXCEPTION_CLASS_BEGIN(name, codes)    \
class name : public ICUtils::ICException {       \
 public:                                         \
  enum ICErrorCode {                             \
    codes                                        \
    IC_NUM_ERRORS                                \
  };                                             \
  name(ICErrorCode code, const string& description = "");                         \
  name(const ICUtils::ICAssert &);                                                \
  name & setLocation(const char * file, long line, const char * func = "") {      \
    ICUtils::ICException::setLocation(file, line, func); return *this;            \
  }                                                                               \
  name & setPrev(const char * prev) { ICUtils::ICException::setPrev(prev); return *this; } \
  ICErrorCode getErrorCode() const { return _errorCode; }                         \
 protected:                                      \
  ICErrorCode _errorCode;                        \
  static const char * _errorStrings[];           \
 private:

#define IC_EXCEPTION_CLASS_END };

#define IC_EXCEPTION_IMPLEMENTATION(name, codes)                   \
name::name(ICErrorCode code, const string& description) :          \
    ICUtils::ICException(#name, _errorStrings[code], description), \
    _errorCode(code) { }                                           \
name::name(const ICUtils::ICAssert & ass) :                        \
    ICUtils::ICException(#name, ass),                              \
    _errorCode(static_cast<name::ICErrorCode>(ass.getCode())) { }  \
const char * name::_errorStrings[] = { codes "Unknown Error Code" };

#define THROW(exce) throw (exce).setLocation(__FILE__, __LINE__, IC_FUNCTION)
#define RETHROW(prev, exce) THROW(exce).setPrev(prev.what())

class ICAssert;

/** 
 * \brief ICException class is designed to be used specifically with the ICSoftware.
 * \author Jonathan Fulcher
 * \author Emlyn Corrin
 * \version 0
 * \date 17 July 2003
 *
 * The base class is just an abstract class to decouple the 
 * exceptions from normal std::exceptions. When deriving an exception class
 * from this class care must be taken to ensure that the error definitions 
 * are constructed. the constructor of the derived class should be implemented 
 * and should take parameters: ICErrorCode, const char * error, const char * rethrownError
 * In order to construct the error codes and descriptions use the macros provided.
 * and example of a derived class definition is as follows:
 *
 \verbatim
 #include "ICException.hh"

 namespace IC {

 #define IC_EXAMPLE_EXCEPTION_CODES_LIST \
 IC_DEF_ERROR(ERROR_FED_INIT, "FED failed to initialize") \
 IC_DEF_ERROR(ERROR_FED_CRAP, "FED is crap")

 IC_EXCEPTION_CLASS_BEGIN(ICExampleException, IC_EXAMPLE_EXCEPTION_CODES_LIST)
 // User defined methods go here
 IC_EXCEPTION_CLASS_END

 }
 \endverbatim
 *
 * The implimentation would be as follows:
 *
 *
 \verbatim
 #include "ICDeviceException.hh"

 namespace IC {
  
 #undef IC_DEF_ERROR
 #define IC_DEF_ERROR IC_GENERATE_STRINGS

 IC_EXCEPTION_IMPLEMENTATION(ICExampleException, IC_EXAMPLE_EXCEPTION_CODES_LIST);

 }
 \endverbatim
 *
 *****************************************************************************/
  
class ICException : public std::exception {
public:
  typedef std::string string;
  ICException(const string& desc) : _time(getTime()), _name("ICException"), _desc(desc) { }
  ICException(const ICUtils::ICAssert & ass);
  /**
   * A function to retrieve explanations for the reason 
   * of the exception. The user is only allowed to read
   * the text returned by this function. 
   */
  virtual const char* what() const noexcept;
  virtual ~ICException() noexcept {}
  ICException & setLocation(const char * file, long line, const char * func = "") {
    _file = file; _line = line; _func = func; return *this;
  }
  ICException & setPrev(const char * prev) {
    _prev = prev; return *this;
  }
protected:
  /**
   * The constructor is protected so that instances of this class
   * cannot be created, only classes derived from it.
   */
  ICException(const string & name, const string & error, const string & desc) :
    _time(getTime()), _name(name), _error(error), _desc(desc)
  { }
  ICException(const string & name, const ICAssert & ass);
private:
  static string getTime();
  string _file;
  long   _line;
  string _func;
  string _time;
  string _prev;
  string _more;
private:
  string _name;
  string _error;
  string _desc;
  mutable string _text;
  static const char _default_text[];
};

}

#endif // H_ICException
