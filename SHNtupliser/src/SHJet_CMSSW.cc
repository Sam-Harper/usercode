#include "SHarper/SHNtupliser/interface/SHJet.hh"

#include "DataFormats/PatCandidates/interface/Jet.h"


SHJet::SHJet(const pat::Jet & jet):
  p4_(jet.px(),jet.py(),jet.pz(),jet.energy()),
  hadFrac_(jet.energyFractionHadronic()),
  caloArea_(jet.towersArea()),
  maxNrgyInEmTowers_(jet.maxEInEmTowers()),
  maxNrgyInHadTowers_(jet.maxEInHadTowers()),
  hadNrgyInH0_(jet.hadEnergyInHO()),
  hadNrgyInHB_(jet.hadEnergyInHB()),
  hadNrgyInHF_(jet.hadEnergyInHF()),
  hadNrgyInHE_(jet.hadEnergyInHE()),
  emNrgyInEB_(jet.emEnergyInEB()),
  emNrgyInEE_(jet.emEnergyInEE()),
  emNrgyInHF_(jet.emEnergyInHF()),  
  jetNrgyScale_(jet.corrFactor("ABS")),
  trkCountHighEffBTag_(jet.bDiscriminator("trackCountingHighEffBJetTag")),
  trkCountHighPureBTag_(jet.bDiscriminator("trackCountingHighPureBJetTag")),
  jetBProbBTag_(jet.bDiscriminator("jetBProbabilityBJetTags")),
  jetProbBTag_(jet.bDiscriminator("jetProbabilityBJetTags")),
  partonFlavour_(jet.partonFlavour()) 
{

}
