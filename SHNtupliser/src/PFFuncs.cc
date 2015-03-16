#include "SHarper/SHNtupliser/interface/PFFuncs.h"

#include "SHarper/SHNtupliser/interface/SHEvent.hh"
#include "SHarper/SHNtupliser/interface/SHPFCandContainer.hh"

#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockElement.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockElementCluster.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlock.h"
#include "DataFormats/ParticleFlowReco/interface/PFCluster.h"
#include "DataFormats/Math/interface/deltaR.h"
#include <iomanip>

bool PFFuncs::passesCleaning(const reco::PFCandidateRef& pfCand,const reco::GsfElectron& ele)
{
  if(pfCand->particleId()==reco::PFCandidate::h){
    return passesCleaningChargedHadron(pfCand,ele);
  }else if(pfCand->particleId()==reco::PFCandidate::h0){
    return passesCleaningNeutralHadron(pfCand,ele);
  }else if(pfCand->particleId()==reco::PFCandidate::gamma){
    return passesCleaningPhoton(pfCand,ele);
  }else return true;
}

bool PFFuncs::passesCleaningPhoton(const reco::PFCandidateRef& pfCand,const reco::GsfElectron& ele)
{
   
  if(pfCand->superClusterRef()==ele.superCluster()) return false;
  
  const reco::PFBlockElementCluster* ecalClusWithMaxEt = getHighestEtECALCluster(*pfCand);
  if(ecalClusWithMaxEt){
    if(ecalClusWithMaxEt->superClusterRef()==ele.superCluster()) return false;

    float clusEta = ecalClusWithMaxEt->clusterRef()->position().Eta();
    float clusPhi = ecalClusWithMaxEt->clusterRef()->position().Phi();
    for(auto cluster : ele.superCluster()->clusters()){
      if(reco::deltaR2(cluster->eta(),cluster->phi(),clusEta,clusPhi)<0.00008) return false; //about half an ecal crystal in barrel
    }
  }
  return true;
}

bool PFFuncs::passesCleaningNeutralHadron(const reco::PFCandidateRef&,const reco::GsfElectron&)
{
  return true;
}

bool PFFuncs::passesCleaningChargedHadron(const reco::PFCandidateRef&,const reco::GsfElectron&)
{
  return true;
}

std::vector<reco::PFCandidateRef> PFFuncs::cleanPFIsolMap(const std::vector<reco::PFCandidateRef> &elePFCands,
							  const reco::GsfElectron& ele)
{
  std::vector<reco::PFCandidateRef> elePFCandsClean;
  elePFCandsClean.reserve(elePFCands.size());
  for(const auto& pfCand : elePFCands){
    bool accept = passesCleaning(pfCand,ele);
    if(accept) elePFCandsClean.push_back(pfCand);
  }
  return elePFCandsClean;
}

const reco::PFBlockElementCluster* PFFuncs::getHighestEtECALCluster(const reco::PFCandidate& pfCand)
{
  float maxECALEt =-1;
  const reco::PFBlockElement* maxEtECALCluster=nullptr;
  const reco::PFCandidate::ElementsInBlocks& elementsInPFCand = pfCand.elementsInBlocks();
  for(auto& elemIndx : elementsInPFCand){
    const reco::PFBlockElement* elem = elemIndx.second<elemIndx.first->elements().size() ? &elemIndx.first->elements()[elemIndx.second] : nullptr;
    if(elem && elem->type()==reco::PFBlockElement::ECAL && elem->clusterRef()->pt()>maxECALEt){
      maxECALEt = elem->clusterRef()->pt();
      maxEtECALCluster = elem;
    }
    
  }
  return dynamic_cast<const reco::PFBlockElementCluster*>(maxEtECALCluster);
	
}


int PFFuncs::getSeedCrysIdOfPFCandSC(const reco::PFCandidateRef pfCandRef,
				     const edm::ValueMap<std::vector<reco::PFCandidateRef> > & gsfToPFMap,
				     const edm::Handle<std::vector<reco::GsfElectron> >& eleHandle)

{
  for(size_t eleNr=0;eleNr<eleHandle->size();eleNr++){
    reco::GsfElectronRef ele(eleHandle,eleNr);
    const std::vector<reco::PFCandidateRef>& elePFCands =  gsfToPFMap[ele];
  
    for(size_t candNr=0;candNr<elePFCands.size();candNr++){
     
      if(&(*pfCandRef)==&(*elePFCands[candNr])){
	if(ele->superCluster().isNonnull()) return ele->superCluster()->seed()->seed().rawId();
	return -1;
      }
    }
  }
  return 0;

}



