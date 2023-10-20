#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Common/interface/Ptr.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "SHarper/HEEPAnalyzer/interface/HEEPCutCodes.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"


#include "TH1D.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

class HEEPIdAndVIDComp : public edm::one::EDAnalyzer<> {

private:
 
  edm::EDGetTokenT<edm::View<reco::GsfElectron> > eleToken_;
  edm::EDGetTokenT<edm::ValueMap<int> > heepIdToken_;
  edm::EDGetTokenT<edm::ValueMap<bool> > vidToken_;
  edm::EDGetTokenT<reco::GenParticleCollection> genPartToken_;
  TH1* passHistEB_;
  TH1* totHistEB_;
  TH1* passHistEE_;
  TH1* totHistEE_;
  
public:
  explicit HEEPIdAndVIDComp(const edm::ParameterSet& iPara);
  virtual ~HEEPIdAndVIDComp(){}
  
private:
  void beginJob() override;
  void endJob() override;
  void analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup) override;
  const reco::GenParticle* matchGenPart(const reco::GsfElectron& ele,edm::Handle<reco::GenParticleCollection>& genParts);
};
  

HEEPIdAndVIDComp::HEEPIdAndVIDComp(const edm::ParameterSet& iPara)
{
  eleToken_=consumes<edm::View<reco::GsfElectron> >(iPara.getParameter<edm::InputTag>("eleLabel"));
  heepIdToken_=consumes<edm::ValueMap<int> >(iPara.getParameter<edm::InputTag>("heepId"));
  vidToken_=consumes<edm::ValueMap<bool> >(iPara.getParameter<edm::InputTag>("vid"));
  genPartToken_=consumes<reco::GenParticleCollection>(iPara.getParameter<edm::InputTag>("genPartLabel"));
  
}

void HEEPIdAndVIDComp::beginJob()
{
  edm::Service<TFileService> fs;
  passHistEB_ = fs->make<TH1D>("passHistEB",";E_{T} [GeV]",500,0,5000);
  totHistEB_ = fs->make<TH1D>("totHistEB",";E_{T} [GeV]",500,0,5000);
  passHistEE_ = fs->make<TH1D>("passHistEE",";E_{T} [GeV]",500,0,5000);
  totHistEE_ = fs->make<TH1D>("totHistEE",";E_{T} [GeV]",500,0,5000);
  

}
void HEEPIdAndVIDComp::endJob()
{
  edm::Service<TFileService> fs;
  auto effHistEB = fs->make<TH1D>("effHistEB",";E_{T} [GeV]; Efficiency",500,0,5000);
  effHistEB->Sumw2();
  effHistEB->Divide(passHistEB_,totHistEB_,1,1,"B");
  
  auto effHistEE = fs->make<TH1D>("effHistEE",";E_{T} [GeV]; Efficiency",500,0,5000);
  effHistEE->Sumw2();
  effHistEE->Divide(passHistEE_,totHistEE_,1,1,"B");
  
}

void HEEPIdAndVIDComp::analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup)
{
  edm::Handle<edm::View<reco::GsfElectron> > eleHandle;
  edm::Handle<edm::ValueMap<int> > heepId;
  edm::Handle<edm::ValueMap<bool> > vid;
  edm::Handle<reco::GenParticleCollection> genPartHandle;

  iEvent.getByToken(eleToken_,eleHandle);
  iEvent.getByToken(heepIdToken_,heepId);
  iEvent.getByToken(vidToken_,vid);
  iEvent.getByToken(genPartToken_,genPartHandle);

  for(size_t eleNr=0;eleNr<eleHandle->size();eleNr++){  
    edm::Ptr<reco::GsfElectron> elePtr(eleHandle,eleNr);

    

    bool passVid=(*vid)[elePtr];
    int heepBits =(*heepId)[elePtr];
    bool passHEEP=heepBits==0x0;
    if(passVid!=passHEEP){
      std::cout <<"VID - HEEP disagreement, vid "<<passVid<<" heep "<<heep::CutCodes::getCodeName(heepBits)<<std::endl;
    }
    const reco::GenParticle* genPart = matchGenPart(*elePtr,genPartHandle);
    if(genPart){

      float detEtaAbs = std::abs(elePtr->superCluster()->eta());
      if(detEtaAbs<1.4442 && elePtr->et()>35){
	if(passHEEP) passHistEB_->Fill(genPart->et());
	totHistEB_->Fill(genPart->et());
      }else if(detEtaAbs>1.566 && detEtaAbs<2.5 && elePtr->et()>35){
	if(passHEEP) passHistEE_->Fill(genPart->et());
	totHistEE_->Fill(genPart->et());
      }
    }
  }
}

const reco::GenParticle* HEEPIdAndVIDComp::matchGenPart(const reco::GsfElectron& ele,edm::Handle<reco::GenParticleCollection>& genParts)
{
  if(!genParts.isValid()) return nullptr;
  
  const float eta = ele.eta();
  const float phi = ele.phi();
  float bestDR2 = 0.1*0.1;
  const reco::GenParticle* bestMatch=nullptr;
  //std::cout <<"ele et "<<ele.et()<<" eta "<<eta<<" "<<phi<<std::endl;
  for(auto& genPart : *genParts){
    if(!genPart.statusFlags().isLastCopyBeforeFSR() || std::abs(genPart.pdgId())!=11 || !genPart.statusFlags().isPrompt()) continue;
    //std::cout <<" gen part "<<genPart.et()<<" "<<genPart.eta()<<" "<<genPart.phi()<<" "<<genPart.pdgId()<<std::endl;
    float dR2= reco::deltaR2(genPart.eta(),genPart.phi(),eta,phi)<0.1*0.1;
    if(dR2<bestDR2){
      bestDR2=dR2;
      bestMatch = &genPart;
    }
  }
  return bestMatch;
}
DEFINE_FWK_MODULE(HEEPIdAndVIDComp);
