#include "SHarper/TrigNtup/interface/EleDataTruthTreeStruct.hh"



void EleDataTruthTreeStruct::createBranches(TTree* tree)
{
  int buffSize = 3200000;  
  addBranch(tree,"evt",evt,buffSize);
  addBranch(tree,"genPU",genPU,buffSize);
  tree->Branch("weight",&weight,"weight/F",buffSize);
  tree->Branch("genWeight",&genWeight,"genWeight/F",buffSize);
  tree->Branch("puWeight",&puWeight,"puWeight/F",buffSize);
  tree->Branch("truthDR",&truthDR,"truthDR/F",buffSize);
  tree->Branch("cutCode",&cutCode,"cutCode/I",buffSize);
  addBranch(tree,"ele",ele,buffSize);
  tree->Branch("truthNrgy",&eleTruthNrgy,"truthNrgy/F",buffSize);
  tree->Branch("eleTruthPhi",&eleTruthPhi,"eleTruthPhi/F",buffSize);
  tree->Branch("eleTruthEta",&eleTruthEta,"eleTruthEta/F",buffSize); 
  tree->Branch("eleTruthDetEta",&eleTruthDetEta,"eleTruthDetEta/F",buffSize); 
  tree->Branch("eleTruthEt",&eleTruthEt,"eleTruthEt/F",buffSize);
  addBranch(tree,"eleTruthZ",eleTruthZ,buffSize);
  addBranch(tree,"pid",pid,buffSize);
  addBranch(tree,"eleTrigs",eleTrigs,buffSize);
  addBranch(tree,"evtTrigs",evtTrigs,buffSize);  
  addBranch(tree,"hlt",hltP4,buffSize); 
  addBranch(tree,"egHLT",egHLT,buffSize);
  addBranch(tree,"egHLTUS",egHLTUS,buffSize);
  
}

void EleDataTruthTreeStruct::setBranchAddresses(TTree* tree)
{
  tree->SetBranchAddress("evt",&evt);
  tree->SetBranchAddress("genPU",&genPU);
  tree->SetBranchAddress("weight",&weight); 
  tree->SetBranchAddress("genWeight",&genWeight); 
  tree->SetBranchAddress("puWeight",&puWeight);
  tree->SetBranchAddress("truthDR",&truthDR);
  tree->SetBranchAddress("cutCode",&cutCode);
  tree->SetBranchAddress("ele",&ele); 
  tree->SetBranchAddress("truthNrgy",&eleTruthNrgy);
  tree->SetBranchAddress("eleTruthPhi",&eleTruthPhi);
  tree->SetBranchAddress("eleTruthEta",&eleTruthEta);
  tree->SetBranchAddress("eleTruthDetEta",&eleTruthDetEta); 
  tree->SetBranchAddress("eleTruthEt",&eleTruthEt); 
  tree->SetBranchAddress("eleTruthZ",&eleTruthZ);
  tree->SetBranchAddress("pid",&pid);
  tree->SetBranchAddress("hlt",&hltP4); 
  tree->SetBranchAddress("eleTrigs",&eleTrigs); 
  tree->SetBranchAddress("evtTrigs",&evtTrigs); 
  tree->SetBranchAddress("egHLT",&egHLT);
  tree->SetBranchAddress("egHLT",&egHLTUS);
 
}


//this allows the struct to be modified and then pass some arbitary cuts
void EleDataTruthTreeStruct::copyTree(TTree* oldTree,TTree* newTree,bool (*cutFunc)(EleDataTruthTreeStruct&),void (*manipFunc)(EleDataTruthTreeStruct&))
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

