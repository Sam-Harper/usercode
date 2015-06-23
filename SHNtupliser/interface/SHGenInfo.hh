#ifndef SHEVENT_SHGENINFO_HH
#define SHEVENT_SHGENINFO_HH

#include "SHarper/SHNtupliser/interface/SHMCParticle.hh"

#include <vector>

class SHGenInfo {
private:
  std::vector<SHMCParticle> mcParticles_; //they are sorted in order according to their index in the gen event info
  std::vector<SHMCParticle> lheParticles_;

  //might add in some PDFInfo as well
  
  mutable std::vector<size_t> mcPartIndxTbl_; //for fast lookup, probably unnecessary but maps particle position to position in mcParticle index

public:
  SHGenInfo(){}
  virtual ~SHGenInfo(){}

  SHGenInfo(const SHGenInfo& rhs)=delete;
  SHGenInfo(SHGenInfo&& rhs)=delete;
  SHGenInfo& operator=(const SHGenInfo& rhs)=delete;

  void addMCParticle(const SHMCParticle& mcPart){mcParticles_.push_back(mcPart);}
  void addLHEParticle(const SHMCParticle& mcPart){lheParticles_.push_back(mcPart);}

  const std::vector<SHMCParticle> mcParts()const{return mcParticles_;}
  const SHMCParticle* getPart(size_t index)const; //note this is via index in event, not vector index
  const std::vector<const SHMCParticle*> getDaughters(size_t index)const;
  const std::vector<const SHMCParticle*> getMothers(size_t index)const;
  const std::vector<const SHMCParticle*> getFirstDaughters(size_t index)const;//gets the first daughters that is not the particle itself
  const std::vector<const SHMCParticle*> getFinalDaughters(size_t index)const;//gets all the daughters of the particle that have no daughters themselves
 
  const std::vector<SHMCParticle> lheParts()const{return lheParticles_;}
  
  void printMCParts(size_t nrLines=std::numeric_limits<size_t>::max())const;

  void clear(){mcParticles_.clear();lheParticles_.clear();}
  void flushIndxTable()const{mcPartIndxTbl_.clear();}

private:
  void getFinalDaughters_(size_t index,std::vector<const SHMCParticle*>& finalDaughters)const;

  void createMCPartIndxTbl_()const;

  ClassDef(SHGenInfo,1)
};


#endif
