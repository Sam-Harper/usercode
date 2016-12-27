#ifndef SHARPER_SHNTUPLISER_SHTRIGSUMMAKER_H
#define SHARPER_SHNTUPLISER_SHTRIGSUMMAKER_H

#include <string>
#include <vector>

#include "DataFormats/RecoCandidate/interface/RecoEcalCandidateFwd.h"
#include "DataFormats/RecoCandidate/interface/RecoEcalCandidate.h"
#include "DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h"
#include "FWCore/Framework/interface/GenericHandle.h"

#include "SHarper/SHNtupliser/interface/TrigMenuMgr.hh"

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
  
  
  const SHL1Menu* l1Menu_;
  const SHHLTMenu* hltMenu_;

  //changes on a lumi basis
  int currPSColumn_;
  
  //our info to track if cache is out of date
  RunLumi lastRunLumi_;

  //menu information
  TrigMenuMgr menuMgr_;  
  
  //debug information
  int verboseLvl_;

  static constexpr size_t kMaxNrL1SeedsToStore_=20;
  static constexpr size_t kNrL1Seeds_=512;//this is hardcoded everywhere in the L1 code, its a bad idea but even when there are 256 seeds, it still things theres 512 so we do it this way for now
public:
  SHTrigSumMaker():l1Menu_(nullptr),hltMenu_(nullptr),verboseLvl_(0){}
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
  void makeSHTrigSum(std::vector<pat::TriggerObjectStandAlone>& trigObjs,
		     const edm::TriggerResults& trigResults,
		     const edm::TriggerNames& trigNames,
		     HLTPrescaleProvider& hltPSProv, 
		     const edm::Event& edmEvent,
		     const edm::EventSetup& edmEventSetup,
		     SHTrigSummary& shTrigSum);
 
  static int convertToSHTrigType(int cmsswTrigType);
  static int convertToSHTrigType(const pat::TriggerObject& trigObj);
  static std::string rmTrigVersionFromName(std::string trigname);
  static bool isUnseededEG(const trigger::TriggerEvent& trigEvt,size_t trigObjNr);
  static bool isUnseededEG(const std::string& collName){
    return collName.compare(0,27,"hltEgammaCandidatesUnseeded")==0;
  }

  static void associateEgHLTDebug(const heep::Event& heepEvent,SHTrigSummary& shTrigSum);
  static void associateEgHLTDebug(const edm::Event& edmEvent,const edm::Handle<std::vector<reco::RecoEcalCandidate>>& ecalCands,SHTrigSummary& shTrigSum);
  static void associateEgHLTDebug(const edm::Event& edmEvent,const reco::RecoEcalCandidateRef& ecalCand,const std::vector<SHTrigObj*> trigObjs);
  
private: 
  void makeSHTrigSumNoFilters_(const edm::TriggerResults& trigResults,
			       const edm::TriggerNames& trigNames,
			       HLTPrescaleProvider& hltPSProv, 
			       const edm::Event& edmEvent,
			       const edm::EventSetup& edmEventSetup,
			       SHTrigSummary& shTrigSum);
  void fillMenu_(SHTrigSummary& shTrigSum)const;
  TBits getL1Result_(const l1t::L1TGlobalUtil& l1UtilsConst)const;
  TBits getHLTResult_(const edm::TriggerResults& trigResults)const;			  
  void fillSHTrigObjs_(const trigger::TriggerEvent& trigEvt,SHTrigSummary& shTrigSum)const;
  void fillSHTrigObjs_(const std::vector<pat::TriggerObjectStandAlone>& trigObjs,
		       SHTrigSummary& shTrigSum)const;
  
  std::vector<std::string> getL1Seeds_(const HLTConfigProvider& hltConfig,const size_t pathNr,
				       const std::string& pathName)const;
  
  void updateCacheRunChange_(const edm::Event& edmEvent,
			     const edm::EventSetup& edmEventSetup, 
			     HLTPrescaleProvider& hltPSProv);
  void updateCacheLumiChange_(const edm::Event& edmEvent,
			      const edm::EventSetup& edmEventSetup, 
			      HLTPrescaleProvider& hltPSProv);

  const SHL1Menu& getL1Menu_(const HLTConfigProvider& hltConfig,const l1t::L1TGlobalUtil& l1GtUtils,
			     const std::string& l1MenuName);
  const SHL1Menu& getL1Menu_(const edm::EventSetup& edmEventSetup,
			     const std::string& l1MenuName);
  const SHHLTMenu& getHLTMenu_(const HLTConfigProvider& hltConfig);
  
  void addL1Menu_(const HLTConfigProvider& hltConfig,const l1t::L1TGlobalUtil& l1GtUtils,
		  const std::string& l1MenuName);
  void addL1Menu_(const edm::EventSetup& edmEventSetup,
		  const std::string& l1MenuName);
    
  void addHLTMenu_(const HLTConfigProvider& hltConfig);
	
  

  static std::vector<std::string> splitL1SeedExpr_(const std::string& l1SeedExpr);
  //converts vector[columnNr][bitNr] -> vector[columnNr] for a given bitNr 
  static std::vector<unsigned int> getSeedPreScales(size_t bitNr,const std::vector<std::vector<int> >& psTbl);

};
  

#endif
