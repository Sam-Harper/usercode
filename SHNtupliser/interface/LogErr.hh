#ifndef LOGERR
#define LOGERR

//defines my error logging macro, macros are evil but I think this one is okay

//#include "SHUtility/LogFuncs.hh"
#include <iostream>
#include <string>
#define LogErr std::cout <<"in "<<__FUNCTION__<<"() (in "<<__FILE__<<", line "<<__LINE__<<") :"
//#define LogErrEvtInfo LogErr <<LogFuncs::evtInfo()<<":"
#define LogErrStr std::string("in ")+__FUNCTION__+"() (in "+__FILE__+", line "+std::to_string(__LINE__)+") :"

#endif

