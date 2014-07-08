#include "SHarper/SHNtupliser/interface/SHEvent.hh"
#include "SHarper/SHNtupliser/interface/SHCaloHit.hh"
#include "SHarper/SHNtupliser/interface/SHBasicCluster.hh"
#include "SHarper/SHNtupliser/interface/SHSuperCluster.hh"
#include "SHarper/SHNtupliser/interface/SHElectron.hh"
#include "SHarper/SHNtupliser/interface/SHMCParticle.hh"
#include "SHarper/SHNtupliser/interface/SHCaloHitContainer.hh"
#include "SHarper/SHNtupliser/interface/SHJet.hh"
#include "SHarper/SHNtupliser/interface/SHIsolCluster.hh"
#include "SHarper/SHNtupliser/interface/SHIsolSuperCluster.hh"
#include "SHarper/SHNtupliser/interface/SHIsolTrack.hh"
#include "SHarper/SHNtupliser/interface/SHTrigInfo.hh"
#include "SHarper/SHNtupliser/interface/SHMet.hh"
#include "SHarper/SHNtupliser/interface/SHCaloGeom.hh"
#include "SHarper/SHNtupliser/interface/SHCaloCellGeom.hh"
#include "SHarper/SHNtupliser/interface/SHL1Cand.hh"
#include "SHarper/SHNtupliser/interface/SHMuon.hh"
#include "SHarper/SHNtupliser/interface/SHTrigEcalCand.hh"
#include "SHarper/SHNtupliser/interface/SHTrigObjContainer.hh"
#include "SHarper/SHNtupliser/interface/SHPileUpInfo.hh"
#include "SHarper/SHNtupliser/interface/SHPileUpSummary.hh"
#include "SHarper/SHNtupliser/interface/SHCaloTowerContainer.hh"
#include "SHarper/SHNtupliser/interface/SHCaloTower.hh"
#include "SHarper/SHNtupliser/interface/SHPFCandidate.hh"
#include "SHarper/SHNtupliser/interface/SHPFCandContainer.hh"
#include "SHarper/SHNtupliser/interface/SHVertex.hh"
#include "SHarper/SHNtupliser/interface/SHPreShowerCluster.hh"
#include "SHarper/SHNtupliser/interface/SHL1Cluster.hh"
#include "SHarper/SHNtupliser/interface/SHL1Event.hh"
#include "SHarper/SHNtupliser/interface/SHL1CaloTowerContainer.hh"
#include "SHarper/SHNtupliser/interface/SHL1CaloTower.hh"
#include "SHarper/SHNtupliser/interface/SHPFCluster.hh"
#include "SHarper/SHNtupliser/interface/SHPFClusterContainer.hh"


#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class SHEvent+;

#pragma link C++ class SHCaloHit+;
#pragma link C++ class SHBasicCluster+;
#pragma link C++ class SHSuperCluster+; 
#pragma link C++ class SHElectron+;
#pragma link C++ class SHMCParticle+; 

                           
#pragma link C++ class SHCaloHitContainer+;
#pragma link C++ class SHJet+;
#pragma link C++ class SHIsolCluster+;
#pragma link C++ class SHIsolSuperCluster+; 
#pragma link C++ class SHIsolTrack+; 
#pragma link C++ class SHTrigInfo+;
#pragma link C++ class SHMet+;
#pragma link C++ class SHCaloGeom+;
#pragma link C++ class SHCaloCellGeom+; 
#pragma link C++ class SHCaloCellGeom::CellEdges+; 
#pragma link C++ class SHL1Cand+; 
#pragma link C++ class SHMuon+;
#pragma link C++ class SHMuon::MuIsolData+;
#pragma link C++ class SHTrigEcalCand+;
#pragma link C++ class SHTrigObjContainer+;
#pragma link C++ class SHPileUpSummary+;
#pragma link C++ class SHPileUpInfo+;
#pragma link C++ class SHCaloTower+;
#pragma link C++ class SHCaloTowerContainer+;
#pragma link C++ class SHPFCandidate+;
#pragma link C++ class SHPFCandContainer+;
#pragma link C++ class SHVertex+;
#pragma link C++ class SHPreShowerCluster+;
#pragma link C++ class SHL1Cluster+;
#pragma link C++ class SHL1Event+;
#pragma link C++ class SHL1CaloTower;
#pragma link C++ class SHL1CaloTowerContainer;
#pragma link C++ class SHPFCluster+;
#pragma link C++ class SHPFClusterContainer+;
#endif
