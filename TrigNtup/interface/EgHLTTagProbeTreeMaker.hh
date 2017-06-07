#ifndef ANATREES_EGHLTTAGPROBETREEMAKER_HH
#define ANATREES_EGHLTTAGPROBETREEMAKER_HH


#include "SHarper/SHNtupliser/interface/SHEvent.hh"
#include "SHarper/TrigNtup/interface/EgHLTTagProbeTreeStruct.hh"


class EgHLTTagProbeTreeMaker {
 
  TTree* tree_; //root owns this

  EgHLTTagProbeTreeStruct data_;

  float minEtCut_;
  std::vector<std::string> tagFilters_;
  std::vector<std::string> probeFilters_;
 
public:

  EgHLTTagProbeTreeMaker(const char* treeName,const char* treeTitle,
			 const std::vector<std::string>& tagFilters,const std::vector<std::string>& probeFilters);
  ~EgHLTTagProbeTreeMaker(){}
  EgHLTTagProbeTreeMaker(EgHLTTagProbeTreeMaker&)=delete;
  EgHLTTagProbeTreeMaker& operator=(EgHLTTagProbeTreeMaker&)=delete;

  void setMinEtCut(float cut){minEtCut_=cut;}

  void fill(const SHEvent* event);
  TTree* tree(){return tree_;}
  
  //probably should find a better home for these
  static void matchRecoToTruth(const SHEvent& event,std::vector<const SHElectron*>& eles,const SHElectron* & ele1,const SHElectron* & ele2);
  static int getBestMatch(const std::vector<const SHElectron*>& eles,const TLorentzVector& p4,double& bestDeltaR,int otherEleIndx=-1);
  static const SHElectron* getBestMatchedRecoEle(const SHEvent& event,const SHMCParticle* mcPart);

  
private:

  void fill_(const SHTrigObj& tag,const SHTrigObj& probe,const SHEvent* event);
  const SHElectron* matchRecoEle_(const float eta,const float phi,const float maxDR,const SHEvent& event);
  const SHMCParticle* matchTruth_(const float eta,const float phi,const float maxDR,const SHEvent& event);
  std::vector<const SHMCParticle*> getMCPartsForMatch_(const SHEvent& event);
};

#endif
