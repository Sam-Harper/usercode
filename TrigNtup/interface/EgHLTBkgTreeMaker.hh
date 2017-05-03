#ifndef ANATREES_EGHLTBKGTREEMAKER_HH
#define ANATREES_EGHLTBKGTREEMAKER_HH


#include "SHarper/SHNtupliser/interface/SHEvent.hh"
#include "SHarper/TrigNtup/interface/EgHLTBkgTreeStruct.hh"


class EgHLTBkgTreeMaker {
 
  TTree* tree_; //root owns this

  EgHLTBkgTreeStruct data_;

  float minEtCut_;
 
public:

  EgHLTBkgTreeMaker(const char* treeName,const char* treeTitle);
  ~EgHLTBkgTreeMaker(){}
  EgHLTBkgTreeMaker(EgHLTBkgTreeMaker&)=delete;
  EgHLTBkgTreeMaker& operator=(EgHLTBkgTreeMaker&)=delete;

  void setMinEtCut(float cut){minEtCut_=cut;}

  void fill(const SHEvent* event);
  TTree* tree(){return tree_;}
  
  //probably should find a better home for these
  static void matchRecoToTruth(const SHEvent& event,std::vector<const SHElectron*>& eles,const SHElectron* & ele1,const SHElectron* & ele2);
  static int getBestMatch(const std::vector<const SHElectron*>& eles,const TLorentzVector& p4,double& bestDeltaR,int otherEleIndx=-1);
  static const SHElectron* getBestMatchedRecoEle(const SHEvent& event,const SHMCParticle* mcPart);

  
private:

  void fill_(const SHTrigObj& hlt,const SHEvent* event);
  const SHElectron* matchRecoEle_(const float eta,const float phi,const float maxDR,const SHEvent& event);
  const SHMCParticle* matchTruth_(const float eta,const float phi,const float maxDR,const SHEvent& event);
  std::vector<const SHMCParticle*> getMCPartsForMatch_(const SHEvent& event);
};

#endif
