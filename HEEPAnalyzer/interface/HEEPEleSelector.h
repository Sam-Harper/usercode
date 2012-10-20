#ifndef SHARPER_HEEPANALYZER_HEEPELESELECTOR
#define SHARPER_HEEPANALYZER_HEEPELESELECTOR

//class: heep::EleSelector
//Description: an class which can use used to select HEEP electrons
//
//Implimentation: 
//    it can either return  true/false bool on either the electron passed
//    or more usefully a 'cutCode' whose bits tell if the cut failed or not
//    note: bit=1 means the cut was failed
//    The bits each cut corresponds to is defined in HEEPCutCodes
//    It can be either used with a heep or pat electron (and the two *should* give identical results)
//
//    in 3X, I dropped the ability to have multiple cuts depending on classification for barrel and endcap electrons
//    it wasnt being used and this makes the code much easier to understand


#include "SHarper/HEEPAnalyzer/interface/HEEPCutValues.h"
#include "SHarper/HEEPAnalyzer/interface/HEEPEffectiveAreas.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/VertexReco/interface/Vertex.h"


#include <iostream>

namespace reco {
  class GsfElectron;
}

namespace heep { 

  class Ele;

  class EleSelector  {
    
  private: 
    EleCutValues barrelCutValues_;
    EleCutValues endcapCutValues_;
    EffectiveAreas isolEffectAreas_;
  public:
    EleSelector(){}//default, it doesnt to anything
    explicit EleSelector(const edm::ParameterSet& config){setup(config);} 
    ~EleSelector(){} 
  
    void setup(const edm::ParameterSet&);  

    bool passCuts(const heep::Ele& ele,const int cutMask=~0x0)const{return getCutCode(ele,cutMask)==0x0;}
    int getCutCode(const heep::Ele& ele,const int cutMask=~0x0)const;
    static int getCutCode(const heep::Ele& ele,const EleCutValues& cuts,const int cutMask=~0x0);
    
    //note: you can pass a pat::Electron into these functions as it inherits from GsfElectron
    //its a little annoying now having to pass rho in but meh, its safer passing it in each function rather than storing it 
    //in the EleSelector and updating every event, let me know if you disagree btw, I'm open to suggestions on how to solve this
    //oh and rho is first as this function is already overloaded with an int at the end
    //now I've had to add in the vertex point as well for dxy, I'm open to suggestions on what to do here
    bool passCuts(const float rho,const std::vector<reco::Vertex>& vertices,math::XYZPoint &vertex,const reco::GsfElectron& ele,const int cutMask=~0x0)const{return getCutCode(rho,vertices.empty() ? math::XYZPoint(0,0,0) : vertices[0].position(),ele,cutMask)==0x0;}
    bool passCuts(const float rho,const math::XYZPoint &vertex,const reco::GsfElectron& ele,const int cutMask=~0x0)const{return getCutCode(rho,vertex,ele,cutMask)==0x0;}
    int getCutCode(const float rho,const std::vector<reco::Vertex>& vertices,const reco::GsfElectron& ele,const int cutMask=~0x0)const;
    int getCutCode(const float rho,const math::XYZPoint &vertex,const reco::GsfElectron& ele,const int cutMask=~0x0)const;
    static int getCutCode(const float rho,const math::XYZPoint &vertex,const reco::GsfElectron& ele,const EffectiveAreas& effectAreas,const EleCutValues& cuts,const int cutMask=~0x0);

    //backwards compatible functions which mean you dont have to specify rho and it will default to zero (therefore correction is zero)
    bool passCuts(const reco::GsfElectron& ele,const int cutMask=~0x0)const{return getCutCode(ele,cutMask)==0x0;}
    int getCutCode(const reco::GsfElectron& ele,const int cutMask=~0x0)const{return getCutCode(0.,math::XYZPoint(0,0,0),ele,cutMask);}
    static int getCutCode(const reco::GsfElectron& ele,const EleCutValues& cuts,int cutMask=~0x0){return getCutCode(0.,math::XYZPoint(0,0,0),ele,EffectiveAreas(),cuts,cutMask);}


    EleCutValues* getBarrelCuts(){return &barrelCutValues_;}
    const EleCutValues* getBarrelCuts()const{return &barrelCutValues_;}
    EleCutValues* getEndcapCuts(){return &endcapCutValues_;}
    const EleCutValues* getEndcapCuts()const{return &endcapCutValues_;} 
    EleCutValues* getCuts(int type){return type<100 ? getBarrelCuts() : getEndcapCuts();}
    const EleCutValues* getCuts(int type)const{return type<100 ? getBarrelCuts() : getEndcapCuts();}

  };
}

#endif
