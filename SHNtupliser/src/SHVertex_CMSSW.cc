#include "SHarper/SHNtupliser/interface/SHVertex.hh"

#include "DataFormats/VertexReco/interface/Vertex.h"

SHVertex::SHVertex(const reco::Vertex& vertex):
  vx_(vertex.x()),vy_(vertex.y()),vz_(vertex.z()),
  chi2_(vertex.chi2()),
  sumPt_(vertex.p4().pt()),
  data_(packData(vertex.isValid(),vertex.ndof(),vertex.tracksSize()))
{ 

}
