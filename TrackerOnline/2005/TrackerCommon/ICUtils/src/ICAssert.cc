#include "ICAssert.hh"

#undef IC_ICUTILS_ASSERT_A
#undef IC_ICUTILS_ASSERT_B

// #include "TypeDefs.hh"
// #include "ICLog.hh"

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cctype>

namespace ICUtils {

static const volatile char id[] = "@(#) $Id: ICAssert.cc,v 1.2 2005/11/28 10:08:29 mirabitl Exp $";
inline void suppress_unused_var_warning() { id; }

using std::cerr;
using std::endl;

ICAssert::ICAssert(const char * expr, const char * file, long line, const char * func) :
  IC_ICUTILS_ASSERT_A(*this), IC_ICUTILS_ASSERT_B(*this),
  _file(file), _line(line), _func(func), _expr(expr), _msg(0), _code(0)
{ }

ICAssert::string ICAssert::getDescription() const {
  std::ostringstream str;
  str << getLevel() << ": Assertion failed in "
      << _file << " : " << _line << "\n";
  if (_func && *_func) { str << "  Function: \'" << _func << "\'\n"; }
  str << getDesc();
  return str.str();
}

ICAssert::string ICAssert::getDesc() const {
  std::ostringstream str;
  if (_msg) { str << "  Message: " << _msg << "\n"; }
  str << "  Expression: \'" << _expr << "\'\n";
  if (_values.size() > 0) {
    str << "  Values: " << _values[0].first << " = " << _values[0].second << "\n";
    for (size_t i = 1; i < _values.size(); i++) {
      str << "          " << _values[i].first << " = " << _values[i].second;
      if (i < _values.size()-1) str << "\n";
    }
  }
  return str.str();
}

ICAssert::Action ICAssert::handleAssertion(Action & override) const {
  const string text = getDescription();
  //log.stamp() << text;
  if (override != act_none) {
    //log << "Action taken: override: " << getAction(override) << endl << endl;
    return override;
  }
//   switch (override) {
//   case ovr_ignore: return act_ignore;
//   case ovr_throw:  return act_throw;
//   default: break; // carry on
//   }
  Action action;
  switch (_level) {
  case lvl_warn:  action = act_ignore;   break;
  case lvl_error: action = act_throw;    break;
  case lvl_fatal: action = act_coredump; break;
  default:        action = askUser(text.c_str());
  }
  //log.stamp() << "Action taken: " << getAction(action) << endl << endl;
  //cerr << "Action taken: " << getAction(action) << endl << endl;
  if (action == act_aignore) { override = action = act_ignore; }
  if (action == act_athrow ) { override = action = act_throw;  }
  return action;
}

bool ICAssert::handleAssertionDebug(Action & override) const {
  switch (handleAssertion(override)) {
  case ICAssert::act_ignore: return false;
  case ICAssert::act_throw: return true;
  case ICAssert::act_exit: exit(EXIT_FAILURE);
  case ICAssert::act_coredump: abort();
  case ICAssert::act_break: IC_DEBUG_BREAK;
  default: return true; /* shouldn't get here */
  }
}
   
ICAssert::Action ICAssert::askUser(const char * text) const {
  static const char* actions = "xcbight";
  Action ret = act_coredump;
  cerr << text << "\nWhat action should be taken? possibilities are:" << endl
       << "I: Ignore" << endl
       << "G: Always ignore" << endl
       << "T: Throw exception" << endl
       << "H: Always throw exception" << endl
       << "B: Break into debugger" << endl
       << "X: Exit program" << endl
       << "C: Core dump" << endl
    ;
  char c;
  //ifstream console("/dev/tty");
  for(;;) {
    //console.get(c);
    std::cin.get(c);
    if (strchr(actions, tolower(c))) {
      ret = static_cast<Action>(tolower(c));
      break;
    }
  }
  // should remove remaining characters from input here (up to newline)
  return ret;
}

const char* ICAssert::getLevel() const {
  switch (_level) {
  case lvl_warn:  return "Warning";
  case lvl_debug: return "Debug";
  case lvl_error: return "Error";
  case lvl_fatal: return "Fatal";
  default:        return "Bug (should never see this)";
  }
}

const char* ICAssert::getAction(Action a) const {
  switch (a) {
  case act_none:     return "No action";
  case act_ignore:   return "Ignore";
  case act_aignore:  return "Always ignore";
  case act_throw:    return "Throw exception";
  case act_athrow:   return "Always throw";
  case act_exit:     return "Exit program";
  case act_coredump: return "Core dump";
  case act_break:    return "Break into debugger";
  default:           return "Bug (should never see this)";
  }
}

}
