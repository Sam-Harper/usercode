#include "SHarper/TrigNtup/interface/EgHLTTagProbeTreeStruct.hh"



void EgHLTTagProbeTreeStruct::createBranches(TTree* tree)
{
  int buffSize = 3200000;  
  addBranch(tree,"evt",evt,buffSize);
  addBranch(tree,"genPU",genPU,buffSize);
  addBranch(tree,"weight",weight,buffSize);
  addBranch(tree,"genWeight",genWeight,buffSize);
  addBranch(tree,"puWeight",puWeight,buffSize);
  addBranch(tree,"evtTrigs",evtTrigs,buffSize);  

  addBranch(tree,"tagTrigs",tagTrigs,buffSize);
  addBranch(tree,"tagHLT",tagHLT,buffSize);
  addBranch(tree,"tagHLTRegion",tagHLTRegion,buffSize);
  addBranch(tree,"tagMC",tagMC,buffSize);
  addBranch(tree,"tagTruthRegion",tagTruthRegion,buffSize);
  addBranch(tree,"tagTruthZ",tagTruthZ,buffSize);
  addBranch(tree,"tagPID",tagPID,buffSize);
  addBranch(tree,"tagTruthDR",tagTruthDR,buffSize);
  addBranch(tree,"tagReco",tagReco,buffSize);
  addBranch(tree,"tagCutCode",tagCutCode,buffSize);
  addBranch(tree,"tagRecoDR",tagRecoDR,buffSize);  

  addBranch(tree,"probeTrigs",probeTrigs,buffSize);
  addBranch(tree,"probeHLT",probeHLT,buffSize);
  addBranch(tree,"probeHLTRegion",probeHLTRegion,buffSize);
  addBranch(tree,"probeMC",probeMC,buffSize);
  addBranch(tree,"probeTruthRegion",probeTruthRegion,buffSize);
  addBranch(tree,"probeTruthZ",probeTruthZ,buffSize);
  addBranch(tree,"probePID",probePID,buffSize);
  addBranch(tree,"probeTruthDR",probeTruthDR,buffSize);
  addBranch(tree,"probeReco",probeReco,buffSize);
  addBranch(tree,"probeCutCode",probeCutCode,buffSize);
  addBranch(tree,"probeRecoDR",probeRecoDR,buffSize);  
}

void EgHLTTagProbeTreeStruct::setBranchAddresses(TTree* tree)
{
  tree->SetBranchAddress("evt",&evt);
  tree->SetBranchAddress("genPU",&genPU);
  tree->SetBranchAddress("weight",&weight); 
  tree->SetBranchAddress("genWeight",&genWeight); 
  tree->SetBranchAddress("puWeight",&puWeight);
  tree->SetBranchAddress("evtTrigs",&evtTrigs); 

  tree->SetBranchAddress("tagTrigs",&tagTrigs); 
  tree->SetBranchAddress("tagHLT",&tagHLT);
  tree->SetBranchAddress("tagHLTRegion",&tagHLTRegion);
  tree->SetBranchAddress("tagMC",&tagMC);
  tree->SetBranchAddress("tagTruthZ",&tagTruthZ);
  tree->SetBranchAddress("tagTruthRegion",&tagTruthRegion);
  tree->SetBranchAddress("tagPID",&tagPID);
  tree->SetBranchAddress("tagTruthDR",&tagTruthDR);
  tree->SetBranchAddress("tagReco",&tagReco);
  tree->SetBranchAddress("tagCutCode",&tagCutCode);
  tree->SetBranchAddress("tagRecoDR",&tagRecoDR); 
  
  tree->SetBranchAddress("probeTrigs",&probeTrigs); 
  tree->SetBranchAddress("probeHLT",&probeHLT);
  tree->SetBranchAddress("probeHLTRegion",&probeHLTRegion);
  tree->SetBranchAddress("probeMC",&probeMC);
  tree->SetBranchAddress("probeTruthZ",&probeTruthZ);
  tree->SetBranchAddress("probeTruthRegion",&probeTruthRegion);
  tree->SetBranchAddress("probePID",&probePID);
  tree->SetBranchAddress("probeTruthDR",&probeTruthDR);
  tree->SetBranchAddress("probeReco",&probeReco);
  tree->SetBranchAddress("probeCutCode",&probeCutCode);
  tree->SetBranchAddress("probeRecoDR",&probeRecoDR);
}


//this allows the struct to be modified and then pass some arbitary cuts
void EgHLTTagProbeTreeStruct::copyTree(TTree* oldTree,TTree* newTree,bool (*cutFunc)(EgHLTTagProbeTreeStruct&),void (*manipFunc)(EgHLTTagProbeTreeStruct&))
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

