
#include "SHarper/SHNtupliser/interface/SHMCParticle.hh"

#include<iomanip>

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
  nrDa_(0),
  fourMomentum_(0.0001,0.,0.,0.),
  pos_(0.0001,0.,0)
{

}

SHMCParticle::SHMCParticle(int partIndx,int partStdhep,int partIdhep,
			   int partJmo1,int partJmo2,int partNrMo,
			   int partJda1,int partJda2,int partNrDa,
			   const TLorentzVector& p4,const TVector3& pos):
  index_(partIndx),
  stdhep_(partStdhep),
  idhep_(partIdhep),
  jmo1_(partJmo1),
  jmo2_(partJmo2),
  nrMo_(partNrMo),
  jda1_(partJda1),
  jda2_(partJda2),
  nrDa_(partNrDa),
  fourMomentum_(p4),
  pos_(pos)
{

}


SHMCParticle::SHMCParticle(const SHMCParticle& rhs):
  index_(rhs.index_),
  stdhep_(rhs.stdhep_),
  idhep_(rhs.idhep_),
  jmo1_(rhs.jmo1_),
  jmo2_(rhs.jmo2_),
  nrMo_(rhs.nrMo_),
  jda1_(rhs.jda1_),
  jda2_(rhs.jda2_),
  nrDa_(rhs.nrDa_),
  fourMomentum_(rhs.fourMomentum_),
  pos_(rhs.pos_)
{

}

std::ostream& SHMCParticle::print(std::ostream& output)const
{
  output<< std::setw(6) << index_ << std::setw(8) << stdhep_ 
	<< std::setw(6) << idhep_ << std::setw(6) << jmo1_ << std::setw(6)
	<< jmo2_ << std::setw(6) << jda1_ << std::setw(6) << jda2_ 
	<< std::setw(10) << std::setprecision(5) << fourMomentum_.Px() << std::setw(10) << std::setprecision(5) << fourMomentum_.Py() 
	<< std::setw(10) << std::setprecision(5) << fourMomentum_.Pz() << std::setw(10) << std::setprecision(5) << fourMomentum_.E() 
	<< std::setw(10) << std::setprecision(5) << fourMomentum_.Pt() << std::setw(15)<<fourMomentum_.Mag();
  return output;
}



std::ostream& operator <<(std::ostream& output,const SHMCParticle &mcPart)
{
  return mcPart.print(output);
}

