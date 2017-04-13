#include "SHarper/TrigNtup/interface/TreeDataStructBase.hh"

TreeDataStructBase::TreeDataStructBase()
{

}

TreeDataStructBase::~TreeDataStructBase()
{

}

void TreeDataStructBase::copyTree(TTree* oldTree,TTree* newTree)
{
  setBranchAddresses(oldTree);
  setBranchAddresses(newTree);
  int nrEntries = oldTree->GetEntries();
  for(int entryNr=0;entryNr<nrEntries;entryNr++){
    oldTree->GetEntry(entryNr);
    newTree->Fill();
  }
}

template<> void TreeDataStructBase::addBranch<int>(TTree* tree,const std::string& name,int& obj,int bufferSize)
{
  tree->Branch(name.c_str(),&obj,(name+leafTypeStr<int>()).c_str(),bufferSize);
}

template<> void TreeDataStructBase::addBranch<float>(TTree* tree,const std::string& name,float& obj,int bufferSize)
{
  tree->Branch(name.c_str(),&obj,(name+leafTypeStr<float>()).c_str(),bufferSize);
}

template<> std::string TreeDataStructBase::leafTypeStr<float>(){return "/F";}
template<> std::string TreeDataStructBase::leafTypeStr<int>(){return "/I";}
template<> std::string TreeDataStructBase::leafTypeStr<double>(){return "/D";}
template<> std::string TreeDataStructBase::leafTypeStr<unsigned int>(){return "/i";}
