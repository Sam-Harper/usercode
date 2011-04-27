
#include "FWCore/Framework/interface/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

#include "FWCore/Framework/interface/MakerMacros.h"

class VertexFilter : public edm::EDFilter {

private:
  edm::InputTag vtxCollTag_;
  int minNrVertices_;
  int maxNrVertices_;

public:
  explicit VertexFilter(const edm::ParameterSet& para);
  ~VertexFilter(){}
  
  virtual bool filter(edm::Event& event,const edm::EventSetup& setup);
};

VertexFilter::VertexFilter(const edm::ParameterSet& para):
  vtxCollTag_(para.getParameter<edm::InputTag>("vtxColl")),
  minNrVertices_(para.getParameter<int>("minNrVertices")),
  maxNrVertices_(para.getParameter<int>("maxNrVertices"))
{

}

bool VertexFilter::filter(edm::Event& event,const edm::EventSetup& setup)
{
  
 edm::Handle<reco::VertexCollection> vertices;
 event.getByLabel(vtxCollTag_,vertices);
 if(static_cast<int>(vertices->size())>=minNrVertices_ && static_cast<int>(vertices->size())<=maxNrVertices_ ) return true;
 else return false;

}
  
DEFINE_FWK_MODULE(VertexFilter);
