#ifndef SHMCPARTICLE
#define SHMCPARTICLE

//stores a Monte Carlo Particle, just p4 and basic truth info

#include "TObject.h"
#include "TLorentzVector.h"



class SHMCParticle: public TObject {

private:
  int index_;
  int stdhep_;
  int idhep_;//pdg id 
  int jmo1_;
  int jmo2_;
  int nrMo_;
  int jda1_;
  int jda2_;
  int nrDa_;
  TLorentzVector fourMomentum_;
 
public:
  SHMCParticle();
  SHMCParticle(int partIndx,int partStdHep,int partIdHep,
	       int partJmo1,int partJmo2,int partNrMo,
	       int partJda1,int partJda2,int partNrDa,
	       TLorentzVector& p4);
  SHMCParticle(const SHMCParticle& rhs);
  virtual ~SHMCParticle(){}
  friend std::ostream &operator <<(std::ostream& output,const SHMCParticle &mcPart);
  std::ostream &print(std::ostream& output)const;
  // std::ostream &print()const ;
  int index()const{return index_;}
  int stdhep()const{return stdhep_;}
  int idhep()const{return idhep_;}
  int pid()const{return idhep_;}
  int jmo1()const{return jmo1_;}
  int jmo2()const{return jmo2_;}
  int jda1()const{return jda1_;}
  int jda2()const{return jda2_;}
  const TLorentzVector& p4()const{return fourMomentum_;}
  double nrgy()const{return fourMomentum_.E();}
  double mass()const{return fourMomentum_.Mag();}
 

  ClassDef(SHMCParticle,1)

};

#endif
