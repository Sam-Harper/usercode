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
  size_t da1 = mcPart->jda1()>=0 ? mcPart->jda1() : std::numeric_limits<size_t>::max()-1;
  size_t da2 = mcPart->jda2()>=0 ? mcPart->jda2() : std::numeric_limits<size_t>::max()-1;
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

const std::vector<const SHMCParticle*> SHGenInfo::getFSRDaughters(size_t index)const
{
  std::vector<const SHMCParticle*> fsrDaughters;
  getFSRDaughters_(index,fsrDaughters);
  return fsrDaughters;
}

const SHMCParticle* SHGenInfo::getHardProcessPart(int pid,PartStatus status,size_t requiredPartNr)const
{
  std::vector<int> pids(1,pid);
  return getHardProcessPart(pids,status,requiredPartNr);
}


const SHMCParticle* SHGenInfo::getHardProcessPart(const std::vector<int>& pids,PartStatus status,
						  size_t requiredPartNr)const
{
  const SHMCParticle* part = getHardProcessPart_(pids,requiredPartNr);
  if(part){
    if(status==PartStatus::INITIAL) return part;
    else if(status==PartStatus::PREFSR) return getLastCopyPreFSR(part->index());
    else if(status==PartStatus::FINAL) return getLastCopy(part->index());
    else{
      //LogErr<<" part status "<<status<<" is invalid "<<std::endl;
      LogErr<<" part status is invalid "<<std::endl;
    }

  }
  return nullptr;
}

const SHMCParticle* SHGenInfo::getLastCopyPreFSR(size_t index)const
{
  const SHMCParticle* part = getPart(index);
  auto daughters = getDaughters(index);
  if(daughters.size()==1 && daughters.front()->pid()==part->pid()){
    return getLastCopyPreFSR(daughters.front()->index());
  }
  else return part;

}

const SHMCParticle* SHGenInfo::getLastCopy(size_t index)const
{
  const SHMCParticle* part = getPart(index);
  auto daughters = getDaughters(index);
  for(auto daughter : daughters){
    if(daughter->pid()==part->pid()) return getLastCopy(daughter->index());
  }
  return part;
}

float SHGenInfo::lheHT()const
{
  float ht=0;
  // std::cout <<"start "<<std::endl;
  for(const auto& part : lheParticles_){
    //std::cout <<part<<std::endl;
    if(part.pid()==21 || std::abs(part.pid())<=6){
       if(part.status()==1){
	 ht+=part.p4().Pt();
	 //std::cout <<" pt "<<part.p4().Pt()<<" ht "<<ht<<std::endl;
       }
     }
  }
  //  std::cout <<"ht "<<ht<<std::endl;
  return ht;
}

float SHGenInfo::lheBosonPt()const
{
  //  float pt=0;
  for(const auto& part : lheParticles_){
    if(part.pid()==22 || part.pid()==23 || std::abs(part.pid())==24){
      //if(part.p4().Pt()<-100){
	//std::cout <<"pt of "<<part.p4().Pt()<<std::endl;
	//for(const auto& p : lheParticles_) std::cout <<p<<std::endl;
	//  }
      return part.p4().Pt();
    }
  }
  //std::cout <<"no W "<<std::endl;
  //for(const auto& p : lheParticles_) std::cout <<p<<std::endl;
 
  return -1;
}

float SHGenInfo::lheDiLeptonMass()const
{
  //  float pt=0;
  std::vector<const SHMCParticle*> leptons;
  for(const auto& part : lheParticles_){
    if(std::abs(part.pid())==15 || std::abs(part.pid())==13 || std::abs(part.pid())==11){
      leptons.push_back(&part);
    }
  }
  if(leptons.size()>=2) return (leptons[0]->p4()+leptons[1]->p4()).Mag();
  return -1;
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

void SHGenInfo::printLHEParts(size_t nrLines)const
{
  if(!lheParticles_.empty()){
    std::cout << std::setw(2)<<" "<< std::setw(6) << "indx" << std::setw(8) << "stdhep" << std::setw(6) << "idhep" 
	      << std::setw(6) << "jmo1" << std::setw(6) << "jmo2" << std::setw(6) << "jda1" << std::setw(6) << "jda2" 
	      << std::setw(6) << "Px" << std::setw(10) << "Py" << std::setw(10) << "Pz" << std::setw(10) <<"E" 
	      << std::setw(10) << "Pt" <<std::setw(15)<< "Mass" << "\n";
    for(size_t i=0;i<lheParticles_.size() && i<nrLines;i++){
      const SHMCParticle& mcPart = lheParticles_[i];
      std::cout << mcPart <<std::endl;
    }
  }
}

const SHMCParticle* SHGenInfo::getHardProcessPart_(const std::vector<int>& pids,size_t requiredPartNr)const
{
  size_t partNr=0;
  for(auto& part : mcParts()){
    if( (part.status()>=20 && part.status()<=29) || //incoming 21, intermediate 22, and outgoing 23 particles
	(part.status()<=2 && (part.jmo1()==-1 || isFromPromptBoson_(part.index()))) || //1=final, 2 = final before decay but need to check its from the prompt boson (also take it if theres no mother particle)
	part.status()==3){ //3=pythia6
      
      if(std::find(pids.begin(),pids.end(),part.pid())!=pids.end()){
	if(requiredPartNr==partNr) return &part;
	partNr++;
      }
    }
  }
  return nullptr;
}

bool SHGenInfo::isFromPromptBoson_(size_t index)const
{
  auto mothers = getMothers(index);
  if(mothers.size()==1){
    auto mother = mothers.front();
    if(mother->pid()==22 || mother->pid()==23 || std::abs(mother->pid())==24 ||
       mother->pid()==32 ||
       mother->pid()==5100039){
      if(mother->status()>=20 && mother->status()<=29) return true;
      else return isFromPromptBoson_(mother->index());
    }
  }
  return false;

}

void SHGenInfo::getFinalDaughters_(size_t index,std::vector<const SHMCParticle*>& finalDaughters)const
{
  auto daughters = getDaughters(index);
  for(auto daughter : daughters){
    if(daughter->jda1()==-1) finalDaughters.push_back(daughter);
    else getFinalDaughters_(daughter->index(),finalDaughters);
  }
  
}
void SHGenInfo::getFSRDaughters_(size_t index,std::vector<const SHMCParticle*>& fsrDaughters)const
{
  const SHMCParticle* part = getPart(index);
  auto daughters = getDaughters(index);

  //first little check if its actually FSR, ie it decays to a particle with an identical PID
  //probably not necessary but nice to check
  bool isFSR=false;
  for(auto daughter : daughters){
    if(daughter->pid()==part->pid()) isFSR=true;
  }
  if(isFSR){
    for(auto daughter : daughters){
      if(daughter->pid()==22 || daughter->pid()==21){
	fsrDaughters.push_back(daughter);;
      }
    }
  }

}

void SHGenInfo::createMCPartIndxTbl_()const
{
  int maxIndex = -1;
  if(!mcParticles_.empty()) maxIndex = mcParticles_.back().index();
  
  std::vector<size_t> tmpIndx(maxIndex+1,std::numeric_limits<size_t>::max());
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
