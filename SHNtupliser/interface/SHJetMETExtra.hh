#ifndef SHEVENT_SHJETMETEXTRA_HH
#define SHEVENT_SHJETMETEXTRA_HH

#include "SHarper/SHNtupliser/interface/SHJet.hh"
#include "SHarper/SHNtupliser/interface/SHMet.hh"

#include "Rtypes.h"

#include <vector>

class SHJetMETExtra {
private:
  std::vector<std::vector<SHJet> > jetColls_;
  std::vector<SHMet> mets_;

  std::vector<std::string> jetCollNames_;
  std::vector<std::string> metNames_;

  const SHMet dummyMET;
  const std::vector<SHJet> dummyJets;
  
public:
  SHJetMETExtra(){}
  virtual ~SHJetMETExtra(){}

  const std::vector<std::vector<SHJet> >& jetColls()const{return jetColls_;}
  const std::vector<SHMet>& mets()const{return mets_;}

  const std::vector<std::string>& jetCollNames()const{return jetCollNames_;}  
  const std::vector<std::string>& metNames()const{return metNames_;}

  size_t nrJetColls()const{return jetColls_.size();}
  size_t nrMETColls()const{return mets_.size();}
 
  const std::vector<SHJet>& jets(size_t collNr)const{return jetColls_[collNr];}
  const SHMet& met(size_t collNr)const{return mets_[collNr];}

  const std::vector<SHJet>& jets(const std::string& collName)const;
  const SHMet& met(const std::string& collName)const;

  void clear();
  
  void setJets(std::vector<std::vector<SHJet> > jets,std::vector<std::string> names){
    jetColls_ = std::move(jets);
    jetCollNames_=std::move(names);
  }
  void setMETs(std::vector<SHMet> mets,std::vector<std::string> names){
    mets_ = std::move(mets);
    metNames_=std::move(names);
  }
  
  ClassDef(SHJetMETExtra,1) 
};

#endif
