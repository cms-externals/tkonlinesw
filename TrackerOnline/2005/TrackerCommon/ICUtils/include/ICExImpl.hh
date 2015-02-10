#include "ICException.hh"
#include "ICAssert.hh"

#ifdef IC_DEF_ERROR
#undef IC_DEF_ERROR
#endif

#define IC_DEF_ERROR IC_GENERATE_STRINGS
