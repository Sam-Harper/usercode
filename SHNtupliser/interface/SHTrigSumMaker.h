#ifndef SHARPER_SHNTUPLISER_SHTRIGSUMMAKER_H
#define SHARPER_SHNTUPLISER_SHTRIGSUMMAKER_H

#include <string>
#include <vector>

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
class HLTConfigProvider;
class SHTrigSummary;

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

public:
  SHTrigSumMaker(){}
  ~SHTrigSumMaker(){}
  
  static void makeSHTrigSum(const heep::Event& heepEvent,SHTrigSummary& shTrigSum);
  
  //having to pass in l1decision as gt utilis was giving me odd results.
  static void makeSHTrigSum(const trigger::TriggerEvent& trigEvt,
			    const edm::TriggerResults& trigResults,
			    const edm::TriggerNames& trigNames,
			    const HLTConfigProvider& hltConfig,
			    const edm::Event& edmEvent,
			    const edm::EventSetup& edmEventSetup,
			    SHTrigSummary& shTrigSum);
  
  
  static int convertToSHTrigType(int cmsswTrigType);
  static std::string rmTrigVersionFromName(std::string trigname);

  
private:
  static void fillMenu_(SHTrigSummary& shTrigSum,const HLTConfigProvider& hltConfig);
  static void fillSHTrigResults_(const edm::TriggerResults& trigResults,
				 const edm::TriggerNames& trigNames,
				 const HLTConfigProvider& hltConfig,
				 const edm::Event& edmEvent,
				 const edm::EventSetup& edmEventSetup,	
				 SHTrigSummary& shTrigSum);

  
  static void fillSHTrigObjs_(const trigger::TriggerEvent& trigEvt,SHTrigSummary& shTrigSum);


  static void fillSHL1Results_(const HLTConfigProvider& hltConfig,const edm::Event& edmEvent,
			       SHTrigSummary& shTrigSum);  

  
  static std::vector<std::pair<size_t,int>> 
  getPathL1Prescales_(const std::string& pathName,
		      const HLTConfigProvider& hltConfig,
		      const edm::Event& edmEvent,
		      const edm::EventSetup& edmEventSetup,
		      SHTrigSummary& shTrigSum);
};
  

#endif
