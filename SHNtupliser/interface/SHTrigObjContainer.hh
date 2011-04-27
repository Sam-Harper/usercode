#ifndef SHTRIGOBJCONTAINER
#define SHTRIGOBJCONTAINER

#include "SHarper/SHNtupliser/interface/SHTrigEcalCand.hh"

#include "TObject.h"

#include <vector>

class SHTrigObjContainer : public TObject {

private:
  std::vector<SHTrigEcalCand> ecalCands_;
public:
  SHTrigObjContainer();
  ~SHTrigObjContainer(){}

  void addEcalCand(const SHTrigEcalCand& ecalCand){ecalCands_.push_back(ecalCand);}
  void clear(){ecalCands_.clear();}
 

  const SHTrigEcalCand* getEcalCand(int candNr)const{return &ecalCands_[candNr];}
  int nrEcalCands()const{return ecalCands_.size();}
  


  ClassDef(SHTrigObjContainer,1)

};

#endif
