#include "SHarper/SHNtupliser/interface/SHVertex.hh"

#include "DataFormats/VertexReco/interface/Vertex.h"

SHVertex::SHVertex(const reco::Vertex& vertex):
  pos_(vertex.x(),vertex.y(),vertex.z()),
  ndof_(vertex.ndof()),
  chi2_(vertex.chi2()),
  nrTracks_(vertex.tracksSize()),
  isValid_(vertex.isValid()),
  sumP4_()
{
  math::XYZTLorentzVectorD p4(vertex.p4());
  sumP4_.SetXYZT(p4.X(),p4.Y(),p4.Z(),p4.T());
  
}
