#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "SHarper/SHNtupliser/interface/SHEvent.hh"
#include "SHarper/SHNtupliser/interface/SHEventHelper.h"
#include "SHarper/SHNtupliser/interface/SHEventTreeData.h"
#include "SHarper/HEEPAnalyzer/interface/HEEPEventHelper.h"
#include "SHarper/HEEPAnalyzer/interface/HEEPEvent.h"
#include "SHarper/TrigNtup/interface/EleDataTruthTreeMaker.hh"
#include "SHarper/SHNtupliser/interface/SHCaloGeom.hh"
#include "SHarper/SHNtupliser/interface/GeomFuncs.hh"
#include "SHarper/SHNtupliser/interface/SHGeomFiller.h"
#include "TFile.h"
#include "TTree.h"

#include <string>

class TrigNtupMaker : public edm::EDAnalyzer {

private:
  heep::EventHelper evtHelper_; //this is our magic class where all the nastyness is contained
  heep::Event heepEvt_;

  SHEventHelper shEvtHelper_;
protected:
  SHEvent* shEvt_; //pointer because thats what root likes, we own it, its protected as derived classes need to set it (and they have a pointer the derived SHEvent so theres no point protecting access, they have it...)
private:
  SHEventTreeData shEvtTree_;

  TFile* outFile_; //we do not own it

  std::string outputFilename_;

  bool initGeom_;

  bool outputGeom_; //write out geom to file
  
  std::unique_ptr<EleDataTruthTreeMaker> genTreeMaker_;

  TrigNtupMaker(const TrigNtupMaker& rhs)=delete;
  TrigNtupMaker& operator=(const TrigNtupMaker& rhs)=delete;

public:
  explicit TrigNtupMaker(const edm::ParameterSet& iPara);
  virtual ~TrigNtupMaker();
  
private:
  virtual void beginJob() ;
  virtual void beginRun(const edm::Run& run,const edm::EventSetup& iSetup);
  virtual void analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup);
  virtual void endRun(edm::Run const& iRun, edm::EventSetup const&){}
  virtual void endJob() ;

protected:
  bool fillSHEvent(const edm::Event& iEvent,const edm::EventSetup& iSetup);
  void fillTree();
  
  //makes this able to store objects inheriting from SHEvent
  virtual void initSHEvent();
  
  

};

void TrigNtupMaker::initSHEvent()
{
  if(shEvt_) delete shEvt_;
  shEvt_ = new SHEvent;
}

void TrigNtupMaker::fillTree()
{
  genTreeMaker_->fill(shEvt_);
}

TrigNtupMaker::TrigNtupMaker(const edm::ParameterSet& iPara):
  shEvt_(nullptr),shEvtTree_(shEvt_),outFile_(nullptr),initGeom_(false)
{
  evtHelper_.setup(iPara,consumesCollector(),*this);
  shEvtHelper_.setup(iPara,consumesCollector());
  shEvtTree_.setup(iPara);

  outputFilename_ = iPara.getParameter<std::string>("outputFilename");
  outputGeom_ = iPara.getParameter<bool>("outputGeom");
 
}

TrigNtupMaker::~TrigNtupMaker()
{
  if(shEvt_) delete shEvt_;
}

void TrigNtupMaker::beginJob()
{
  initSHEvent();
 
  std::cout <<"opening file "<<outputFilename_.c_str()<<std::endl;
  edm::Service<TFileService> fs;
  outFile_ = &fs->file();
  outFile_->cd();
  genTreeMaker_ = std::make_unique<EleDataTruthTreeMaker>("hltGenTree","");
  //  shEvtTree_.makeTree("evtTree");
} 

void TrigNtupMaker::beginRun(const edm::Run& run,const edm::EventSetup& iSetup)
{ 
  if(!initGeom_){
  //write out calogeometry
   
    SHGeomFiller geomFiller(iSetup);  
    SHCaloGeom ecalGeom(SHCaloGeom::ECAL);
    SHCaloGeom hcalGeom(SHCaloGeom::HCAL);
    geomFiller.fillEcalGeom(ecalGeom);
    geomFiller.fillHcalGeom(hcalGeom);
    if(outputGeom_){
      std::cout <<"writing geom "<<std::endl;
      outFile_->WriteObject(&ecalGeom,"ecalGeom");
      outFile_->WriteObject(&hcalGeom,"hcalGeom");
    }
    GeomFuncs::loadCaloGeom(ecalGeom,hcalGeom);
    initGeom_=true;
  }
  evtHelper_.makeHeepEvent(run,iSetup,heepEvt_);
}

void TrigNtupMaker::analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup)
{
  if(fillSHEvent(iEvent,iSetup)){
    fillTree();
  }
  shEvt_->clear();
  shEvt_->flushTempData();
} 

bool TrigNtupMaker::fillSHEvent(const edm::Event& iEvent,const edm::EventSetup& iSetup)
{
  evtHelper_.makeHeepEvent(iEvent,iSetup,heepEvt_);
  shEvtHelper_.makeSHEvent(heepEvt_,*shEvt_);
  return true;

}




void TrigNtupMaker::endJob()
{ 
  TrigBitsDef::write(outFile_);
 
  
  // outFile_->cd();
  // //quick and dirty hack as writing ints directly isnt working
  // TTree* tree = new TTree("eventCountTree","Event count");
  // tree->Branch("nrPass",&nrPass_,"nrPass/I");
  // tree->Branch("nrTot",&nrTot_,"nrTot/I");
  // tree->Fill();

  // std::cout <<"job ended "<<std::endl;
}


  


//define this as a plug-in
DEFINE_FWK_MODULE(TrigNtupMaker);
