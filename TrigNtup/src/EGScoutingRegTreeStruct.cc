#include "SHarper/TrigNtup/interface/EGScoutingRegTreeStruct.hh"
#include "SHarper/SHNtupliser/interface/MathFuncs.hh"
#include "DataFormats/Scouting/interface/Run3ScoutingElectron.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"
#include "DataFormats/DetId/interface/DetId.h"

#include <array>

namespace {
  template<typename T1,typename T2=T1>
  void fillArrayFromVector(const std::vector<T1>& vec, T2* arr, size_t size) {
    for (size_t i = 0; i < size && i < vec.size(); ++i) {
      arr[i] = vec[i];
    }
  }

}




void EGScoutingRegTreeStruct::createBranches(TTree* tree)
{
  tree->Branch("nrVert",&nrVert,"nrVert/I");
  tree->Branch("rho",&rho,"rho/F");
  tree->Branch("nrPUInt",&nrPUInt,"nrPUInt/F");
  tree->Branch("nrPUIntTrue",&nrPUIntTrue,"nrPUIntTrue/F");
  tree->Branch("evt",&evt,evt.contents().c_str());
  tree->Branch("ele",&ele,ele.contents().c_str());
  tree->Branch("eleSS",&eleSS,eleSS.contents().c_str());
  tree->Branch("mc",&mc,mc.contents().c_str());

}

void EGScoutingRegTreeStruct::setBranchAddresses(TTree* tree)
{
  tree->SetBranchAddress("nrVert",&nrVert);
  tree->SetBranchAddress("rho",&rho);
  tree->SetBranchAddress("nrPUInt",&nrPUInt);
  tree->SetBranchAddress("nrPUIntTrue",&nrPUIntTrue);
  tree->SetBranchAddress("evt",&evt);
  tree->SetBranchAddress("ele",&ele);
  tree->SetBranchAddress("eleSS",&eleSS);
  tree->SetBranchAddress("mc",&mc);
  
}

void EGScoutingRegTreeStruct::fill(const edm::Event& event,int iNrVert,float iRho,float iNrPUInt,float iNrPUIntTrue,
				   const reco::GenParticle* iMC,const Run3ScoutingElectron* iEle)
{
  clear();

  nrVert = iNrVert;
  rho = iRho;
  nrPUInt = iNrPUInt;
  nrPUIntTrue = iNrPUIntTrue;
  evt.fill(event);
  if(iMC) mc.fill(*iMC, iEle ? std::sqrt(reco::deltaR2(iEle->eta(),iEle->phi(),iMC->eta(),iMC->phi())) : 999);
  if(iEle){
    ele.fill(*iEle);
    eleSS.fill(iEle->seedId(),iEle->detIds(),iEle->energyMatrix());
  }
}


namespace {
  DetId getOffsetId(const DetId& seedId,int iEtaOrIX,int iPhiOrIY){
    if(seedId.det()==DetId::Ecal && seedId.subdetId()==EcalBarrel){
      EBDetId ebId(seedId);
      return ebId.offsetBy(iEtaOrIX,iPhiOrIY);   
    }else if (seedId.det()==DetId::Ecal && seedId.subdetId()==EcalEndcap){
      EEDetId eeId(seedId);
      return eeId.offsetBy(iEtaOrIX,iPhiOrIY);
    }else{
      return DetId(0);
    }
  }
      
  float getHitEnergy(const DetId& id,const std::unordered_map<int,float>& detIdToEnergy){
    auto entry = detIdToEnergy.find(id.rawId());
    if(entry!=detIdToEnergy.end()){
      return entry->second;
    }else{
      return 0.f;
    }
  };
}

void ScoutShowerShapeStruct::fill(const int seedId,const std::vector<unsigned int>& detIds,const std::vector<float>& nrgys)
{
  std::unordered_map<int,float> detIdToEnergy;
  for(size_t index=0;index<detIds.size();index++){
    detIdToEnergy[detIds[index]] = nrgys[index];
  }
  clear();
  constexpr int arrayOffset = 2;
  std::array<float,5> iEtaOrIXStrips;
  std::array<float,5> iPhiOrIYStrips;
  iEtaOrIXStrips.fill(0.f);
  iPhiOrIYStrips.fill(0.f);
  for(int iEtaOrIXNr=-2;iEtaOrIXNr<=2;iEtaOrIXNr++){
    for(int iPhiOrIYNr=-2;iPhiOrIYNr<=2;iPhiOrIYNr++){
      const DetId& id = getOffsetId(seedId,iEtaOrIXNr,iPhiOrIYNr);
      float energy = getHitEnergy(id,detIdToEnergy);
      if(energy>eMax){
	e2nd = eMax;
	eMax = energy;
      }else if(energy>e2nd){
	e2nd = energy;
      }
      e5x5+=energy;
      iEtaOrIXStrips[iEtaOrIXNr+arrayOffset]+=energy;
      iPhiOrIYStrips[iPhiOrIYNr+arrayOffset]+=energy;
    }
  }
  eLeft = getHitEnergy(getOffsetId(seedId,-1,0),detIdToEnergy);
  eRight = getHitEnergy(getOffsetId(seedId,1,0),detIdToEnergy);
  eTop = getHitEnergy(getOffsetId(seedId,0,1),detIdToEnergy);
  eBottom = getHitEnergy(getOffsetId(seedId,0,-1),detIdToEnergy);
 
  
  e2x5Left = iEtaOrIXStrips[-2+arrayOffset]+iEtaOrIXStrips[-1+arrayOffset];
  e2x5Right = iEtaOrIXStrips[1+arrayOffset]+iEtaOrIXStrips[2+arrayOffset];
  e1x5 = iEtaOrIXStrips[0+arrayOffset];
  e2x5Top = iPhiOrIYStrips[1+arrayOffset] + iPhiOrIYStrips[2+arrayOffset];
  e2x5Bottom = iPhiOrIYStrips[-2+arrayOffset] + iPhiOrIYStrips[-1+arrayOffset];
  e2x5Max = std::max(iEtaOrIXStrips[-1+arrayOffset],iEtaOrIXStrips[1+arrayOffset])+ iEtaOrIXStrips[0+arrayOffset];
  
}
      