void PFFuncs::fillPFCands(const SHEvent* event,double maxDR,SHPFCandContainer& shPFCands,
			  const edm::Handle<std::vector<reco::PFCandidate> >& pfCands,
			  const reco::VertexRef mainVtx,const edm::Handle<reco::VertexCollection> vertices,
			  const edm::ValueMap<std::vector<reco::PFCandidateRef> > & gsfToPFMap,
			  const edm::Handle<std::vector<reco::GsfElectron> >& eleHandle)
{
 

  edm::ValueMap<std::vector<reco::PFCandidateRef>> gsfToPFMapCleaned;
  edm::ValueMap<std::vector<reco::PFCandidateRef>>::Filler mapFiller(gsfToPFMapCleaned);
  std::vector<std::vector<reco::PFCandidateRef>> pfCandsFromEles;
  for(size_t eleNr=0;eleNr<eleHandle->size();eleNr++){
    reco::GsfElectronRef ele(eleHandle,eleNr);
    const std::vector<reco::PFCandidateRef>& elePFCands =  gsfToPFMap[ele];
    pfCandsFromEles.push_back(cleanPFIsolMap(elePFCands,*ele));
  }
  mapFiller.insert(eleHandle,pfCandsFromEles.begin(),pfCandsFromEles.end());
  mapFiller.fill();
  
  const auto& gsfToPFMapToUse =gsfToPFMapCleaned;

  const double maxDR2 = maxDR*maxDR;
  std::vector<std::pair<float,float> > eleEtaPhi;
  for(int eleNr=0;eleNr<event->nrElectrons();eleNr++){
    const SHElectron* ele = event->getElectron(eleNr);
    if(ele->et()>20){
      eleEtaPhi.push_back(std::make_pair(ele->detEta(),ele->detPhi()));
    }
  }

  for(size_t candNr=0;candNr<pfCands->size();candNr++){ 
    const reco::PFCandidateRef pfCandRef(pfCands,candNr);
    const reco::PFCandidate& pfParticle = *pfCandRef;
    int scSeedCrysId=getSeedCrysIdOfPFCandSC(pfCandRef,gsfToPFMapToUse,eleHandle);
    bool accept =false;
    for(size_t eleNr=0;eleNr<eleEtaPhi.size();eleNr++){
      if(MathFuncs::calDeltaR2(eleEtaPhi[eleNr].first,eleEtaPhi[eleNr].second,
			       pfParticle.eta(),pfParticle.phi())<maxDR2){
	accept=true;
	break;
      }
    }//end ele loop
  
   
    if(accept){
      if(isPhoton(pfCandRef)){
       	shPFCands.addPhoton(pfParticle.pt(),pfParticle.eta(),pfParticle.phi(),pfParticle.mass(),pfParticle.mva_nothing_gamma(),scSeedCrysId);
      }else if(isNeutralHadron(pfCandRef)){
	shPFCands.addNeutralHad(pfParticle.pt(),pfParticle.eta(),pfParticle.phi(),pfParticle.mass(),pfParticle.mva_nothing_gamma(),scSeedCrysId);
      }else if(isChargedHadron(pfCandRef)){
	int pfCandVtx= chargedHadronVertex(pfParticle,*vertices.product());
	if(pfCandVtx==-1 || pfCandVtx==0){
	
	  SHPFCandidate& shPFCand =shPFCands.addChargedHad(pfParticle.pt(),pfParticle.eta(),pfParticle.phi(),pfParticle.mass(),pfParticle.mva_nothing_gamma(),scSeedCrysId);
	  shPFCand.setVertex(0,0,0);
	  //shPFCand.setVertex(pfCandVtx->x(),pfCandVtx->y(),pfCandVtx->z());
	}
      }
    }	 
  }
}

bool PFFuncs::isPhoton(const reco::PFCandidateRef& pfCand)
{
  const int pfParticleIDAbs=std::abs(pfCand->pdgId()); 
  if(pfParticleIDAbs==22) return true;
  else return false;
}

bool PFFuncs::isNeutralHadron(const reco::PFCandidateRef& pfCand)
{
  const int pfParticleIDAbs=std::abs(pfCand->pdgId()); 
  if(pfParticleIDAbs==130 ||
     pfParticleIDAbs==111 ||
     pfParticleIDAbs==310 ||
     pfParticleIDAbs==2112) return true;
  else return false;
}

bool PFFuncs::isChargedHadron(const reco::PFCandidateRef& pfCand)
{
  const int pfParticleIDAbs=std::abs(pfCand->pdgId()); 
  if(pfParticleIDAbs == 211 ||
     pfParticleIDAbs == 321 ||
     pfParticleIDAbs == 999211 ||
     pfParticleIDAbs == 2212) return true;
  else return false;
}

//stolen from PFPileUpAlgo.cc
int PFFuncs::chargedHadronVertex(  const reco::PFCandidate& pfcand, const reco::VertexCollection& vertices ) {

  auto const & track = pfcand.trackRef();  
  size_t  iVertex = 0;
  unsigned int index=0;
  unsigned int nFoundVertex = 0;
  float bestweight=0;
  for( auto const & vtx : vertices) {
      float w = vtx.trackWeight(track);
     //select the vertex for which the track has the highest weight
 	if (w > bestweight){
	  bestweight=w;
	  iVertex=index;
	  nFoundVertex++;
	}
     ++index;
  }

  if (nFoundVertex>0){
  
    return iVertex;
  }
  // no vertex found with this track. 

  // optional: as a secondary solution, associate the closest vertex in z
  bool checkClosestZVertex=true;
  if ( checkClosestZVertex ) {

    double dzmin = 10000;
    double ztrack = pfcand.vertex().z();
    bool foundVertex = false;
    index = 0;
    for(auto iv=vertices.begin(); iv!=vertices.end(); ++iv, ++index) {

      double dz = fabs(ztrack - iv->z());
      if(dz<dzmin) {
	dzmin = dz; 
	iVertex = index;
	foundVertex = true;
      }
    }

    if( foundVertex ) 
      return iVertex;  

  }


  return -1 ;
}
