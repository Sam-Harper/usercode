#include "SHarper/SHNtupliser/interface/LogFuncs.hh"

#include "SHarper/SHNtupliser/interface/SHEvent.hh"

namespace LogFuncs{
  EvtInfo evtInfoObj;
}

const LogFuncs::EvtInfo& LogFuncs::evtInfo()
{
  return evtInfoObj;

}

void LogFuncs::setEvent(SHEvent* event)
{
  evtInfoObj.setEvent(event);
}

int LogFuncs::EvtInfo::runnr()const
{
  return evt_->runnr();
}

int LogFuncs::EvtInfo::eventnr()const
{
  return evt_->eventnr();
  
}

std::ostream &operator <<(std::ostream& output,const LogFuncs::EvtInfo& evtInfo)
{
  output << "runnr: "<<evtInfo.runnr()<<" eventnr: "<<evtInfo.eventnr()<<std::endl;
  return output;

}