{
  
  float safeDivide(float numer,float denom,float zeroVal=0. ){
    return denom!=0 ? numer / denom : zeroVal;
  }
  float ptToP(float pt,float eta){
    return safeDivide(pt,std::sin(MathFuncs::etaToTheta(eta),0));
  }
  int getBestTrkIndx(const Run3ScoutingElectron& ele,int maxTrks=ScoutEleStruct::kMaxTracks){

    int bestIndx = 0;
    float bestAbsEPM1 = 999;
    float eleEnergy = ptToP(ele.pt(),ele.eta());
    for(size_t trkNr=0;trkNr<ele.trkpt().size() && trkNr<maxTrks;trkNr++){
      if(ele.trkpt()[trkNr]==0){
	continue;
      }
      
      float trkP = ptToP(ele.trkpt()[trkNr],ele.trketa()[trkNr]);
      float absEPM1 = std::abs(eleEnergy/trkP-1);
      if(absEPM1<bestAbsEPM1){
	bestAbsEPM1 = absEPM1;
	bestIndx = trkNr;
      }
    }
    return bestIndx;
  }
}

void ScoutEleStruct::fill(const Run3ScoutingElectron& ele){
  pt = ele.pt();
  energy = ptToP(ele.pt(),ele.eta());
  eta = ele.eta();
  phi = ele.phi();
  m = ele.m();
    
  fillArrayFromVector(ele.trkd0(), trkd0, kMaxTracks);
  fillArrayFromVector(ele.trkdz(), trkdz, kMaxTracks);
  fillArrayFromVector(ele.trkpt(), trkpt, kMaxTracks);
  fillArrayFromVector(ele.trketa(), trketa, kMaxTracks);
  fillArrayFromVector(ele.trkphi(), trkphi, kMaxTracks);
  fillArrayFromVector(ele.trkchi2overndf(), trkchi2overndf, kMaxTracks);
  bestTrkIndx = getBestTrkIndx(ele);

  dEtaIn = ele.dEtaIn();
  dPhiIn = ele.dPhiIn();
  sigmaIetaIeta = ele.sigmaIetaIeta();
  hOverE = ele.hOverE();
  ooEMOop = ele.ooEMOop();
  missingHits = ele.missingHits();

  fillArrayFromVector(ele.trkcharge(), trkcharge, kMaxTracks);

  ecalIso = ele.ecalIso();
  hcalIso = ele.hcalIso();
  trackIso = ele.trackIso();
  r9 = ele.r9();
  sMin = std::isnan(ele.sMin()) ? 0.f : ele.sMin();
  sMaj = std::isnan(ele.sMaj()) ? 0.f : ele.sMaj();
  seedId = ele.seedId();
 
  rechitZeroSuppression = ele.rechitZeroSuppression();
  nrTrks = ele.trkd0().size();
  DetId eleSeedId(ele.seedId());
  if(eleSeedId.det()==DetId::Ecal){
    if(eleSeedId.subdetId()==EcalBarrel){
      EBDetId ebId(eleSeedId);
      iEtaOrIX = ebId.ieta();
      iPhiOrIY = ebId.iphi();
      isEB = true;
    }else if(eleSeedId.subdetId()==EcalEndcap){
      EEDetId eeId(eleSeedId);
      iEtaOrIX = eeId.ix();
      iPhiOrIY = eeId.iy();
      isEB = false;
    }

  }else{
    isEB =false;
  }

}

void ScoutEleStruct::clear() {
    pt = 0.0f;
    energy = 0.0f;
    eta = 0.0f;
    phi = 0.0f;
    m = 0.0f;

    std::fill(std::begin(trkd0), std::end(trkd0), 0.0f);
    std::fill(std::begin(trkdz), std::end(trkdz), 0.0f);
    std::fill(std::begin(trkpt), std::end(trkpt), 0.0f);
    std::fill(std::begin(trketa), std::end(trketa), 0.0f);
    std::fill(std::begin(trkphi), std::end(trkphi), 0.0f);
    std::fill(std::begin(trkchi2overndf), std::end(trkchi2overndf), 0.0f);
    std::fill(std::begin(trkcharge), std::end(trkcharge), 0);
    
    dEtaIn = 0.0f;
    dPhiIn = 0.0f;
    sigmaIetaIeta = 0.0f;
    hOverE = 0.0f;
    ooEMOop = 0.0f;
    missingHits = 0;
   
    ecalIso = 0.0f;
    hcalIso = 0.0f;
    trackIso = 0.0f;
    r9 = 0.0f;
    sMin = 0.0f;
    sMaj = 0.0f;
    seedId = 0;
    
    rechitZeroSuppression = false;
    nrTrks=0;
    isEB=0;
    iEtaOrIX=0;
    iPhiOrIY=0;
    bestTrkIndx=0;
}
