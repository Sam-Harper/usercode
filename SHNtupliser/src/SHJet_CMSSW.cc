#include "SHarper/SHNtupliser/interface/SHJet.hh"

#include "DataFormats/PatCandidates/interface/Jet.h"

SHJet::SHJet(const pat::Jet & jet):
  p4_(jet.px(),jet.py(),jet.pz(),jet.energy()),
  hadFrac_(jet.isCaloJet() ? jet.energyFractionHadronic() : -1),
  caloArea_(jet.isCaloJet() ? jet.towersArea(): -1),
  maxNrgyInEmTowers_(jet.isCaloJet() ?jet.maxEInEmTowers() : -1),
  maxNrgyInHadTowers_(jet.isCaloJet() ? jet.maxEInHadTowers() : -1),
  hadNrgyInH0_(jet.isCaloJet() ? jet.hadEnergyInHO(): -1),
  hadNrgyInHB_(jet.isCaloJet() ? jet.hadEnergyInHB(): -1),
  hadNrgyInHF_(jet.isCaloJet() ? jet.hadEnergyInHF(): -1),
  hadNrgyInHE_(jet.isCaloJet() ? jet.hadEnergyInHE(): -1),
  emNrgyInEB_(jet.isCaloJet() ?jet.emEnergyInEB(): -1),
  emNrgyInEE_(jet.isCaloJet() ?jet.emEnergyInEE(): -1),
  emNrgyInHF_(jet.isCaloJet() ? jet.emEnergyInHF(): -1),  
  jetNrgyScale_(jet.jecFactor(3)), 
  //jetNrgyScale_(-1),
  trkCountHighEffBTag_(jet.bDiscriminator("trackCountingHighEffBJetTag")),
  trkCountHighPureBTag_(jet.bDiscriminator("trackCountingHighPureBJetTag")),
  jetBProbBTag_(jet.bDiscriminator("jetBProbabilityBJetTags")),
  jetProbBTag_(jet.bDiscriminator("jetProbabilityBJetTags")),
  partonFlavour_(jet.partonFlavour()),
  neutralHadFrac_(jet.isPFJet() ? jet.neutralHadronEnergyFraction() : -1 ), //new for v3, pf id
  neutralEmFrac_(jet.isPFJet() ? jet.neutralEmEnergyFraction() : -1 ),
  nrConstituents_(jet.isPFJet() ? jet.numberOfDaughters() : -1 ),
  chargedHadFrac_(jet.isPFJet() ? jet.chargedHadronEnergyFraction() : -1 ),
  chargedMultiplicty_(jet.isPFJet() ? jet.chargedMultiplicity() : -1 ),
  muonMultiplicity_(jet.isPFJet() ? jet.muonMultiplicity() : -1 ),
  chargedEmFrac_(jet.isPFJet() ? jet.chargedEmEnergyFraction() : -1 ),
  unCorrNrgy_(jet.jecFactor(0)*jet.energy()),
  jetType_(jet.isCaloJet() ? 1 : 2)
{
  // std::cout <<"jet energy "<<jet.energy()<<" jet corr energy "<<jet.correctedP4(3).E()<<" uncorr "<<unCorrNrgy_<<std::endl;
}


