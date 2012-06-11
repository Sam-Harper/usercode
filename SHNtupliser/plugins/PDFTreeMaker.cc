
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "TTree.h"

struct EventId {
  
  int runnr;
  int lumiSec;
  int eventnr;
  int datasetCode;
  EventId():runnr(0),lumiSec(0),eventnr(0),datasetCode(-1){}
  EventId(int iRunnr,int iLumiSec,int iEventnr,int iDataset=-1):runnr(iRunnr),lumiSec(iLumiSec),eventnr(iEventnr),datasetCode(iDataset){}

  static std::string contents(){return "runnr/I:lumiSec:eventnr:datasetCode";}
  
  bool operator<(const EventId& rhs){
    if(datasetCode<rhs.datasetCode) return true;
    else if(datasetCode>rhs.datasetCode) return false;
    else { //same dataset
      if(runnr<rhs.runnr) return true;
      else if(runnr>rhs.runnr) return false;
      else{ //same run
	if(lumiSec<rhs.lumiSec) return true;
	else if(lumiSec>rhs.lumiSec) return false;
	else if(eventnr<rhs.eventnr) return true;
	else return false;
      }
    }
  }

  bool operator==(const EventId& rhs){
    return datasetCode==rhs.datasetCode && runnr==rhs.runnr && lumiSec==rhs.lumiSec && eventnr==rhs.eventnr;
  }

};

struct PDFInfo{
  int id1,id2;
  float x1,pdf1;
  float x2,pdf2;
  float scale;

  PDFInfo():id1(0),id2(0),x1(0),pdf1(0),x2(0),pdf2(0),scale(0){}
 
};

struct P4Struct {
  float nrgy,et,eta,phi,pt,mass;
  void fill(const math::XYZTLorentzVector& p4){
    if(p4.Pt()!=0 && p4.Et()!=0){
      nrgy = p4.E();
      et = p4.Et();
      eta = p4.Eta();
      phi = p4.Phi();
      pt =p4.Pt();
      mass = p4.mag();
    }else clear();
  }
  void clear(){nrgy=-999;et=-999;eta=-999;phi=-0;pt=-999;mass=-999;}
  static std::string contents(){return "nrgy/F:et:eta:phi:pt:mass";}
};

class PDFTreeMaker : public edm::EDAnalyzer {

private:

  TTree* tree_;
  std::vector<double> pdfWeights_;
  EventId evtId_;
  P4Struct bosonP4_; // as a sanity check we have the right event...
  int bosonId_;

  edm::InputTag genPartsTag_;
  edm::InputTag pdfWeightsTag_;
  int datasetCode_;
  
  
public:
  explicit PDFTreeMaker(const edm::ParameterSet& para);
  ~PDFTreeMaker(){}
  
  virtual void beginJob();
  virtual void analyze(const edm::Event& event,const edm::EventSetup& setup);
  virtual void endJob();
 
};

PDFTreeMaker::PDFTreeMaker(const edm::ParameterSet& para)

{
  genPartsTag_=para.getParameter<edm::InputTag>("genPartsTag");
  pdfWeightsTag_=para.getParameter<edm::InputTag>("pdfWeightsTag");
  datasetCode_=para.getParameter<int>("datasetCode");
  
}

void PDFTreeMaker::beginJob()
{
  edm::Service<TFileService> fs;
  fs->file().cd();
  tree_= new TTree("pdfTree","PDF Tree");
  tree_->Branch("evtId",&evtId_,EventId::contents().c_str());
  tree_->Branch("bosonP4",&bosonP4_,P4Struct::contents().c_str());
  tree_->Branch("bosonId",&bosonId_,"bosonId/I");
  tree_->Branch("pdfWeights",&pdfWeights_);
}

void PDFTreeMaker::analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup)
{ 
  edm::Handle<std::vector<double> > pdfWeightsHandle;
  iEvent.getByLabel(pdfWeightsTag_,pdfWeightsHandle);
  pdfWeights_ = *pdfWeightsHandle;
  evtId_.runnr = iEvent.id().run();
  evtId_.lumiSec = iEvent.luminosityBlock();
  evtId_.eventnr = iEvent.id().event();
  evtId_.datasetCode=datasetCode_;
  
  edm::Handle<reco::GenParticleCollection> genPartsHandle;
  iEvent.getByLabel(genPartsTag_,genPartsHandle);
  const reco::GenParticleCollection& genParts = *genPartsHandle;
  
  bosonId_=0;
  bosonP4_.clear();
  for(size_t partNr=0;partNr<genParts.size();partNr++){
    const reco::GenParticle& part = genParts[partNr];
    if(part.status()!=3) continue;
    if(part.pdgId()!=23 && part.pdgId()!=24) continue;
    bosonP4_.fill(part.p4());
    bosonId_=part.pdgId();
    break;
  }
  
  tree_->Fill();  
}

void PDFTreeMaker::endJob()
{ 
 
  
}  



DEFINE_FWK_MODULE(PDFTreeMaker);
