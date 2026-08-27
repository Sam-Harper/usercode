
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Version/interface/GetReleaseVersion.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloCellGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloSubdetectorGeometry.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

//dumps the ECAL crystal geometry to a columnar json file so it can be used
//outside of CMSSW; arrays are ordered by the det id dense/hashed index
class DumpEcalGeomJson : public edm::one::EDAnalyzer<> {
public:
  explicit DumpEcalGeomJson(const edm::ParameterSet& iPara);
  ~DumpEcalGeomJson() override = default;

  DumpEcalGeomJson(const DumpEcalGeomJson& rhs) = delete;
  DumpEcalGeomJson& operator=(const DumpEcalGeomJson& rhs) = delete;

private:
  void analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) override;

  struct GeomColumns {
    std::vector<unsigned int> rawId;
    std::vector<int> index1;  //ieta (EB) or ix (EE)
    std::vector<int> index2;  //iphi (EB) or iy (EE)
    std::vector<int> zside;   //EE only
    std::vector<double> front;
    std::vector<double> back;
    std::vector<double> axis;
    std::vector<double> corners;

    void reserve(size_t nrCells) {
      rawId.reserve(nrCells);
      index1.reserve(nrCells);
      index2.reserve(nrCells);
      zside.reserve(nrCells);
      front.reserve(3 * nrCells);
      back.reserve(3 * nrCells);
      axis.reserve(3 * nrCells);
      corners.reserve(24 * nrCells);
    }
  };

  void fillCell(const CaloSubdetectorGeometry& subDetGeom, const DetId& id, GeomColumns& cols) const;
  void writeJson(const GeomColumns& cols,
                 const std::string& filename,
                 const std::string& detector,
                 const std::string& index1Name,
                 const std::string& index2Name,
                 unsigned int run) const;

  static std::string utcTimestamp();
  static void writeIntArray(std::ofstream& file, const std::string& name, const std::vector<int>& vals, bool trailingComma);
  static void writeUIntArray(std::ofstream& file, const std::string& name, const std::vector<unsigned int>& vals, bool trailingComma);
  static void writeFloatArray(std::ofstream& file, const std::string& name, const std::vector<double>& vals, bool trailingComma);

  edm::ESGetToken<CaloGeometry, CaloGeometryRecord> caloGeomToken_;
  std::string outputFilename_;
  std::string globalTag_;
  bool dumpEndcap_;
  bool written_;
};

DumpEcalGeomJson::DumpEcalGeomJson(const edm::ParameterSet& iPara)
    : caloGeomToken_(esConsumes()),
      outputFilename_(iPara.getParameter<std::string>("outputFilename")),
      globalTag_(iPara.getParameter<std::string>("globalTag")),
      dumpEndcap_(iPara.getParameter<bool>("dumpEndcap")),
      written_(false) {}

void DumpEcalGeomJson::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
  if (written_)
    return;
  written_ = true;

  const CaloGeometry& caloGeom = iSetup.getData(caloGeomToken_);
  const unsigned int run = iEvent.id().run();

  {
    const CaloSubdetectorGeometry* ebGeom = caloGeom.getSubdetectorGeometry(DetId::Ecal, EcalBarrel);
    GeomColumns cols;
    cols.reserve(EBDetId::kSizeForDenseIndexing);
    for (int hashIndx = 0; hashIndx < EBDetId::kSizeForDenseIndexing; hashIndx++) {
      EBDetId id = EBDetId::unhashIndex(hashIndx);
      cols.index1.push_back(id.ieta());
      cols.index2.push_back(id.iphi());
      fillCell(*ebGeom, id, cols);
    }
    writeJson(cols, outputFilename_, "EcalBarrel", "ieta", "iphi", run);
  }

  if (dumpEndcap_) {
    std::string eeFilename = outputFilename_;
    const std::string jsonExt = ".json";
    if (eeFilename.size() >= jsonExt.size() && eeFilename.compare(eeFilename.size() - jsonExt.size(), jsonExt.size(), jsonExt) == 0) {
      eeFilename.insert(eeFilename.size() - jsonExt.size(), ".ee");
    } else {
      eeFilename += ".ee.json";
    }
    const CaloSubdetectorGeometry* eeGeom = caloGeom.getSubdetectorGeometry(DetId::Ecal, EcalEndcap);
    GeomColumns cols;
    cols.reserve(EEDetId::kSizeForDenseIndexing);
    for (int hashIndx = 0; hashIndx < EEDetId::kSizeForDenseIndexing; hashIndx++) {
      EEDetId id = EEDetId::unhashIndex(hashIndx);
      cols.index1.push_back(id.ix());
      cols.index2.push_back(id.iy());
      cols.zside.push_back(id.zside());
      fillCell(*eeGeom, id, cols);
    }
    writeJson(cols, eeFilename, "EcalEndcap", "ix", "iy", run);
  }
}

