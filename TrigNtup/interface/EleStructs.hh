#ifndef ELESTRUCTS
#define ELESTRUCTS

//defines my simple data structs, these are meant to be stored in TTrees as structs not classes and dont need to be known to be used
//all the datamembers are intentionally the same type to ensure that they are contingious in memory

class SHElectron;
class SHEvent;
class SHTrigObj;

#include "TVector3.h"
#include "TLorentzVector.h"

#include <string>
namespace EleStructs {

  struct EleSimpleStruct {
  public:
    
    float nrgy,preShowerNrgy,et,clusEt,trkPt,eta,phi,detEta,detPhi,nrClus,nrCryst,q,type,region,epIn,dEtaIn,dPhiIn,hadem,sigmaIEtaIEta,e1x5Over5x5,e2x5Over5x5,e5x5,isolEm,isolHadDepth1,isolHadDepth2,isolPtTrks,eMaxPhi,eMaxEta,nrMissHits,dxy,passPFPreSel,dEtaInSeed,dzTrkVtx,z0;
    static std::string contents(){return "nrgy/F:preShowerNrgy:et:clusEt:trkPt:eta:phi:detEta:detPhi:nrClus:nrCryst:q:type:region:epIn:dEtaIn:dPhiIn:hadem:sigmaIEtaIEta:e1x5Over5x5:e2x5Over5x5:e5x5:isolEm:isolHadDepth1:isolHadDepth2:isolPtTrks:eMaxPhi:eMaxEta:nrMissHits:dxy:passPFPreSel:dEtaInSeed:dzTrkVtx:z0";}
    void clear(){nrgy=preShowerNrgy=et=clusEt=trkPt=eta=phi=detEta=detPhi=nrClus=nrCryst=q=type=region=epIn=dEtaIn=dPhiIn=hadem=sigmaIEtaIEta=e1x5Over5x5=e2x5Over5x5=e5x5=isolEm=isolHadDepth1=isolHadDepth2=isolPtTrks=eMaxPhi=eMaxEta=nrMissHits=dxy=passPFPreSel=dEtaInSeed=dzTrkVtx=z0=-999;}
    void fill(const SHElectron& ele); 
  };

  struct XYZStruct {
    float x,y,z;
    static std::string contents(){return "x/F:y:z";}
    void fill(TVector3 pos){x=pos.X();y=pos.Y();z=pos.Z();}
    void fill(float ix,float iy,float iz){x=ix;y=iy;z=iz;}
    void clear(){x=y=z=0.;}
  };

  struct P4Struct {
    float nrgy,et,eta,phi,pt;
    P4Struct(){clear();}
    void fill(float pt,float eta,float phi,float mass){
      TLorentzVector p4;
      p4.SetPtEtaPhiM(et,eta,phi,mass);
      fill(p4);
    }
    void fill(const TLorentzVector& p4){
      if(p4.Pt()>0.001 && p4.Et()>0.001){
	nrgy = p4.E();
	et = p4.Et();
	eta = p4.Eta();
	phi = p4.Phi();
	pt =p4.Pt();
      }else clear();
    }
    bool operator<(const P4Struct& rhs)const{return pt<rhs.pt;}
    bool operator>(const P4Struct& rhs)const{return pt>rhs.pt;}
    void clear(){nrgy=-999;et=-999;eta=-999;phi=-0;pt=-999;}
    static std::string contents(){return "nrgy/F:et:eta:phi:pt";}
  };


  struct P4DetEtaStruct {
    float nrgy,et,eta,phi,pt,detEta;
    void fill(const TLorentzVector& p4,float iDetEta){
      if(p4.Pt()>0.001 && p4.Et()>0.001){
	nrgy = p4.E();
	et = p4.Et();
	eta = p4.Eta();
	phi = p4.Phi();
	pt =p4.Pt();
	detEta = iDetEta;
      }else clear();
    }
    bool operator<(const P4DetEtaStruct& rhs)const{return pt<rhs.pt;}
    bool operator>(const P4DetEtaStruct& rhs)const{return pt>rhs.pt;}
    void clear(){nrgy=-999;et=-999;eta=-999;phi=-0;pt=-999;detEta=-999;}
    static std::string contents(){return "nrgy/F:et:eta:phi:pt:detEta";}
  };

  struct EvtInfoStruct {
    int runnr,lumiSec,eventnr,bx,datasetCode;
    
    void fill(const SHEvent* event);
    void clear(){runnr=lumiSec=eventnr=bx=datasetCode=-1;}
    bool operator<(const EleStructs::EvtInfoStruct& rhs)const;
    static std::string contents(){return "runnr/I:lumiSec:eventnr:bx:datasetCode";}
  };

  struct GenPUStruct {
    int nrTruePUInt,nrPUInt,nrPUIntPos,nrPUIntNeg;   
    void fill(const SHEvent& event);
    void clear(){nrTruePUInt=nrPUInt=nrPUIntPos=nrPUIntNeg=-1;}
    static std::string contents(){return "nrTruePUInt/I:nrPUInt:nrPUIntPos:nrPUIntNeg";}
  };

   struct HLTEgammaStruct {
     float et,nrgy,eta,phi,hadem,sigmaIEtaIEta,dEtaIn,dPhiIn,nrMissHits,nrClus,seedClusEFrac,pmDPhi1,pmDPhi2,pmDPhi3,pmDPhi4,pmDPhi1Info,pmDPhi2Info,pmDPhi3Info,pmDPhi4Info,pmDRZ1,pmDRZ2,pmDRZ3,pmDRZ4,pmDRZ1Info,pmDRZ2Info,pmDRZ3Info,pmDRZ4Info,phiWidth,etaWidth,s2,dPhi1BestS2,dPhi2BestS2,dPhi3BestS2,dRZ2BestS2,dRZ3BestS2,ecalIso,hcalIso,trkIso,trkChi2,invEOInvP;
   
    static std::string contents(){return "et/F:nrgy:eta:phi:hadem:sigmaIEtaIEta:dEtaIn:dPhiIn:nrMissHits:nrClus:seedClusEFrac:pmDPhi1:pmDPhi2:pmDPhi3:pmDPhi4:pmDPhi1Info:pmDPhi2Info:pmDPhi3Info:pmDPhi4Info:pmDRZ1:pmDRZ2:pmDRZ3:pmDRZ4:pmDRZ1Info:pmDRZ2Info:pmDRZ3Info:pmDRZ4Info:phiWidth:etaWidth:s2:dPhi1BestS2:dPhi2BestS2:dPhi3BestS2:dRZ2BestS2:dRZ3BestS2:ecalIso:hcalIso:trkIso:trkChi2:invEOInvP";}
    void fill(const SHTrigObj& cand,const std::string& tag=""); 
     void clear(){et=nrgy=eta=phi=hadem=sigmaIEtaIEta=dEtaIn=dPhiIn=nrMissHits=nrClus=seedClusEFrac=pmDPhi1=pmDPhi2=pmDPhi3=pmDPhi4=pmDPhi1Info=pmDPhi2Info=pmDPhi3Info=pmDPhi4Info=pmDRZ1=pmDRZ2=pmDRZ3=pmDRZ4=pmDRZ1Info=pmDRZ2Info=pmDRZ3Info=pmDRZ4Info=phiWidth=etaWidth=s2=dPhi1BestS2=dPhi2BestS2=dPhi3BestS2=dRZ2BestS2=dRZ3BestS2=ecalIso=hcalIso=trkIso=trkChi2=invEOInvP=-999.;}
  };
}

#endif
