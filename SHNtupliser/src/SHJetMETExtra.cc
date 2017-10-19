#include "SHarper/SHNtupliser/interface/SHJetMETExtra.hh"

ClassImp(SHJetMETExtra)

namespace {
  size_t getIndex(const std::string& name,const std::vector<std::string>& coll)
  {
    for(size_t index = 0;index<coll.size();index++){
      if(name==coll[index]) return index;
    }
    return coll.size();
  }
}

void SHJetMETExtra::clear()
{
  jetColls_.clear();
  mets_.clear();
  jetCollNames_.clear();
  metNames_.clear();
}

const std::vector<SHJet> & SHJetMETExtra::jets(const std::string& collName)const
{
  auto index = getIndex(collName,jetCollNames_);
  if(index<jetColls_.size()) return jetColls_[index];
  else return dummyJets;
}

const SHMet& SHJetMETExtra::met(const std::string& name)const
{
  auto index = getIndex(name,metNames_);
  if(index<mets_.size()) return mets_[index];
  else return dummyMET;
}
