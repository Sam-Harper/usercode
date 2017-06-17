#ifndef EGHLTTAGPROBETREESTRUCT_HH
#define EGHLTTAGPROBETREESTRUCT_HH

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

struct EgHLTTagProbeTreeStruct : public TreeDataStructBase {
public:
  EleStructs::EvtInfoStruct evt;
  EleStructs::GenPUStruct genPU;
  float weight;
  float genWeight;
  float puWeight;

  std::array<int,TrigBitsDef::kNrFilterWords> tagTrigs; 
  std::array<int,TrigBitsDef::kNrFilterWords> probeTrigs; 
  std::array<int,TrigBitsDef::kNrPathWords> evtTrigs;
 
  float mass;

  EleStructs::HLTEgammaStruct tagHLT,probeHLT;
  int tagHLTRegion,probeHLTRegion;

  EleStructs::P4DetEtaStruct tagMC,probeMC;
  float tagTruthZ,probeTruthZ;
  int tagTruthRegion,probeTruthRegion;
  int tagPID,probePID;
  float tagTruthDR,probeTruthDR;
  
  EleStructs::EleSimpleStruct tagReco,probeReco;  
  int tagCutCode,probeCutCode;
  float tagRecoDR,probeRecoDR;

public:
  EgHLTTagProbeTreeStruct(){}
  EgHLTTagProbeTreeStruct(const EgHLTTagProbeTreeStruct& rhs)=delete;
  EgHLTTagProbeTreeStruct& operator()(const EgHLTTagProbeTreeStruct& rhs)=delete;
  void createBranches(TTree* tree);
  void setBranchAddresses(TTree* tree); //rather assumes that the tree is an eledata tree
  void copyTree(TTree* oldTree,TTree* newTree,bool (*cutFunc)(EgHLTTagProbeTreeStruct&),void (*manipFunc)(EgHLTTagProbeTreeStruct&)=NULL);
 

};

#endif
