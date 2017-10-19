#include "SHarper/SHNtupliser/interface/SHJet.hh"

#include "DataFormats/PatCandidates/interface/Jet.h"

SHJet::SHJet(const pat::Jet & jet):
  pt_(jet.pt()),eta_(jet.eta()),phi_(jet.phi()),
  nrgy_(jet.energy()),
  invNrgyCorr_(jet.jecFactor(0)), //this assumes the jet is already corrected, 3 if its not
  neutralHadNrgy_(jet.neutralHadronEnergy()),
  neutralEmNrgy_(jet.neutralEmEnergy()),
  chargedHadNrgy_(jet.chargedHadronEnergy()),
  chargedEmNrgy_(jet.chargedEmEnergy()),
  muonNrgy_(jet.muonEnergy()),
  chargedMultiplicity_(convertMultiplicity(jet.chargedMultiplicity())),
  neutralMultiplicity_(convertMultiplicity(jet.neutralMultiplicity())),
  combinedSecVtxBTag_(jet.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags")),
  combinedMVABTag_(jet.bDiscriminator("pfCombinedMVAV2BJetTags")),
  partonFlavour_(convertFlavour(jet.partonFlavour())),
  hadronFlavour_(convertFlavour(jet.hadronFlavour()))
{
  // std::cout <<"jet energy "<<jet.energy()<<" jet corr energy "<<jet.correctedP4(3).E()<<" uncorr "<<unCorrNrgy_<<std::endl;
  //if(jet.chargedEmEnergyFraction()>0) std::cout <<"jet test reco "<<jet.chargedEmEnergyFraction()<<" ntup : "<<chargedEmNrgy_/(nrgy_*invNrgyCorr_)<<" corr "<<1./invNrgyCorr_<<std::endl;
}


