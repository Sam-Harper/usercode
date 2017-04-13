#ifndef ANATREES_ELEDATATRUTHTREEMAKER_HH
#define ANATREES_ELEDATATRUTHTREEMAKER_HH


#include "SHarper/SHNtupliser/interface/SHEvent.hh"
#include "SHarper/TrigNtup/interface/EleDataTruthTreeStruct.hh"


class EleDataTruthTreeMaker {
 
  TTree* tree_; //root owns this

  EleDataTruthTreeStruct data_;
 
  float minEtCut_;
 
public:

  EleDataTruthTreeMaker(const char* treeName,const char* treeTitle);
  ~EleDataTruthTreeMaker(){}
  EleDataTruthTreeMaker(EleDataTruthTreeStruct&)=delete;
  EleDataTruthTreeMaker& operator=(EleDataTruthTreeStruct&)=delete;

  void setMinEtCut(float cut){minEtCut_=cut;}

  void fill(const SHEvent* event);
  TTree* tree(){return tree_;}
  
  //probably should find a better home for these
  static void matchRecoToTruth(const SHEvent& event,std::vector<const SHElectron*>& eles,const SHElectron* & ele1,const SHElectron* & ele2);
  static int getBestMatch(const std::vector<const SHElectron*>& eles,const TLorentzVector& p4,double& bestDeltaR,int otherEleIndx=-1);
  static const SHElectron* getBestMatchedRecoEle(const SHEvent& event,const SHMCParticle* mcPart);

  
private:
  void fill_(const SHEvent* event,const SHMCParticle& truthEle,const SHElectron* recoEle);
};

#endif
