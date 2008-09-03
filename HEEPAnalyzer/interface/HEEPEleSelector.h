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

//this class works out which cuts the electron passes/fails


#include "SHarper/HEEPAnalyzer/interface/HEEPCutValues.h"

#include <vector>
#include <iostream>

namespace heep { 

  class Ele;

  class EleSelector  {
    
  private:
    
    std::vector<heep::CutValues> cutValues_;
    
  public:
    EleSelector();  
    EleSelector(const EleSelector& rhs):cutValues_(rhs.cutValues_){}
    ~EleSelector(){} 
    

    bool passCuts(const heep::Ele& ele,int cutMask=~0x0)const{return getCutCode(ele,cutMask)==0x0;}
    int getCutCode(const heep::Ele& ele,int cutMask=~0x0)const;
    
    static int getCutCode(const heep::Ele& ele,const CutValues& cuts,int cutMask=~0x0);
    
    void addCuts(const CutValues& cuts){cutValues_.push_back(cuts);}
    heep::CutValues* getCuts(int type); //gets the cuts appropriate to the type of the electron
    const heep::CutValues* getCuts(int type)const;
    heep::CutValues* getCutsByIndx(int cutNr){return &cutValues_[cutNr];} 
    const heep::CutValues* getCutsByIndx(int cutNr)const{return &cutValues_[cutNr];}
    int nrCuts()const{return cutValues_.size();}
    
    void setHighNrgy();
    void setPreSel();
    void setPreSelWithEp();
    void setCutMask(int cutMask,int eleType=~0x0);
    void removeCuts(int cutCode,int eleType=~0x0);
    void setMinEt(float minEt,int eleType=~0x0);
    
    void clearCuts(){cutValues_.clear();}
    
  };
}

#endif
