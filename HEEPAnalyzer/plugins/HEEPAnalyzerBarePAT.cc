

// Description: A simple example HEEPAnalyzer which makes a mass spectrum of selected electron pairs

// Implementation: 
//    This does it using just bare PAT with only heep::EleSelector which does the heep selection
//
 
//
// Original Author:  S. Harper
//         Created: Tues Sep 2 2008


#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"



#include "TH1D.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "SHarper/HEEPAnalyzer/interface/HEEPEleSelector.h"
#include "SHarper/HEEPAnalyzer/interface/HEEPGsfEleExtraFiller.h"




class HEEPAnalyzerBarePAT : public edm::one::EDAnalyzer<> {

private:
  heep::EleSelector cuts_; //allows us to apply the heep selection
  heep::GsfEleExtraFiller eleExtraFiller_;
  edm::EDGetTokenT<edm::View<pat::Electron>> eleToken_;
  
  
  //the next three variables are simply for the example analysis
  int nrPass_;
  int nrFail_;
  TH1* massHist_; //we do not own hist

  //disabling copy and assignment 
  //this class is in theory copyable but it seems odd to allow it
private:
  HEEPAnalyzerBarePAT(const HEEPAnalyzerBarePAT& rhs)=delete;
  HEEPAnalyzerBarePAT& operator=(const HEEPAnalyzerBarePAT& rhs)=delete;

public:
  explicit HEEPAnalyzerBarePAT(const edm::ParameterSet& iPara);
  virtual ~HEEPAnalyzerBarePAT(){}
  
private:
  virtual void beginJob() ;
  virtual void analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup);
  virtual void endJob() ;
  
};




HEEPAnalyzerBarePAT::HEEPAnalyzerBarePAT(const edm::ParameterSet& iPara):
  cuts_(iPara),eleExtraFiller_(iPara,consumesCollector()),nrPass_(0),nrFail_(0)
{
  eleToken_=consumes<edm::View<pat::Electron> >(iPara.getParameter<edm::InputTag>("eleLabel"));
  
}

void HEEPAnalyzerBarePAT::beginJob()
{
  edm::Service<TFileService> fs;
  massHist_ = fs->make<TH1D>("massHist","Di-Electron Mass;M_{ee} (GeV/c^{2});# Events / 10 GeV/c^{2}",250,50,2550); 
}

//this analyser shows you how to apply the heep selection to pat electrons straight out of pat
//it will also work if you replace  const edm::View<pat::Electron>& eles  by const reco::GsfElectronCollection& eles
//the energy correction will have no effect if the energy correction is already applied
void HEEPAnalyzerBarePAT::analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup)
{ 
  edm::Handle<edm::View<pat::Electron> > eleHandle;
  iEvent.getByToken(eleToken_,eleHandle);
  const edm::View<pat::Electron>& eles = *(eleHandle.product());
  eleExtraFiller_.getEvtContent(iEvent,iSetup);
  
  //do what ever you wa
  //count the number that pass / fail cuts
  for(size_t eleNr=0;eleNr<eles.size();eleNr++){
    if(cuts_.passCuts(eles[eleNr],eleExtraFiller_(eles.ptrAt(eleNr)))) nrPass_++;
    else nrFail_++;
  }
  
  //lets fill a mass hist
  for(size_t ele1Nr=0;ele1Nr<eles.size();ele1Nr++){
    for(size_t ele2Nr=ele1Nr+1;ele2Nr<eles.size();ele2Nr++){
      const pat::Electron& ele1 = eles[ele1Nr];
      const pat::Electron& ele2 = eles[ele2Nr];
      int ele1CutCode = cuts_.getCutCode(ele1,eleExtraFiller_(eles.ptrAt(ele1Nr)));
      int ele2CutCode = cuts_.getCutCode(ele2,eleExtraFiller_(eles.ptrAt(ele2Nr)));
     
      if(ele1CutCode==0x0 && ele2CutCode==0x0 && !(ele1.isEE() && ele2.isEE())){ //EB-EB, EE-EE
	math::XYZTLorentzVector ele1P4 = ele1.p4()*ele1.caloEnergy()/ele1.energy();
	math::XYZTLorentzVector ele2P4 = ele2.p4()*ele2.caloEnergy()/ele2.energy();	
	float mass = (ele1P4+ele2P4).mag();
	massHist_->Fill(mass);
      }
    }
  }
  
}


void HEEPAnalyzerBarePAT::endJob()
{
 
  edm::LogInfo("HEEPAnalyzerBarePAT") <<"Nr pass "<<nrPass_<<" nr fail "<<nrFail_<<" eff "<<static_cast<float>(nrPass_)/static_cast<float>(nrFail_+nrPass_);
}

//define this as a plug-in
DEFINE_FWK_MODULE(HEEPAnalyzerBarePAT);
