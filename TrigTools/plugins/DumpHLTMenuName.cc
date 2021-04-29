
#include "HLTrigger/HLTcore/interface/HLTFilter.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"

#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Provenance/interface/ParameterSetID.h"

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include <vector>
#include <string>

namespace trigger{
  class TriggerEvent;
}

namespace edm{
  class Event;
  class EventSetup;
  class ParameterSet;
}


class DumpHLTMenuName : public edm::EDAnalyzer {


private:
  std::string hltProcess_;
  std::vector<std::string> pathsToPrintFilters_;
public:
  explicit DumpHLTMenuName(const edm::ParameterSet& iPara):
    hltProcess_(iPara.getParameter<std::string>("hltProcess")),
    pathsToPrintFilters_(iPara.getParameter<std::vector<std::string>>("pathsToPrintFilters")){}
  ~DumpHLTMenuName(){}
  
 private:
  virtual void beginJob(){}
  virtual void beginRun(const edm::Run& run,const edm::EventSetup& iSetup);
  virtual void analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup){}
  virtual void endJob(){}
  
  

};

void DumpHLTMenuName::beginRun(const edm::Run& run,const edm::EventSetup& setup)
{
  bool changed=false;
  HLTConfigProvider hltConfig;
  hltConfig.init(run,setup,hltProcess_,changed);
  if(changed){
    std::cout <<"table name "<<hltConfig.tableName()<<std::endl;
    auto trigNames = hltConfig.triggerNames();
  
    for(const auto& pathPattern : pathsToPrintFilters_){
      for(const auto& pathName : trigNames){
	if(pathName.find(pathPattern)==0){
	  std::cout <<"path "<<pathName<<" filters"<<std::endl;
	  const auto modLabels = hltConfig.saveTagsModules(pathName);
	  for(const auto& modLabel : modLabels){
	    std::cout <<"   "<<modLabel<<std::endl;
	  }
	}
      }
    }
  }
	  
 
}

DEFINE_FWK_MODULE(DumpHLTMenuName);