void DumpEcalGeomJson::fillCell(const CaloSubdetectorGeometry& subDetGeom, const DetId& id, GeomColumns& cols) const {
  auto cellGeom = subDetGeom.getGeometry(id);
  if (cellGeom == nullptr) {
    throw cms::Exception("GeomError") << "DumpEcalGeomJson: no cell geometry for det id " << std::hex << id.rawId();
  }
  cols.rawId.push_back(id.rawId());

  const GlobalPoint& front = cellGeom->getPosition();
  cols.front.push_back(front.x());
  cols.front.push_back(front.y());
  cols.front.push_back(front.z());

  const auto& corners = cellGeom->getCorners();
  if (corners.size() != 8) {
    throw cms::Exception("GeomError") << "DumpEcalGeomJson: cell " << std::hex << id.rawId() << std::dec << " has "
                                      << corners.size() << " corners, expected 8";
  }
  for (size_t cornerNr = 0; cornerNr < corners.size(); cornerNr++) {
    cols.corners.push_back(corners[cornerNr].x());
    cols.corners.push_back(corners[cornerNr].y());
    cols.corners.push_back(corners[cornerNr].z());
  }

  //rear face centre = centroid of corners 4-7; axis = unit(back - front) which
  //matches TruncatedPyramid::axis() by construction
  double backX = 0.25 * (corners[4].x() + corners[5].x() + corners[6].x() + corners[7].x());
  double backY = 0.25 * (corners[4].y() + corners[5].y() + corners[6].y() + corners[7].y());
  double backZ = 0.25 * (corners[4].z() + corners[5].z() + corners[6].z() + corners[7].z());
  cols.back.push_back(backX);
  cols.back.push_back(backY);
  cols.back.push_back(backZ);

  double axisX = backX - front.x();
  double axisY = backY - front.y();
  double axisZ = backZ - front.z();
  double axisMag = std::sqrt(axisX * axisX + axisY * axisY + axisZ * axisZ);
  cols.axis.push_back(axisX / axisMag);
  cols.axis.push_back(axisY / axisMag);
  cols.axis.push_back(axisZ / axisMag);
}

void DumpEcalGeomJson::writeJson(const GeomColumns& cols,
                                 const std::string& filename,
                                 const std::string& detector,
                                 const std::string& index1Name,
                                 const std::string& index2Name,
                                 unsigned int run) const {
  std::ofstream file(filename);
  if (!file) {
    throw cms::Exception("FileError") << "DumpEcalGeomJson: could not open output file " << filename;
  }

  std::string release = edm::getReleaseVersion();
  //getReleaseVersion() historically comes with embedded quotes, strip them
  release.erase(std::remove(release.begin(), release.end(), '"'), release.end());

  file << "{\n\"meta\": {\n";
  file << "  \"format_version\": 1,\n";
  file << "  \"detector\": \"" << detector << "\",\n";
  file << "  \"n_crystals\": " << cols.rawId.size() << ",\n";
  file << "  \"ordering\": \"" << (detector == "EcalBarrel" ? "EBDetId::hashedIndex()" : "EEDetId::hashedIndex()") << "\",\n";
  file << "  \"units\": \"cm\",\n";
  file << "  \"cmssw_release\": \"" << release << "\",\n";
  file << "  \"global_tag\": \"" << globalTag_ << "\",\n";
  file << "  \"run\": " << run << ",\n";
  file << "  \"timestamp\": \"" << utcTimestamp() << "\",\n";
  file << "  \"corner_convention\": \"24 floats/crystal [x,y,z]*8; corners 0-3 front face, 4-7 rear face; corner k pairs with k+4\",\n";
  file << "  \"axis_convention\": \"unit vector front-face centroid -> rear-face centroid\"\n";
  file << "},\n";

  writeUIntArray(file, "rawid", cols.rawId, true);
  writeIntArray(file, index1Name, cols.index1, true);
  writeIntArray(file, index2Name, cols.index2, true);
  if (!cols.zside.empty()) {
    writeIntArray(file, "zside", cols.zside, true);
  }
  writeFloatArray(file, "front", cols.front, true);
  writeFloatArray(file, "back", cols.back, true);
  writeFloatArray(file, "axis", cols.axis, true);
  writeFloatArray(file, "corners", cols.corners, false);
  file << "}\n";
}

std::string DumpEcalGeomJson::utcTimestamp() {
  auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  std::tm utc{};
  gmtime_r(&now, &utc);
  std::ostringstream ss;
  ss << std::put_time(&utc, "%Y-%m-%dT%H:%M:%SZ");
  return ss.str();
}

void DumpEcalGeomJson::writeIntArray(std::ofstream& file, const std::string& name, const std::vector<int>& vals, bool trailingComma) {
  file << "\"" << name << "\": [";
  for (size_t i = 0; i < vals.size(); i++) {
    if (i != 0)
      file << ",";
    file << vals[i];
  }
  file << "]" << (trailingComma ? "," : "") << "\n";
}

void DumpEcalGeomJson::writeUIntArray(std::ofstream& file, const std::string& name, const std::vector<unsigned int>& vals, bool trailingComma) {
  file << "\"" << name << "\": [";
  for (size_t i = 0; i < vals.size(); i++) {
    if (i != 0)
      file << ",";
    file << vals[i];
  }
  file << "]" << (trailingComma ? "," : "") << "\n";
}

void DumpEcalGeomJson::writeFloatArray(std::ofstream& file, const std::string& name, const std::vector<double>& vals, bool trailingComma) {
  file << "\"" << name << "\": [";
  char buffer[32];
  for (size_t i = 0; i < vals.size(); i++) {
    if (i != 0)
      file << ",";
    snprintf(buffer, sizeof(buffer), "%.5f", vals[i]);
    file << buffer;
  }
  file << "]" << (trailingComma ? "," : "") << "\n";
}

DEFINE_FWK_MODULE(DumpEcalGeomJson);
