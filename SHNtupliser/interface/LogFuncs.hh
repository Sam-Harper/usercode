#ifndef LOGFUNCS
#define LOGFUNCS

//class is designed to help with logging info
//currently it helps make the run and event number globally availible

#include <iostream>

class SHEvent;


namespace LogFuncs {

  class EvtInfo {
  private:
    const SHEvent* evt_; //unhappy with this solution as the temptation to make evt_ availible globally using this function is great
  public:
    EvtInfo(){}
    ~EvtInfo(){};
    
    int runnr()const;
    int eventnr()const;

    void setEvent(SHEvent* evt){evt_=evt;}

  };

  const EvtInfo& evtInfo();

  void setEvent(SHEvent* event);

}


std::ostream &operator <<(std::ostream& output,const LogFuncs::EvtInfo& evtInfo);


#endif
