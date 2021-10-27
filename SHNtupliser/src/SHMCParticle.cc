
#include "SHarper/SHNtupliser/interface/SHMCParticle.hh"
#include "SHarper/SHNtupliser/interface/MathFuncs.hh"
#include<iomanip>
#include<iostream>

ClassImp(SHMCParticle)


SHMCParticle::SHMCParticle():
  index_(0),
  stdhep_(0),
  idhep_(0),
  jmo1_(0),
  jmo2_(0),
  nrMo_(0),
  jda1_(0),
  jda2_(0),
  nrDa_(0)
{

}

SHMCParticle::SHMCParticle(int partIndx,int partStdhep,int partIdhep,
			   int partJmo1,int partJmo2,int partNrMo,
			   int partJda1,int partJda2,int partNrDa,
			   const TLorentzVector& p4,const TVector3& pos):
  pt_(p4.Pt()),
  eta_(p4.Pt() > 0 ? p4.Eta() : std::numeric_limits<float>::max()),
  phi_(p4.Phi()),
  mass_(p4.Mag()),
  nrgy_(p4.Pt()!=0. ? 0. : p4.E()),
  vx_(pos.X()),
  vy_(pos.Y()),
  vz_(pos.Z()),
  index_(partIndx),
  stdhep_(partStdhep),
  idhep_(partIdhep),
  jmo1_(partJmo1),
  jmo2_(partJmo2),
  nrMo_(partNrMo),
  jda1_(partJda1),
  jda2_(partJda2),
  nrDa_(partNrDa)
{
 
}


SHMCParticle::SHMCParticle(const SHMCParticle& rhs):TObject(rhs),
  pt_(rhs.pt_),
  eta_(rhs.eta_),
  phi_(rhs.phi_),
  mass_(rhs.mass_),
  nrgy_(rhs.nrgy_),
  vx_(rhs.vx_),
  vy_(rhs.vy_),
  vz_(rhs.vz_),
  index_(rhs.index_),
  stdhep_(rhs.stdhep_),
  idhep_(rhs.idhep_),
  jmo1_(rhs.jmo1_),
  jmo2_(rhs.jmo2_),
  nrMo_(rhs.nrMo_),
  jda1_(rhs.jda1_),
  jda2_(rhs.jda2_),
  nrDa_(rhs.nrDa_)
{

}

float SHMCParticle::detEta()const
{
  return MathFuncs::detEtaFromEvnt(p4().Eta(),vz_);
}

std::ostream& SHMCParticle::print(std::ostream& output)const
{
  output<< std::setw(6) << index_ << std::setw(8) << stdhep_ 
	<< std::setw(6) << idhep_ << std::setw(6) << jmo1_ << std::setw(6)
	<< jmo2_ << std::setw(6) << jda1_ << std::setw(6) << jda2_ 
	<< std::setw(10) << std::setprecision(5) << p4().Px() << std::setw(10) << std::setprecision(5) << p4().Py() 
	<< std::setw(10) << std::setprecision(5) << p4().Pz() << std::setw(10) << std::setprecision(5) << p4().E() 
	<< std::setw(10) << std::setprecision(5) << p4().Pt() << std::setw(10) << std::setprecision(5) << p4().Eta() << std::setw(10) << std::setprecision(5) << p4().Phi() << std::setw(15)<<p4().Mag();

  return output;
}



std::ostream& operator <<(std::ostream& output,const SHMCParticle &mcPart)
{
  return mcPart.print(output);
}

