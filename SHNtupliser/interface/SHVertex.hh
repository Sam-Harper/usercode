#ifndef SHVERTEX_H
#define SHVERTEX_H

#include "TLorentzVector.h"
#include "TVector3.h"
#include "TObject.h"

namespace reco{
  class Vertex;
}

class SHVertex : public TObject {
private:
  TVector3 pos_;
  int ndof_;
  float chi2_;
  int nrTracks_;
  bool isValid_;
  TLorentzVector sumP4_;

public:
  SHVertex():pos_(),ndof_(-1),chi2_(-1),nrTracks_(-1),isValid_(false),sumP4_(){}
  SHVertex(const reco::Vertex& vertex);


  const TVector3& pos()const{return pos_;}
  int ndof()const{return ndof_;}
  float chi2()const{return chi2_;}
  int nrTracks()const{return nrTracks_;}
  bool isValid()const{return isValid_;}
  const TLorentzVector& sumP4()const{return sumP4_;}
  float sumPt()const{return sumP4_.Pt();}
  
  ClassDef(SHVertex,1)

};


#endif
