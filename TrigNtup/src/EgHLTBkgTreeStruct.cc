#include "SHarper/TrigNtup/interface/EgHLTBkgTreeStruct.hh"



void EgHLTBkgTreeStruct::createBranches(TTree* tree)
{
  int buffSize = 3200000;  
  addBranch(tree,"evt",evt,buffSize);
  addBranch(tree,"genPU",genPU,buffSize);
  addBranch(tree,"weight",weight,buffSize);
  addBranch(tree,"genWeight",genWeight,buffSize);
  addBranch(tree,"puWeight",puWeight,buffSize);
  addBranch(tree,"eleTrigs",eleTrigs,buffSize);
  addBranch(tree,"evtTrigs",evtTrigs,buffSize);  
  addBranch(tree,"egHLT",egHLT,buffSize);
  addBranch(tree,"hltRegion",hltRegion,buffSize);
  addBranch(tree,"mc",mc,buffSize);
  addBranch(tree,"truthRegion",truthRegion,buffSize);
  addBranch(tree,"eleTruthZ",eleTruthZ,buffSize);
  addBranch(tree,"pid",pid,buffSize);
  addBranch(tree,"truthDR",truthDR,buffSize);
  addBranch(tree,"ele",ele,buffSize);
  addBranch(tree,"cutCode",cutCode,buffSize);
  addBranch(tree,"recoDR",recoDR,buffSize);  
}

void EgHLTBkgTreeStruct::setBranchAddresses(TTree* tree)
{
  tree->SetBranchAddress("evt",&evt);
  tree->SetBranchAddress("genPU",&genPU);
  tree->SetBranchAddress("weight",&weight); 
  tree->SetBranchAddress("genWeight",&genWeight); 
  tree->SetBranchAddress("puWeight",&puWeight);
  tree->SetBranchAddress("eleTrigs",&eleTrigs); 
  tree->SetBranchAddress("evtTrigs",&evtTrigs); 
  tree->SetBranchAddress("egHLT",&egHLT);
  tree->SetBranchAddress("hltRegion",&hltRegion);
  tree->SetBranchAddress("mc",&mc);
  tree->SetBranchAddress("eleTruthZ",&eleTruthZ);
  tree->SetBranchAddress("truthRegion",&truthRegion);
  tree->SetBranchAddress("pid",&pid);
  tree->SetBranchAddress("truthDR",&truthDR);
  tree->SetBranchAddress("ele",&ele);
  tree->SetBranchAddress("cutCode",&cutCode);
  tree->SetBranchAddress("recoDR",&recoDR);
}


//this allows the struct to be modified and then pass some arbitary cuts
void EgHLTBkgTreeStruct::copyTree(TTree* oldTree,TTree* newTree,bool (*cutFunc)(EgHLTBkgTreeStruct&),void (*manipFunc)(EgHLTBkgTreeStruct&))
{
  setBranchAddresses(oldTree);
  setBranchAddresses(newTree);
  int nrEntries = oldTree->GetEntries();
  for(int entryNr=0;entryNr<nrEntries;entryNr++){
    oldTree->GetEntry(entryNr);
    if(manipFunc!=NULL) (*manipFunc)(*this);
    if((*cutFunc)(*this)) newTree->Fill();
  }
}

