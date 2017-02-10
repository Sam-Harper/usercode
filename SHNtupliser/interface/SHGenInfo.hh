#ifndef SHEVENT_SHGENINFO_HH
#define SHEVENT_SHGENINFO_HH

#include "SHarper/SHNtupliser/interface/SHMCParticle.hh"
#include "SHarper/SHNtupliser/interface/SHPDFInfo.hh"

#include <vector>

class SHGenInfo {
private:
  std::vector<SHMCParticle> mcParticles_; //they are sorted in order according to their index in the gen event info
  std::vector<SHMCParticle> lheParticles_;

  SHPDFInfo pdfInfo_;
  float weight_; //the generator weight (ie for madgraph)
  std::vector<std::pair<int,float> > extraWeights_; //pdf/scale weights
  
  mutable std::vector<size_t> mcPartIndxTbl_; //for fast lookup, probably unnecessary but maps particle position to position in mcParticle index

public:
  enum class PartStatus{INITIAL,PREFSR,FINAL};
  

  SHGenInfo():weight_(0){}
  virtual ~SHGenInfo(){}

  SHGenInfo(const SHGenInfo& rhs)=delete;
  SHGenInfo(SHGenInfo&& rhs)=delete;
  SHGenInfo& operator=(const SHGenInfo& rhs)=delete;

  void addMCParticle(const SHMCParticle& mcPart){mcParticles_.push_back(mcPart);}
  void addLHEParticle(const SHMCParticle& mcPart){lheParticles_.push_back(mcPart);}
  void addPDFInfo(const SHPDFInfo& pdfInfo){pdfInfo_=pdfInfo;}
  void setWeight(float iWeight){weight_=iWeight;}
  void setExtraWeights(std::vector<std::pair<int,float> >&& val){extraWeights_=val;}
  void setExtraWeights(const std::vector<std::pair<int,float> >& val){extraWeights_=val;}

  const std::vector<SHMCParticle>& mcParts()const{return mcParticles_;}
  const SHMCParticle* getPart(size_t index)const; //note this is via index in event, not vector index
  const std::vector<const SHMCParticle*> getDaughters(size_t index)const;
  const std::vector<const SHMCParticle*> getMothers(size_t index)const;
  const std::vector<const SHMCParticle*> getFirstDaughters(size_t index)const;//gets the first daughters that is not the particle itself
  const std::vector<const SHMCParticle*> getFinalDaughters(size_t index)const;//gets all the daughters of the particle that have no daughters themselves
  const std::vector<const SHMCParticle*> getFSRDaughters(size_t index)const;//gets all the daughters that are just FSR
  
  //there was some debate on whether thse two functions should live in MC tools or not
  //decided that as they were selfcontained (ie solely dependent on GenInfo info) and general
  //that they should live here
  //gets the hard process particle (original particle has a status of 20-29)
  const SHMCParticle* getHardProcessPart(int pid,PartStatus status,size_t requiredPartNr=0)const;
  const SHMCParticle* getHardProcessPart(const std::vector<int>& pids,PartStatus status,size_t requiredPartNr=0)const;

  const SHMCParticle* getLastCopyPreFSR(size_t index)const;
  const SHMCParticle* getLastCopy(size_t index)const;
  

  const std::vector<SHMCParticle>& lheParts()const{return lheParticles_;}
  const SHPDFInfo& pdfInfo()const{return pdfInfo_;}
  float weight()const{return weight_;}
  std::vector<std::pair<int,float> > extraWeights()const{return extraWeights_;}


  float lheHT()const;
  float lheBosonPt()const;
  float lheDiLeptonMass()const;
  TLorentzVector lheDiLeptonP4()const;
  void printMCParts(size_t nrLines=std::numeric_limits<size_t>::max())const;
  void printLHEParts(size_t nrLines=std::numeric_limits<size_t>::max())const;
  


  void clear(){mcParticles_.clear();lheParticles_.clear();}
  void flushIndxTable()const{mcPartIndxTbl_.clear();}

private:
  const SHMCParticle* getHardProcessPart_(const std::vector<int>& pids,size_t requiredPartNr)const;
  bool isFromPromptBoson_(size_t index)const;
  void getFinalDaughters_(size_t index,std::vector<const SHMCParticle*>& finalDaughters)const;
  void getFSRDaughters_(size_t index,std::vector<const SHMCParticle*>& fsrDaughters)const;
  void createMCPartIndxTbl_()const;

  ClassDef(SHGenInfo,5)
};


#endif
