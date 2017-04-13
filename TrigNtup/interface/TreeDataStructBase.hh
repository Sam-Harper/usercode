#ifndef TREEDATASTRUCTBASE
#define TREEDATASTRUCTBASE

//the base "class" of the tree data structs 
//mainly allows easy setting of branch addresses of the tree

#include "SHarper/SHNtupliser/interface/LogErr.hh"

#include "TTree.h"

#include <sstream>

struct TreeDataStructBase {

public:
  TreeDataStructBase();
  virtual ~TreeDataStructBase();

  virtual void createBranches(TTree* tree)=0;
  virtual void setBranchAddresses(TTree* tree)=0;

  template<typename T> static void addBranch(TTree* tree,const std::string& name,T& obj,int bufferSize=3200000){
    tree->Branch(name.c_str(),&obj,obj.contents().c_str(),bufferSize);
  }
  template<typename T,std::size_t N> 
  static void addBranch(TTree* tree,const std::string& name,std::array<T,N>& obj,int bufferSize=3200000){
    std::ostringstream leafName;
    leafName<<name<<"["<<N<<"]"<<leafTypeStr<T>();
    tree->Branch(name.c_str(),obj.data(),leafName.str().c_str(),bufferSize);
  }
  
  template<typename T> static std::string leafTypeStr(){LogErr<<" type not recognised, abort"<<std::endl;exit(0);return "";}
 
  void copyTree(TTree* oldTree,TTree* newTree); //assumes that both trees are made 
};

template<> void TreeDataStructBase::addBranch<int>(TTree* tree,const std::string& name,int& obj,int bufferSize);
template<> void TreeDataStructBase::addBranch<float>(TTree* tree,const std::string& name,float& obj,int bufferSize);

template<> std::string TreeDataStructBase::leafTypeStr<float>();
template<> std::string TreeDataStructBase::leafTypeStr<int>();
template<> std::string TreeDataStructBase::leafTypeStr<double>();
template<> std::string TreeDataStructBase::leafTypeStr<unsigned int>();
#endif
