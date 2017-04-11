#ifndef ELEDATAONLYTRUTHTREESTRUCT
#define ELEDATAONLYTRUTHTREESTRUCT

//a struct to hold the data of an eleDataTree
//the scope of this "class" has expanded to completely define the tree
//and handle all management (such as copying, address setting etc) of it
//however it is still a struct as its primary task is to hold unmanipulated
//data and as such all the data members are public
//in theory I could simply load this in as a class to the tree rather than 
//setting the different branches, it would achieve the same thing but I would have to inherit from TObject as well

#include "SHarper/TrigNtup/interface/TreeDataStructBase.hh"
#include "SHarper/TrigNtup/interface/EleStructs.hh"
#include "SHarper/TrigNtup/interface/TrigBitsDef.hh"

struct EleDataTruthTreeStruct : public TreeDataStructBase {
public:
  int cutCode;
  EleStructs::EleSimpleStruct ele;
  float truthDR;
  
  EleStructs::EvtInfoStruct evt;
  EleStructs::GenPUStruct genPU;
  float weight;
  float genWeight;
  float puWeight;
  float eleTruthNrgy;
  float eleTruthPhi;
  float eleTruthEta;
  float eleTruthDetEta;
  float eleTruthEt;
  float eleTruthZ;
  int eleTruthRegion;
  int pid;
  
  EleStructs::P4Struct hltP4;
  std::array<int,TrigBitsDef::kNrFilterWords> eleTrigs; 
  std::array<int,TrigBitsDef::kNrPathWords> evtTrigs;
  EleStructs::HLTEgammaStruct egHLT;
  EleStructs::HLTEgammaStruct egHLTUS;
  
public:
  EleDataTruthTreeStruct(){}
  EleDataTruthTreeStruct(const EleDataTruthTreeStruct& rhs)=delete;
  EleDataTruthTreeStruct& operator()(const EleDataTruthTreeStruct& rhs)=delete;
  void createBranches(TTree* tree);
  void setBranchAddresses(TTree* tree); //rather assumes that the tree is an eledata tree
  void copyTree(TTree* oldTree,TTree* newTree,bool (*cutFunc)(EleDataTruthTreeStruct&),void (*manipFunc)(EleDataTruthTreeStruct&)=NULL);
 

};

#endif
