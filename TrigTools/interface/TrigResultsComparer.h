#ifndef SHARPER_TRIGTOOLS_TRIGRESULTSCOMPARER
#define SHARPER_TRIGTOOLS_TRIGRESULTSCOMPARER

#include "HLTrigger/HLTcore/interface/HLTFilter.h"
#include "HLTrigger/HLTcore/interface/HLTPrescaleProvider.h"

#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Provenance/interface/ParameterSetID.h"

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "SHarper/TrigTools/interface/TrigToolsStructs.h"

#include "TBits.h"
#include "TTree.h"
#include "TFile.h"


#include <vector>
#include <string>

//author S. Harper, RAL
//this class compares the results of two HLT menus using TriggerResults
//events where a trigger fails the prescaler do not count for comparison purposes
//the output is designed for easy manpipulation
//eg if you outputed to a output.txt
//then to see all the triggers which match perfectly and accepted more than 10 events:
// cat output.txt | sed 's/ //g' | awk -F "&" '{if(!($4!=0 || $5!=0) && $3+$4+$5>=10) print $1,$2,$3,$4,$5}'
//to see all which do not match perfectly and accepted more than 10 events
// cat output.txt | sed 's/ //g' | awk -F "&" '{if(($4!=0 || $5!=0) && $3+$4+$5>=10) print $1,$2,$3,$4,$5}'
namespace trigger{
  //  class TriggerEvent;
  class TriggerResults;
  class TriggerNames;
}

namespace edm{
  class Event;
  class EventSetup;
  class ParameterSet;
}



class TrigResultsComparer : public edm::one::EDAnalyzer<> { 

public:
  struct TrigCompData {
    std::string name;
    int menuExistsIn; //0x1 = prev menu, 0x2 = new menu
    size_t nrBothPass;
    size_t nrPrevOnlyPass;
    size_t nrNewOnlyPass;
    TrigCompData():
      name(),menuExistsIn(0),
      nrBothPass(0),nrPrevOnlyPass(0),nrNewOnlyPass(0){}
    TrigCompData(const std::string& iName,int iMenuExistsIn):
      name(iName),menuExistsIn(iMenuExistsIn),
      nrBothPass(0),nrPrevOnlyPass(0),nrNewOnlyPass(0){}
    
    bool operator<(const TrigCompData& rhs)const{return name<rhs.name;}
    bool operator<(const std::string& rhs)const{return name<rhs;}
    std::ostream& print(std::ostream& os)const;
    friend std::ostream& operator<<(std::ostream& os,const TrigCompData& data);
  };

  enum TrigCodeBits{kInMenu=0x1,kWasRun=0x2,kPassed=0x4,kFailedPreScale=0x8}; //I have never come up with a good convention here, I re-use k as prefix

private:
   
  edm::InputTag prevTrigResultsTag_;
  edm::InputTag newTrigResultsTag_;
 
  //we will fill this at begin run from HLTConfig
  //therefore this currently can not handle the case where the menu changes between runs
  std::vector<TrigCompData> pathCompData_; //this vector is unsorted 
  
  HLTPrescaleProvider prevHLTConfig_; 
  HLTPrescaleProvider newHLTConfig_; 
  

public:
  explicit TrigResultsComparer(const edm::ParameterSet& iPara);
  ~TrigResultsComparer(){}
  TrigResultsComparer(const TrigResultsComparer& rhs)=delete;
  TrigResultsComparer& operator=(const TrigResultsComparer& rhs)=delete;

 private:
  virtual void beginJob(){}
  virtual void beginRun(const edm::Run& run,const edm::EventSetup& iSetup);
  virtual void analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup);
  virtual void endJob();
  
  static int getTrigCode(const TrigResultsComparer::TrigCompData& data,const edm::TriggerResults& trigResults,const edm::TriggerNames& trigNames,const HLTConfigProvider& hltConfig);

};

#endif
