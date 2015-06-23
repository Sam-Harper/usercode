#include "SHarper/SHNtupliser/interface/SHGenInfo.hh"

#include "SHarper/SHNtupliser/interface/LogErr.hh"

#include <iomanip>

ClassImp(SHGenInfo)

const SHMCParticle* SHGenInfo::getPart(size_t index)const
{
  if(mcPartIndxTbl_.empty()) createMCPartIndxTbl_();
  if(index<mcPartIndxTbl_.size()){
    size_t vecIndx = mcPartIndxTbl_[index];
    if(vecIndx<mcParticles_.size()) return &mcParticles_[vecIndx];
  }
  return nullptr;
}


const std::vector<const SHMCParticle*> SHGenInfo::getDaughters(size_t index)const
{
  std::vector<const SHMCParticle*> daughters;
  const SHMCParticle* mcPart = getPart(index);
  size_t da1 = mcPart->jda1()>=0 ? mcPart->jda1() : std::numeric_limits<size_t>::max();
  size_t da2 = mcPart->jda2()>=0 ? mcPart->jda2() : std::numeric_limits<size_t>::max();
  for(size_t daNr=da1;daNr<=da2;daNr++){
    const SHMCParticle* daughter = getPart(daNr);
    if(daughter) daughters.push_back(daughter);
  }
  return daughters;

}

const std::vector<const SHMCParticle*> SHGenInfo::getMothers(size_t index)const
{
  std::vector<const SHMCParticle*> mothers;
  const SHMCParticle* mcPart = getPart(index);
  size_t mo1 = mcPart->jmo1()>=0 ? mcPart->jmo1() : std::numeric_limits<size_t>::max();
  size_t mo2 = mcPart->jmo2()>=0 ? mcPart->jmo2() : std::numeric_limits<size_t>::max();
  for(size_t moNr=mo1;moNr<=mo2;moNr++){
    const SHMCParticle* mother = getPart(moNr);
    if(mother) mothers.push_back(mother);
  }
  return mothers;

}


const std::vector<const SHMCParticle*> SHGenInfo::getFirstDaughters(size_t index)const
{
  const SHMCParticle* part = getPart(index);
  auto daughters = getDaughters(index);
  if(daughters.size()==1){
    if(daughters[0]->pid()==part->pid()){
      return getFirstDaughters(daughters[0]->index());
    }
  }
  return daughters; 
}

const std::vector<const SHMCParticle*> SHGenInfo::getFinalDaughters(size_t index)const
{
  std::vector<const SHMCParticle*> finalDaughters;
  getFinalDaughters_(index,finalDaughters);
  return finalDaughters; 
}

void SHGenInfo::printMCParts(size_t nrLines)const
{
  if(!mcParticles_.empty()){
    std::cout << std::setw(2)<<" "<< std::setw(6) << "indx" << std::setw(8) << "stdhep" << std::setw(6) << "idhep" 
	      << std::setw(6) << "jmo1" << std::setw(6) << "jmo2" << std::setw(6) << "jda1" << std::setw(6) << "jda2" 
	      << std::setw(6) << "Px" << std::setw(10) << "Py" << std::setw(10) << "Pz" << std::setw(10) <<"E" 
	      << std::setw(10) << "Pt" <<std::setw(15)<< "Mass" << "\n";
    for(size_t i=0;i<mcParticles_.size() && i<nrLines;i++){
      const SHMCParticle& mcPart = mcParticles_[i];
      std::cout << mcPart <<std::endl;
    }
  }
}



void SHGenInfo::getFinalDaughters_(size_t index,std::vector<const SHMCParticle*>& finalDaughters)const
{
  auto daughters = getDaughters(index);
  for(auto daughter : daughters){
    if(daughter->jda1()==-1) finalDaughters.push_back(daughter);
    else getFinalDaughters_(daughter->index(),finalDaughters);
  }
  
}

void SHGenInfo::createMCPartIndxTbl_()const
{
  int maxIndex = -1;
  if(!mcParticles_.empty()) maxIndex = mcParticles_.back().index();
  
  std::vector<size_t> tmpIndx(maxIndex,std::numeric_limits<size_t>::max());
  mcPartIndxTbl_.swap(tmpIndx);
  
  for(size_t vecIndx=0;vecIndx<mcParticles_.size();vecIndx++){
    const SHMCParticle& mcPart = mcParticles_[vecIndx];
    if(static_cast<size_t>(mcPart.index())<mcPartIndxTbl_.size()){
      mcPartIndxTbl_[mcPart.index()]=vecIndx;
    }else{
      LogErr<<" Error, "<<mcPart.index()<<" is bigger than index table "<<mcPartIndxTbl_.size()<<" exiting "<<std::endl;
      exit(0);
    }
  }
  
}
