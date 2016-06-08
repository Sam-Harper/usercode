#ifndef SHARPER_HEEPANALYZER_HEEPGSFELEEXTRA_H
#define SHARPER_HEEPANALYZER_HEEPGSFELEEXTRA_H

//a class to store all the little things that a GsfElectron needs in order to calculate the heep ID

namespace heep {
  struct GsfEleExtra {
    double rho;
    int nrSatCrysIn5x5;
    float trkIsoNoJetCore;
    math::XYZPoint primaryVertex;
  };
}


#endif
