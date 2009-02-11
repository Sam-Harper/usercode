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



#include "SHarper/HEEPAnalyzer/interface/HEEPCutValues.h"


#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include <vector>
#include <iostream>

namespace pat {
  class Electron;
}

namespace heep { 

  class Ele;

  class EleSelector  {
    
  private: 
    std::vector<EleCutValues> cutValues_;
    
  public:
    EleSelector(){}//default, it doesnt to anything
    explicit EleSelector(const edm::ParameterSet& config){setup(config);}
    EleSelector(const EleSelector& rhs):cutValues_(rhs.cutValues_){}
    ~EleSelector(){} 
    
    EleSelector& operator=(const EleSelector& rhs){cutValues_=rhs.cutValues_;return *this;} //no owned resources, so self assignment "okay"
      
    void setup(const edm::ParameterSet&);  

    bool passCuts(const heep::Ele& ele,int cutMask=~0x0)const{return getCutCode(ele,cutMask)==0x0;}
    int getCutCode(const heep::Ele& ele,int cutMask=~0x0)const;
    static int getCutCode(const heep::Ele& ele,const EleCutValues& cuts,int cutMask=~0x0);
    
    bool passCuts(const pat::Electron& ele,int cutMask=~0x0)const{return getCutCode(ele,cutMask)==0x0;}
    int getCutCode(const pat::Electron& ele,int cutMask=~0x0)const;
    static int getCutCode(const pat::Electron& ele,const EleCutValues& cuts,int cutMask=~0x0);


    EleCutValues* getCuts(int type); //gets the cuts appropriate to the type of the electron
    const EleCutValues* getCuts(int type)const;
    int nrCuts()const{return cutValues_.size();}
  
  
  private:
    void addCuts(const EleCutValues& cuts){cutValues_.push_back(cuts);}
  };
}

#endif
