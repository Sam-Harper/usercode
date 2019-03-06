



#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "HLTrigger/HLTcore/interface/HLTPrescaleProvider.h"

#include <vector>
#include <string>
#include <iostream>

/****************************************************************************
 Simple example on how to access the L1 decision via  l1t::L1TGlobalUtil
 To make things easier it uses HLTPrescaleProvider to obtain the  
 l1t::L1TGlobalUtil object

 note, its very important that you run with the correct global tag as it
 retrieves the menu from this

 

 author Sam  Harper (RAL), 2019
*****************************************************************************/

class L1MenuExample : public edm::EDAnalyzer {
 
private:
  
  HLTPrescaleProvider hltPSProv_;  
  std::string hltProcess_; //name of HLT process, usually "HLT"

public:
  explicit L1MenuExample(const edm::ParameterSet& iConfig);
  ~L1MenuExample(){}
  
 private:
  virtual void beginJob(){}
  virtual void beginRun(const edm::Run& run,const edm::EventSetup& iSetup);
  virtual void analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup);
  virtual void endJob(){}
};



L1MenuExample::L1MenuExample(const edm::ParameterSet& iConfig):
  hltPSProv_(iConfig,consumesCollector(),*this), //it needs a referernce to the calling module for some reason, hence the *this
  hltProcess_(iConfig.getParameter<std::string>("hltProcess"))
{

}

//we need to initalise the menu each run (menu can and will change on run boundaries)
void L1MenuExample::beginRun(const edm::Run& run,const edm::EventSetup& setup)
{
  bool changed=false;
  hltPSProv_.init(run,setup,hltProcess_,changed);
  const l1t::L1TGlobalUtil& l1GtUtils = hltPSProv_.l1tGlobalUtil();
  std::cout <<"l1 menu "<<l1GtUtils.gtTriggerMenuName()<<" version "<<l1GtUtils.gtTriggerMenuVersion()<<" comment "<<std::endl;
  std::cout <<"hlt name "<<hltPSProv_.hltConfigProvider().tableName()<<std::endl;
}

void L1MenuExample::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  //I seem to recall this function being slow so perhaps cache for a given lumi
  //(it only changes on lumi boundaries)
  int psColumn = hltPSProv_.prescaleSet(iEvent,iSetup);
  std::cout <<"PS column "<<psColumn<<std::endl;
  if(psColumn==0 && iEvent.isRealData()){
    std::cout <<"PS column zero detected for data, this is unlikely (almost all triggers are disabled in normal menus here) and its more likely that you've not loaded the correct global tag in "<<std::endl;
  }

  //note to the reader, what I'm doing is extremely dangerious (a const cast), never do this!
  //however in this narrow case, it fixes a bug in l1t::L1TGlobalUtil (the method should be const)
  //and it is safe for this specific instance
  l1t::L1TGlobalUtil& l1GtUtils = const_cast<l1t::L1TGlobalUtil&> (hltPSProv_.l1tGlobalUtil());
  std::cout <<"l1 menu: name decisions prescale "<<std::endl;
  for(size_t bitNr=0;bitNr<l1GtUtils.decisionsFinal().size();bitNr++){
    const std::string& bitName = l1GtUtils.decisionsFinal()[bitNr].first; // l1GtUtils.decisionsFinal() is of type std::vector<std::pair<std::string,bool> >

    bool passInitial = l1GtUtils.decisionsInitial()[bitNr].second; //before masks and prescales, so if we have a 15 GeV electron passing L1_SingleEG10, it will show up as true but will likely not cause a L1 acccept due to the seeds high prescale
    bool passInterm = l1GtUtils.decisionsInterm()[bitNr].second; //after mask (?, unsure what this is)
    bool passFinal = l1GtUtils.decisionsFinal()[bitNr].second; //after masks & prescales, true means it gives a L1 accept to the HLT
    int prescale = l1GtUtils.prescales()[bitNr].second;
    std::cout <<"   "<<bitNr<<" "<<bitName<<" "<<passInitial<<" "<<passInterm<<" "<<passFinal<<" "<<prescale<<std::endl;
  }
}



//define this as a plug-in
DEFINE_FWK_MODULE(L1MenuExample);
