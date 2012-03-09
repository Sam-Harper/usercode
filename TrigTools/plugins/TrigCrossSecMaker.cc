#include "SHarper/TrigTools/interface/TrigCrossSecMaker.h"

#include "SHarper/TrigTools/interface/TrigToolsFuncs.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "TFile.h"
#include "TTree.h"

#include <iostream>
#include <fstream>

const float TrigCrossSecMaker::TrigCrossSecData::LumiSecData::kLumiSecLength(23.3);

TrigCrossSecMaker::TrigCrossSecMaker(const edm::ParameterSet& iPara)
{
  trigResultsTag_ = iPara.getParameter<edm::InputTag>("trigResultsTag");
  pathName_ = iPara.getParameter<std::string>("pathName");
  selectingPathName_= iPara.getParameter<std::string>("selectingPathName");
  outputFilename_ = iPara.getParameter<std::string>("outputFilename"); 

  std::string lumiDataFilename =iPara.getParameter<std::string>("lumiDataFilename");
  trigCrossSecData_.addLumiSecData(lumiDataFilename);
}


void TrigCrossSecMaker::beginRun(const edm::Run& run,const edm::EventSetup& setup)
{
  bool changed=false;
  hltConfig_.init(run,setup,"HLT",changed); //as we need the orginal HLT config...
  std::cout <<"table name "<<hltConfig_.tableName()<<std::endl;
 
}

void TrigCrossSecMaker::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  
  edm::Handle<edm::TriggerResults> trigResultsHandle;
  iEvent.getByLabel(trigResultsTag_,trigResultsHandle);
  
  const edm::TriggerResults& trigResults = *trigResultsHandle;
  const edm::TriggerNames& trigNames = iEvent.triggerNames(trigResults);  
  
  int runnr=iEvent.id().run();
  int lumiSec = iEvent.luminosityBlock();
  
  //if(lumiSec>=314 && lumiSec<=341) return; //temp skip for bad lumi
  
  size_t pathIndex = trigNames.triggerIndex(pathName_);
  int preScale = !selectingPathName_.empty() ? hltConfig_.prescaleValue(iEvent,iSetup,selectingPathName_) : 1;
  //std::cout <<"prescale "<<preScale<<std::endl;
  if(pathIndex<trigResults.size() && trigResults.accept(pathIndex)){
    trigCrossSecData_.fill(runnr,lumiSec,preScale);
    
  }
 
}
void TrigCrossSecMaker::endJob()
{
  TFile* file = new TFile(outputFilename_.c_str(),"RECREATE");
   
  float instLumiRec;
  float instLumiDel;
  float nrWeights;
  int nrEntries;
  TTree* tree = new TTree("trigCrossSecTree","tree");
  tree->Branch("instLumiRec",&instLumiRec,"instLumiRec/F");
  tree->Branch("instLumiDel",&instLumiDel,"instLumiDel/F");
  tree->Branch("nrWeights",&nrWeights,"nrWeights/F");
  tree->Branch("nrEntries",&nrEntries,"nrEntries/I");


  
  
  for(size_t lumiNr=0;lumiNr<trigCrossSecData_.nrLumiSecs();lumiNr++){
    instLumiRec = trigCrossSecData_.lumiSec(lumiNr).instLumiRecorded();
    instLumiDel = trigCrossSecData_.lumiSec(lumiNr).instLumiDelivered();
    nrWeights = trigCrossSecData_.lumiSec(lumiNr).nrWeights();
    nrEntries = trigCrossSecData_.lumiSec(lumiNr).nrEntries();
    tree->Fill();
  }
  file->WriteObject(&pathName_,"pathName");
  file->Write();
  delete file;

}
//I have to write a better file reader....
void TrigCrossSecMaker::TrigCrossSecData::addLumiSecData(const std::string& filename)
{
  if(!filename.empty()){
    std::ifstream file(filename.c_str());
    if(file.bad()){
      std::cout<<" file "<<filename<<" not found"<<std::endl;
    }else if(file.eof()){
      std::cout<<" file "<<filename<<" found but empty"<<std::endl;
    }
    
    if(!file.bad()){
      while(!file.eof()){

	char tempBuffer[256]; 
	file.getline(tempBuffer,256);
	
	std::pair<int,float> runLumi;

	char* runnrChar = strtok(tempBuffer,"|");
	char* lumiChar = strtok(NULL,"|");
	char* deliveredChar = strtok(NULL,"|");
	char* recordedChar = strtok(NULL,"|");
	if(runnrChar && lumiChar && deliveredChar && recordedChar){
	  runLumi.first = atoi(runnrChar);
	  runLumi.second = atof(lumiChar);
	  LumiSecData data(atoi(runnrChar), atoi(lumiChar),atof(deliveredChar),atof(recordedChar));
	  lumiSecData_.push_back(data);
	}
      }
    }
  }
  std::sort(lumiSecData_.begin(),lumiSecData_.end());

}
void TrigCrossSecMaker::TrigCrossSecData::fill(int runnr,int lumiSec,int nrEvents)
{
  //I'm going to be lazy and do in a badly designed way
  //what I should to is to do it off std::pair<runnr,lumi> but I cant be bothered
  LumiSecData dummyLumiSec(runnr,lumiSec,0,0);
  typedef std::vector<LumiSecData>::iterator LumiIt;
  std::pair<LumiIt,LumiIt> lumiSecIt = std::equal_range(lumiSecData_.begin(),lumiSecData_.end(),dummyLumiSec);
  if(lumiSecIt.second-lumiSecIt.first==1) lumiSecIt.first->addEvent(nrEvents);
  else {
    std::cout <<"in "<<__FUNCTION__<<"() (in "<<__FILE__<<", line "<<__LINE__<<") : WARNING run "<<runnr<<" lumisec "<<lumiSec<<" found "<<lumiSecIt.second-lumiSecIt.first<<" times"<<std::endl;
  }
  

}




//define this as a plug-in
DEFINE_FWK_MODULE(TrigCrossSecMaker);
