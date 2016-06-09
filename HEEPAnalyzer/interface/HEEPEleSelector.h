#ifndef SHARPER_HEEPANALYZER_HEEPELESELECTOR
#define SHARPER_HEEPANALYZER_HEEPELESELECTOR

//class: heep::EleSelector
//Description: an class which can use used to select HEEP electrons
//
//Implimentation: 
//    it can either return  true/false bool on either the electron passed
//    or more usefully a 'cutCode' whose bits tell if the cut failed or not
//    note: bit=1 means the cut was failed
//    The bits each cut corresponds to is defined in HEEPCutCodes
//    It can be either used with a heep or pat electron (and the two *should* give identical results)
//
//    in 3X, I dropped the ability to have multiple cuts depending on classification for barrel and endcap electrons
//    it wasnt being used and this makes the code much easier to understand
//
//    for 80X, I put all the cut logic into HEEPCutValues and introduced GsfEleExtra 
//    as the number of things needed to be passed in was getting large


#include "SHarper/HEEPAnalyzer/interface/HEEPCutValues.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"



#include <iostream>

namespace reco {
  class GsfElectron;
}

namespace heep { 

  class Ele;
  class GsfEleExtra;
  class EleSelector  {
    
  private: 
    EleCutValues barrelCutValues_;
    EleCutValues endcapCutValues_;

  public:
    EleSelector(){}//default, it doesnt to anything
    explicit EleSelector(const edm::ParameterSet& config){setup(config);} 
    ~EleSelector(){} 
  
    void setup(const edm::ParameterSet&);  

    bool passCuts(const heep::Ele& ele,const int cutMask=~0x0)const{return getCutCode(ele,cutMask)==0x0;}
    int getCutCode(const heep::Ele& ele,const int cutMask=~0x0)const;

    //note: you can pass a pat::Electron into these functions as it inherits from GsfElectron
    bool passCuts(const reco::GsfElectron& ele,const heep::GsfEleExtra& eleExtra,const int cutMask=~0x0)const{return getCutCode(ele,eleExtra,cutMask)==0x0;}  
    int getCutCode(const reco::GsfElectron& ele,const heep::GsfEleExtra& eleExtra,const int cutMask=~0x0)const;
    static int getCutCode(const reco::GsfElectron& ele,const heep::GsfEleExtra& eleExtra,const EleCutValues& cuts,const int cutMask=~0x0);

    EleCutValues* getBarrelCuts(){return &barrelCutValues_;}
    const EleCutValues* getBarrelCuts()const{return &barrelCutValues_;}
    EleCutValues* getEndcapCuts(){return &endcapCutValues_;}
    const EleCutValues* getEndcapCuts()const{return &endcapCutValues_;} 
    EleCutValues* getCuts(int type){return type<100 ? getBarrelCuts() : getEndcapCuts();}
    const EleCutValues* getCuts(int type)const{return type<100 ? getBarrelCuts() : getEndcapCuts();}

  
  };
}

#endif
