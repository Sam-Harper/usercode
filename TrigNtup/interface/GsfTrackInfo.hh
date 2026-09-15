#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include <limits>

struct GsfTrackInfo {
public:
    GsfTrackInfo():
    p(0),
    pt(0),
    eta(0),
    phi(0),
    pMode(0),
    ptMode(0),
    etaMode(0),
    phiMode(0),
    qoverpModeError(0),
    fbrem(std::numeric_limits<float>::max()),
    charge(0),
    chargeMode(0)
    {}

    GsfTrackInfo(const reco::GsfTrack& gsfTrack,float fbrem=std::numeric_limits<float>::max()):fbrem(fbrem) {
        p = gsfTrack.p();
        pt = gsfTrack.pt();
        eta = gsfTrack.eta();
        phi = gsfTrack.phi();
        pMode = gsfTrack.pMode();
        ptMode = gsfTrack.ptMode();
        etaMode = gsfTrack.etaMode();
        phiMode = gsfTrack.phiMode();
        qoverpModeError = gsfTrack.qoverpModeError();        
        charge = gsfTrack.charge();
        chargeMode = gsfTrack.chargeMode();
    }

    float p;
    float pt;
    float eta;
    float phi;
    float pMode;
    float ptMode;
    float etaMode;
    float phiMode;
    float qoverpModeError;
    float fbrem;
    int charge;
    int chargeMode;


};
