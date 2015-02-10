#ifndef H_ICAssert
#define H_ICAssert

#define IC_FUNCTION __PRETTY_FUNCTION__
//#define IC_FUNCTION ""
#define IC_DEBUG_BREAK __asm("int3")

#ifndef IC_ICUTILS_ASSERT_DEBUG
# ifdef NDEBUG
#  define IC_ICUTILS_ASSERT_DEBUG 0
# else
#  define IC_ICUTILS_ASSERT_DEBUG 1
# endif
#endif

#include "ICException.hh"

#include <string>
#include <sstream>
#include <vector>
#include <utility> // for std::pair

namespace ICUtils {
/** 
 * \brief An enhanced assert macro.
 * \author Emlyn Corrin
 * \version 0
 * \date 30 July 2003
 * \todo Add the functionality of allowing the user to specify additional arguments
 *       to be passed to a user-defined exception class, instead of just the
 *       description string.
 *
 * There are actually four macros: ICUTILS_ASSERT() and ICUTILS_VERIFY(), ICUTILS_ASSERTX() and ICUTILS_VERIFYX().
 * The ICUTILS_ASSERT() and ICUTILS_ASSERTX() versions only work in debug builds (i.e. NDEBUG is
 * not defined), in release builds they will have no effect. ICUTILS_VERIFY() and ICUTILS_VERIFYX()
 * on the other hand will work in all builds. \n
 * The versions with a trailing X in the name, take a second parameter, which is
 * an exception class to be thrown in the case of an assertion failure. \n
 * The first parameter is the condition to test. If the parameter evaluates to
 * true, then the assert succeeds and no further action is taken. If the parameter
 * evaluates to false, then the assert fails, and one of several actions may be taken.
 * The default action will be to throw an exception in release builds (ICException
 * unless the user specifies another exception class), or in debug builds it will
 * display a menu asking the user what action to take. The user may choose from the
 * following possibilities: \n
 * \li Ignore the error and continue execution of the program.
 * \li Throw an exception.
 * \li Exit the program (through the exit() function).
 * \li Terminate the program with a core dump (through the abort() function).
 * \li Break into a debugger (if not running in a debugger it will simply terminate
 *     the program).
 * \li Always ignore. This will ignore the error, and also ignore this same error
 *     if it occurs again during the same execution of the program.
 * \li Always throw. This is equivalent to the above option, but will always throw
 *     an exception instead of ignoring the error.
 *
 * If the assert fails, a textual description of the error will be generated, which
 * is passed into the exception class if an exception is thrown, or sent to the
 * standard error output in other cases. In addition it is always sent to the log
 * file, along with what action was taken. \n
 * The description includes the file and line number where the assert failed, and
 * the name of the function if the compiler supports this option. In addittion the
 * expression which evaluated to false is included. \n
 * The names and values of additional variables can be included by putting them in
 * parentheses after the assert macro (i.e. 'ICUTILS_ASSERT(x < 100)(x)'). An explanatory
 * message can also be included by appending '.msg("explanatory message")' to the
 * macro after any additional variables. \n
 * The assert can have several levels, specified by appending the appropriate method
 * call to the assert macro:
 * \li .warn();    The message is sent to the standard error and the log, but
 *                         no further action is taken.
 * \li .debug();   The user is asked what action to take, as specified above.
 * \li .error();   An exception (possibly user-specified, otherwise
 *                         ICException) is thrown.
 * \li .fatal();   The program is terminated via the abort() function.
 *
 * Example use:
 \verbatim
 #include "ICAssert.hh"

 // fails if a >= b:
 ICUTILS_ASSERT(a < b);

 // Same but also prints the values of a and b:
 ICUTILS_ASSERT(a < b)(a)(b);

 // Also adds a message to the description:
 ICUTILS_ASSERT(a < b)(a)(b).msg("b must always be greater than a");

 // Terminates the program if the assert fails:
 ICUTILS_ASSERT(a < b)(a)(b).fatal();
 \endverbatim
 * Many of the ideas used here are from articles by Andrei Alexandrescu
 * in the C/C++ Users Journal, for a detailed explanantion, see:
 * http://www.cuj.com/documents/s=8248/cujcexp2104alexandr/ \n
 * http://www.cuj.com/documents/s=8464/cujcexp0308alexandr/ \n
 */
class ICAssert {
public:
  typedef std::string string;
  enum Action { act_none = '?', act_ignore = 'i', act_aignore = 'g', act_throw = 't',
                act_athrow = 'h', act_exit = 'x', act_coredump = 'c', act_break = 'b' };
  enum Level { lvl_warn, lvl_debug, lvl_error, lvl_fatal };
  ICAssert(const char * expr, const char * file, long line, const char * func);
  ICAssert& msg(const char* msg) { _msg = msg; return *this; }
  ICAssert& code(int c) { _code = c; return *this; }
  template<class T>
  ICAssert& printValue(const string & name, const T& value);
  /**
   * \name Level setters
   */
  //@{
  ICAssert& setLevel(Level l) { _level = l; return *this; }
  ICAssert& warn()  { return setLevel(lvl_warn);  }
  ICAssert& debug() { return setLevel(lvl_debug); }
  ICAssert& error() { return setLevel(lvl_error); }
  ICAssert& fatal() { return setLevel(lvl_fatal); }
  //@}
  string getDescription() const;
  string getDesc() const;
  bool handleAssertionDebug(Action & override) const;
  ICAssert& IC_ICUTILS_ASSERT_A;
  ICAssert& IC_ICUTILS_ASSERT_B;

