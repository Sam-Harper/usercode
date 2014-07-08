
#include "FWCore/Framework/interface/EDAnalyzer.h"
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

class EventListDumper : public edm::EDAnalyzer {
  
public:
 
  
private:
  std::string filename_;
  std::ofstream file_;

public:
  explicit EventListDumper(const edm::ParameterSet&);
  ~EventListDumper(){}
  
  virtual void beginJob();
  virtual void analyze(const edm::Event& ,const edm::EventSetup& );
  virtual void endJob(){}
  
 
};

EventListDumper::EventListDumper(const edm::ParameterSet& iConfig)
{
  filename_ = iConfig.getParameter<std::string>("filename");
 
}

void EventListDumper::beginJob()
{ 
  std::string path(getenv("CMSSW_BASE"));
  path+="/src/";
  file_.open((path+filename_));
  file_<<"# list of events: runnr lumisec event"<<std::endl;
  
}

void EventListDumper::analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup )
{
  file_<<iEvent.id().run()<<" "<<iEvent.luminosityBlock()<<" "<<iEvent.id().event()<<std::endl;
 
}




DEFINE_FWK_MODULE(EventListDumper);
