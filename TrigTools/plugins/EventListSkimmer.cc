
#include "FWCore/Framework/interface/one/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include <vector>
#include <algorithm>
#include <iostream>
#include <limits>
#include <fstream>
#include <sstream>

class EventListSkimmer : public edm::one::EDFilter<>{
  
public:
  
  class EvtList {
  public:
    struct LumiData {
      int lumiSec;
      std::vector<int> events;
      
      LumiData(int iLumiSec):lumiSec(iLumiSec){}
      void sort(){std::sort(events.begin(),events.end());}
      bool operator<(const LumiData& rhs)const{return lumiSec<rhs.lumiSec;}
      bool operator<(int rhs)const{return lumiSec<rhs;} 
      bool operator==(const LumiData& rhs)const{return lumiSec==rhs.lumiSec;}
      bool operator==(int rhs)const{return lumiSec==rhs;}
      friend bool operator<(int lhs,const LumiData& rhs);
    };
    
    struct RunData {
      int runnr;
      std::vector<LumiData> lumis;
      
      RunData(int iRunnr):runnr(iRunnr){}
      void sort();
      bool operator<(const RunData& rhs)const{return runnr<rhs.runnr;}
      bool operator<(int rhs)const{return runnr<rhs;}
      bool operator==(const RunData& rhs)const{return runnr==rhs.runnr;}
      bool operator==(int rhs)const{return runnr==rhs;}
      friend bool operator<(int lhs,const RunData& rhs);
    };
    
  private:
    std::vector<RunData> runs_;
    
    mutable size_t lastRunIndex_;
    mutable size_t lastLumiIndex_;

  public:
    
    
    EvtList():lastRunIndex_(std::numeric_limits<size_t>::max()),lastLumiIndex_(std::numeric_limits<size_t>::max()){}
    ~EvtList(){}
    
    void add(int runnr,int lumiSec,int eventnr);
    void sort();
    void read(std::istream& stream);
    
    bool hasEvt(int runnr,int lumiSec,int eventnr)const;
    
  private:
    const RunData* lastRun_()const{return lastRunIndex_<runs_.size() ? &runs_[lastRunIndex_] : 0;}
    RunData* lastRun_(){return lastRunIndex_<runs_.size() ? &runs_[lastRunIndex_] : 0;}
    
    const LumiData* lastLumi_()const{return lastRun_() && lastLumiIndex_<lastRun_()->lumis.size() ? &(lastRun_()->lumis[lastLumiIndex_]) : 0;}
    LumiData* lastLumi_(){return lastRun_() && lastLumiIndex_<lastRun_()->lumis.size() ? &(lastRun_()->lumis[lastLumiIndex_]) : 0;}
    
    
  };
  
  
private:
  std::string filename_;
  EvtList evtList_;

public:
  explicit EventListSkimmer(const edm::ParameterSet&);
  ~EventListSkimmer(){}
  
  virtual void beginJob();
  virtual bool filter(edm::Event& ,const edm::EventSetup& );
  virtual void endJob(){}
  
 
};

EventListSkimmer::EventListSkimmer(const edm::ParameterSet& iConfig)
{
  filename_ = iConfig.getParameter<std::string>("filename");
 
}

void EventListSkimmer::beginJob()
{ 
  std::string path(getenv("CMSSW_BASE"));
  path+="/src/";
  std::ifstream file(path+filename_);
  evtList_.read(file);
  
}

bool EventListSkimmer::filter(edm::Event& iEvent,const edm::EventSetup& iSetup )
{
  return evtList_.hasEvt(iEvent.id().run(),iEvent.luminosityBlock(),iEvent.id().event());
 
}


template <typename TCont,typename TVal> const size_t findSingleSortedIndex(const TCont& container,const TVal& val)
{
  // typename TCont::const_iterator;
  std::pair<typename TCont::const_iterator,typename TCont::const_iterator> result = std::equal_range(container.begin(),container.end(),val);
  int nrFound = std::distance(result.first,result.second);
  if(nrFound==1) return std::distance(container.begin(),result.first);
  else if(nrFound>=1) {
    std::cout <<"warning "<<nrFound<<" instead of 1 for "<<val<<std::endl;
    return std::distance(container.begin(),result.first);
  }
  return container.size();
}
template <typename TCont,typename TVal> const size_t findSingleIndex(const TCont& container,const TVal& val)
{
  //typename TCont::const_iterator;
  typename TCont::const_iterator result = std::find(container.begin(),container.end(),val);
  return std::distance(container.begin(),result);
}
bool operator<(int lhs,const EventListSkimmer::EvtList::RunData& rhs){return lhs<rhs.runnr;}
bool operator<(int lhs,const EventListSkimmer::EvtList::LumiData& rhs){return lhs<rhs.lumiSec;}
bool operator==(int lhs,const EventListSkimmer::EvtList::RunData& rhs){return lhs==rhs.runnr;}
bool operator==(int lhs,const EventListSkimmer::EvtList::LumiData& rhs){return lhs==rhs.lumiSec;}

