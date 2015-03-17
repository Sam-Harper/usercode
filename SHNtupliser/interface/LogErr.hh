#ifndef LOGERR
#define LOGERR

//defines my error logging macro, macros are evil but I think this one is okay

//#include "SHUtility/LogFuncs.hh"
#include <iostream>
#define LogErr std::cout <<"in "<<__FUNCTION__<<"() (in "<<__FILE__<<", line "<<__LINE__<<") :"
//#define LogErrEvtInfo LogErr <<LogFuncs::evtInfo()<<":"


#endif

