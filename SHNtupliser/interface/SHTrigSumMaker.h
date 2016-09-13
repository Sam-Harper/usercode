#ifndef SHARPER_SHNTUPLISER_SHTRIGSUMMAKER_H
#define SHARPER_SHNTUPLISER_SHTRIGSUMMAKER_H

#include <string>
#include <vector>

#include "DataFormats/RecoCandidate/interface/RecoEcalCandidateFwd.h"
#include "DataFormats/RecoCandidate/interface/RecoEcalCandidate.h"
#include "FWCore/Framework/interface/GenericHandle.h"

#include "SHarper/SHNtupliser/interface/TrigMenuDataMgr.hh"

namespace edm{
  class Event;
  class EventSetup;
  class TriggerNames;
  class TriggerResults;
}
namespace trigger{
  class TriggerEvent;
}
namespace heep{
  class Event;
}
namespace l1t{
  class L1TGlobalUtil;
}
class HLTConfigProvider;
class HLTPrescaleProvider;

class SHTrigSummary;
class SHTrigObj;
//known bugs:
//L1 pass info appears not to be correctly filled and its a bit of a mystery why
//as l1GtUtils and L1GlobalTriggerReadoutRecord appear to agree wrongly


class SHTrigSumMaker {
private:

public:
  struct P4Struct{
    float pt,eta,phi,mass;
    P4Struct(float iPt,float iEta,float iPhi,float iMass):pt(iPt),eta(iEta),phi(iPhi),mass(iMass){}
  };
  class RunLumi{
  private:
    uint runnr_;
    uint lumiSec_;
  public:
    RunLumi():runnr_(0),lumiSec_(0){}
    RunLumi(uint iRunnr,uint iLumiSec):runnr_(iRunnr),lumiSec_(iLumiSec){}
    bool sameLumi(RunLumi rhs)const{return runnr_==rhs.runnr_ && lumiSec_==rhs.lumiSec_;}
    bool sameRun(RunLumi rhs)const{return runnr_==rhs.runnr_;}  
  };
  
  class PathL1Seeds {
    std::string name_;
    std::vector<size_t> seeds_;
  public:
    PathL1Seeds(std::string iName,std::vector<size_t> iSeeds):
      name_(std::move(iName)),seeds_(std::move(iSeeds)){} 

    bool operator==(const PathL1Seeds& rhs)const{return name_==rhs.name_;}
    bool operator==(const std::string& rhs)const{return name_==rhs;}
    template<typename T> bool operator!=(const T& rhs)const{return !(*this==rhs);}
    const std::vector<size_t>& seeds()const{return seeds_;}
  };
  
  
  //our cached data (mainly L1 stuff as its really slow)
  //changes on a menu basis
  std::vector<PathL1Seeds> pathsL1Seeds_;
  
  //changes on a run basis
  std::vector<std::vector<int> > l1PreScaleTbl_; //currently not filled due to L1 software issues
  //changes on a lumi basis
  int currPSColumn_;
  std::vector<int> l1Prescales_; //temporary for L1 software issues
  
  //our info to track if cache is out of date
  RunLumi lastRunLumi_;
  std::pair<std::string,std::string> menuAndProcName_;

  //menu information
  TrigMenuDataMgr menuMgr_;  
  
  //debug information
  int verboseLvl_;

  static const size_t kMaxNrL1SeedsToStore_=20;
public:
  SHTrigSumMaker():verboseLvl_(0){}
  ~SHTrigSumMaker(){}
  
  void makeSHTrigSum(const heep::Event& heepEvent,SHTrigSummary& shTrigSum);
  
  //having to pass in l1decision as gt utilis was giving me odd results.
  void makeSHTrigSum(const trigger::TriggerEvent& trigEvt,
		     const edm::TriggerResults& trigResults,
		     const edm::TriggerNames& trigNames,
		     HLTPrescaleProvider& hltPSProv,
		     const edm::Event& edmEvent,
		     const edm::EventSetup& edmEventSetup,
		     SHTrigSummary& shTrigSum);
  
  
  static int convertToSHTrigType(int cmsswTrigType);
  static std::string rmTrigVersionFromName(std::string trigname);

  static void associateEgHLTDebug(const heep::Event& heepEvent,SHTrigSummary& shTrigSum);
  static void associateEgHLTDebug(const edm::Event& edmEvent,const edm::Handle<std::vector<reco::RecoEcalCandidate>>& ecalCands,SHTrigSummary& shTrigSum);
  static void associateEgHLTDebug(const edm::Event& edmEvent,const reco::RecoEcalCandidateRef& ecalCand,const std::vector<SHTrigObj*> trigObjs);
  
private:
  void fillMenu_(SHTrigSummary& shTrigSum,const HLTConfigProvider& hltConfig,
		 l1t::L1TGlobalUtil& l1GtUtils);
  const SHTrigMenuData& getMenuData_(const HLTConfigProvider& hltConfig,
				 l1t::L1TGlobalUtil& l1GtUtils);
  void addMenuData_(const HLTConfigProvider& hltConfig,
		    l1t::L1TGlobalUtil& l1GtUtils);
    
  void fillSHTrigResults_(const edm::TriggerResults& trigResults,
			  const edm::TriggerNames& trigNames,
			  HLTPrescaleProvider& hltPSProv,
			  SHTrigSummary& shTrigSum)const;

  void fillSHTrigObjs_(const trigger::TriggerEvent& trigEvt,SHTrigSummary& shTrigSum)const;

  static void fillSHL1Results_(l1t::L1TGlobalUtil& l1Util,const edm::Event& edmEvent,
			       SHTrigSummary& shTrigSum);  

  
  std::vector<std::pair<size_t,int>> 
  getPathL1Prescales_(const std::string& pathName)const;
  
  std::vector<size_t> getL1Seeds_(const std::string& pathName,
				  const std::vector<std::string>& l1Names,
				  const HLTConfigProvider& hltConfig)const;
    

  void updateCacheMenuChange_(const edm::Event& edmEvent,
			      const edm::EventSetup& edmEventSetup, 			   
			      HLTPrescaleProvider& hltPSProv);
  void updateCacheRunChange_(const edm::Event& edmEvent,
			     const edm::EventSetup& edmEventSetup, 
			     HLTPrescaleProvider& hltPSProv);
  void updateCacheLumiChange_(const edm::Event& edmEvent,
			      const edm::EventSetup& edmEventSetup, 
			      HLTPrescaleProvider& hltPSProv);

  static std::vector<std::string> splitL1SeedExpr_(const std::string& l1SeedExpr);
};
  

#endif
