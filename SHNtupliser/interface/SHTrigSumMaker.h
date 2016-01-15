#ifndef SHARPER_SHNTUPLISER_SHTRIGSUMMAKER_H
#define SHARPER_SHNTUPLISER_SHTRIGSUMMAKER_H

#include <string>

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
  

};
  

#endif