  long   getCode() const { return _code; }
  string getFile() const { return string(_file); }
  long   getLine() const { return _line; }
  string getFunc() const { return string(_func); }
private:
  Action handleAssertion(Action & override) const;
  const char* getLevel() const;
  const char* getAction(Action) const;
  Action askUser(const char * text) const;
  Level _level;
  const char* _file;
  long _line;
  const char* _func;
  const char* _expr;
  const char* _msg;
  long _code;
  typedef std::pair<string, string> ValuePair;
  std::vector<ValuePair> _values;
};

template<class T>
ICAssert& ICAssert::printValue(const string & name, const T& value) {
  std::ostringstream str;
  str << value;
  _values.push_back(ValuePair(name, str.str()));
  return *this;
}

#define IC_ICUTILS_ASSERT_A(x) IC_ICUTILS_ASSERT_OP(x, B)
#define IC_ICUTILS_ASSERT_B(x) IC_ICUTILS_ASSERT_OP(x, A)
#define IC_ICUTILS_ASSERT_OP(x, next) \
  printValue(#x, (x)).IC_ICUTILS_ASSERT_ ## next

#if IC_ICUTILS_ASSERT_DEBUG
# define ICUTILS_ASSERTX(expr, exce) ICUTILS_VERIFYX(expr, exce)
# define ICUTILS_ASSERT(expr)        ICUTILS_VERIFY(expr)
# define ICUTILS_VERIFYX(expr, exce) ICUTILS_VERIFYXL(expr, exce, debug)

#else
# define ICUTILS_ASSERTX(expr, exce) ((void)(0))
# define ICUTILS_ASSERT(expr)        ((void)(0))
# define ICUTILS_VERIFYX(expr, exce) ICUTILS_VERIFYXL(expr, exce, error)
#endif

#define IC_STATIC_error(exce) throw (exce(ass))
#define IC_STATIC_warn(exce)  std::cerr << ass.getDescription() << std::flush; return
#define IC_STATIC_fatal(exce) std::cerr << ass.getDescription() << std::flush; abort()
#define IC_STATIC_debug(exce) \
  static ICUtils::ICAssert::Action override = ICUtils::ICAssert::act_none; \
  if (ass.handleAssertionDebug(override)) throw (exce(ass))

#define ICUTILS_VERIFYXL(expr, exce, levl)                   \
  if (expr) ;                                        \
  else struct ICLocal {                              \
    ICLocal(const ICUtils::ICAssert & ass) {         \
      IC_STATIC_##levl(exce);                        \
    }                                                \
    ~ICLocal() {} /* suppress unused var warning */  \
  } LocalAsserter = ICUtils::ICAssert(#expr, __FILE__, __LINE__, IC_FUNCTION) \
                    .levl().IC_ICUTILS_ASSERT_A

#define ICUTILS_VERIFY(expr) ICUTILS_VERIFYX(expr, ICUtils::ICException)
#define ICUTILS_VERIFYL(expr, levl) ICUTILS_VERIFYXL(expr, ICUtils::ICException, levl)

}

#endif // H_ICAssert