bool EventListSkimmer::EvtList::hasEvt(int runnr,int lumiSec,int eventnr)const
{ 
  //std::cout <<"runnr "<<runnr<<" lumiSec "<<lumiSec<<" eventnr "<<eventnr<<" lastRun "<<lastRun_()<<" "<<lastLumi_()<<" "<<lastRunIndex_<<" "<<lastLumiIndex_<<std::endl;
  if(lastRun_() && lastRun_()->runnr==runnr){
    if(!lastLumi_() || lastLumi_()->lumiSec!=lumiSec){
      lastLumiIndex_ =  findSingleSortedIndex(lastRun_()->lumis,lumiSec);
    }
  }else{
    
    lastRunIndex_ = findSingleSortedIndex(runs_,runnr);
    lastLumiIndex_ = lastRun_() ? findSingleSortedIndex(lastRun_()->lumis,lumiSec) : std::numeric_limits<size_t>::max(); 
    //  std::cout <<" now testing runnr "<<runnr<<" lumiSec "<<lumiSec<<" eventnr "<<eventnr<<" lastRun "<<lastRun_()<<" "<<lastLumi_()<<" "<<lastRunIndex_<<" "<<lastLumiIndex_<<std::endl;
  }
  
  // std::cout <<"runnr "<<runnr<<" lumiSec "<<lumiSec<<" eventnr "<<eventnr<<" lastRun "<<lastRun_()<<" "<<lastLumi_()<<" pass "<<findSingleSortedIndex(lastLumi_()->events,eventnr)<<" "<<lastLumi_()->events.size()<<std::endl;

  if(!lastRun_() || !lastLumi_()) return false;
  else return findSingleSortedIndex(lastLumi_()->events,eventnr)<lastLumi_()->events.size();
  
}

void EventListSkimmer::EvtList::add(int runnr,int lumiSec,int eventnr)
{
  if(!lastRun_() || lastRun_()->runnr!=runnr){ 
    lastLumiIndex_=std::numeric_limits<size_t>::max(); //lumisec is from a different run, is invalid
    //std::cout <<"last run index "<<lastRunIndex_<<" runnr "<<runnr<<std::endl;
    lastRunIndex_ = findSingleIndex(runs_,runnr);
    //std::cout <<"updated "<<lastRunIndex_<<std::endl;
    if(!lastRun_()){  
      // std::cout <<"adding run "<<runnr<<" "<<lastRun_()<<" lastRunIndex "<<lastRunIndex_<<std::endl;
      lastRunIndex_ =runs_.size();
      runs_.push_back(runnr);

    }
  }
  
  
  if(!lastLumi_() || lastLumi_()->lumiSec!=lumiSec){
    lastLumiIndex_ =  findSingleIndex(lastRun_()->lumis,lumiSec);
  } 
  
  if(!lastLumi_()){
    lastLumiIndex_ = lastRun_()->lumis.size();
    lastRun_()->lumis.push_back(lumiSec);
    
  }
 
  lastLumi_()->events.push_back(eventnr);
}
      
void EventListSkimmer::EvtList::sort()
{
  lastRunIndex_=std::numeric_limits<size_t>::max();
  lastLumiIndex_=std::numeric_limits<size_t>::max();
  std::sort(runs_.begin(),runs_.end());
  std::for_each(runs_.begin(),runs_.end(),std::mem_fun_ref(&EventListSkimmer::EvtList::RunData::sort));
  
}

void EventListSkimmer::EvtList::RunData::sort()
{
  std::sort(lumis.begin(),lumis.end());
  std::for_each(lumis.begin(),lumis.end(),std::mem_fun_ref(&EventListSkimmer::EvtList::LumiData::sort));
}
void EventListSkimmer::EvtList::read(std::istream& stream)
{ 
  std::string line;
  //  std::cout <<"reading "<<std::endl;
  while(std::getline(stream,line)){
    // std::cout <<"line "<<line<<std::endl;
    line.erase( std::find( line.begin(), line.end(), '#' ), line.end() ); //ignore comments
    std::istringstream lineStream(line);
    int runnr,lumiSec,eventnr;
    while(lineStream >> runnr >> lumiSec >> eventnr ){
      //std::cout <<"pusing back "<<runnr<<" "<<lumiSec<<" "<<eventnr<<std::endl;
      add(runnr,lumiSec,eventnr);
    }
  }
  if(runs_.empty()) std::cout <<"nothing read from file "<<std::endl;
  sort();  
}


DEFINE_FWK_MODULE(EventListSkimmer);
