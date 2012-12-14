#ifndef SHPILEUPSUMMARY
#define SHPILEUPSUMMARY

#include "SHarper/SHNtupliser/interface/SHPileUpInfo.hh"

#include "TObject.h"
#include "TClonesArray.h"
#include <vector>


//okay I hate the TClonesArray design but meh, its consistant with the rest of what goes in SHEvent

class SHPileUpSummary  {
private:

  TClonesArray puInfos_;

public:
  
  SHPileUpSummary();
  SHPileUpSummary(const SHPileUpSummary& rhs);
  virtual ~SHPileUpSummary(){}
  
  SHPileUpSummary& operator=(const SHPileUpSummary& rhs);

  void addPUInfo(int iBx,int iNrInt,int iNrTrueInt);
  void clear(){puInfos_.Delete();}

  const SHPileUpInfo* getPUInfo(int puNr)const{return static_cast<SHPileUpInfo*>(puInfos_[puNr]);}

  unsigned nrPUInfos()const{return puInfos_.GetLast()+1;}
  int nrInteractionsByBx(int bx)const;
  int nrTrueInteractions()const{return getPUInfo(1) && getPUInfo(1)->bx()==0 ? getPUInfo(1)->nrTrueInteractions() : 0;}

  ClassDef(SHPileUpSummary,1)


};

#endif
