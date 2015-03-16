#ifndef SHARPER_SHNTUPLISER_PFFUNCS_H
#define SHARPER_SHNTUPLISER_PFFUNCS_H

#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/Common/interface/Handle.h"
class SHEvent;
class SHPFCandContainer;

namespace reco{
  class PFBlockElementCluster;
}

class PFFuncs {
  PFFuncs()=delete;

public:
  static bool passesCleaning(const reco::PFCandidateRef& pfCand,const reco::GsfElectron& ele);
  static bool passesCleaningPhoton(const reco::PFCandidateRef& pfCand,const reco::GsfElectron& ele);
  static bool passesCleaningNeutralHadron(const reco::PFCandidateRef&,const reco::GsfElectron&);
  static bool passesCleaningChargedHadron(const reco::PFCandidateRef&,const reco::GsfElectron&);
  static std::vector<reco::PFCandidateRef> cleanPFIsolMap(const std::vector<reco::PFCandidateRef> &,
							  const reco::GsfElectron&);
  static const reco::PFBlockElementCluster* getHighestEtECALCluster(const reco::PFCandidate& pfCand);
  static int getSeedCrysIdOfPFCandSC(const reco::PFCandidateRef pfCandRef,
				     const edm::ValueMap<std::vector<reco::PFCandidateRef> > & gsfToPFMap,
				     const edm::Handle<std::vector<reco::GsfElectron> >& eleHandle);
  
  
  static void fillPFCands(const SHEvent* event,double maxDR,SHPFCandContainer& shPFCands, 
			  const edm::Handle<std::vector<reco::PFCandidate> >& pfCands,
			  const reco::VertexRef mainVtx,const edm::Handle<reco::VertexCollection> vertices,
			  const edm::ValueMap<std::vector<reco::PFCandidateRef> > & gsfToPFMap,
			  const edm::Handle<std::vector<reco::GsfElectron> >& eleHandle);
  static bool isPhoton(const reco::PFCandidateRef& pfCand);
  static bool isNeutralHadron(const reco::PFCandidateRef& pfCand);
  static bool isChargedHadron(const reco::PFCandidateRef& pfCand);
  static int chargedHadronVertex(const reco::PFCandidate& pfcand, const reco::VertexCollection& vertices);


};

#endif
