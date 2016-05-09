#ifndef Sharper_TrigTools_TrigToolsStructs_h
#define Sharper_TrigTools_TrigToolsStructs_h
#include "DataFormats/Math/interface/LorentzVector.h"
#include "TLorentzVector.h"
#include <string>
#include <vector>
#include <set>

namespace edm {
  class Event;
}

namespace trigtools{

//   typedef std::bitset<500> TrigBits; //my one typedef as I want to be able to change the number of bits easily

//   class TrigBitsDef{
//   public:
//     std::vector<std::string> bitNames_;
    
    

//   };

  class HLTPathInfo{
  public:
    class FilterInfo {
    private:
      std::string filterName_;
      std::vector<TLorentzVector> objP4s_;
    };

  private:
    std::string pathName_;
    bool pass_;
    bool wasRun_;
    int preScale_;

    std::vector<FilterInfo> filters_;
  };


  struct P4Struct {
    float et,nrgy,pt,eta,phi;
    P4Struct(){}
    P4Struct(const TLorentzVector& p4){fill(p4);}
    static std::string contents(){return "et/F:nrgy:pt:eta:phi";}
    void fill(const TLorentzVector&p4){
      et=p4.Et();pt=p4.Pt();nrgy=p4.E();eta=p4.Eta();phi=p4.Phi();
    }
    void fill(const math::XYZTLorentzVector&p4){
      et=p4.Et();pt=p4.Pt();nrgy=p4.E();eta=p4.Eta();phi=p4.Phi();
    }
    
    P4Struct& operator=(float val){et=val;nrgy=val;eta=val;phi=val;return *this;}
    P4Struct& operator=(int val){et=val;nrgy=val;eta=val;phi=val;return *this;}
      P4Struct& operator=(const TLorentzVector&p4){fill(p4);return *this;}
      P4Struct& operator=(const math::XYZTLorentzVector&p4){fill(p4);return *this;}
    void clear(){*this=-999;}
  };
  
  struct PathData {
    float nrPass;
    float sumE;
    float sumEt;
    float sumHT;
    float sumMHT;
    P4Struct p4First;
    P4Struct p4Second;
    P4Struct p4Third;
    P4Struct p4Fourth;
    P4Struct p4Fifth;
    P4Struct p4Sixth;
    P4Struct p4Seventh;
    P4Struct p4Eighth;
    P4Struct p4FirstEB;
    P4Struct p4SecondEB;
    P4Struct p4FirstEE;
    P4Struct p4SecondEE;
    
    static std::string contents(){return "nrPass/F:sumE:sumEt:sumHT:sumMHT:et1:nrgy1:pt1:eta1:phi1:et2:nrgy2:pt2:eta2:phi2:et3:nrgy3:pt3:eta3:phi3:et4:nrgy4:pt4:eta4:phi4:et5:nrgy5:pt5:eta5:phi5:et6:nrgy6:pt6:eta6:phi6:et7:nrgy7:pt7:eta7:phi7:et8:nrgy8:pt8:eta8:phi8:et1EB:nrgy1EB:pt1EB:eta1EB:phi1EB:et2EB:nrgy2EB:pt2EB:eta2EB:phi2EB:et1EE:nrgy1EE:pt1EE:eta1EE:phi1EE:et2EE:nrgy2EE:pt2EE:eta2EE:phi2EE";}
    
    void fill(const std::vector<TLorentzVector>& trigP4s);
  };
  
  
  
  struct EtP4Sorter {
    bool operator()(const TLorentzVector&lhs,const TLorentzVector& rhs){return lhs.Et()<rhs.Et();}
    
  };

  struct EvtInfoStruct {
    int runnr,lumiSec,eventnr,bx;
    void fill(const edm::Event& event);
    static std::string contents(){return "runnr/I:lumiSec:eventnr:bx";}
  };

  struct TowerStruct {
    float emNrgy,hadNrgy,iEta,iPhi;

  };

}

#endif
