import FWCore.ParameterSet.VarParsing as VarParsing

def registerDefaultMCOptions(options):
    options.register ('minMass',
                      -1,
                      VarParsing.VarParsing.multiplicity.singleton,
                      VarParsing.VarParsing.varType.float,          
                      "min mass")
    options.register ('maxMass',
                      -1, 
                      VarParsing.VarParsing.multiplicity.singleton,
                      VarParsing.VarParsing.varType.float,          
                      "max mass")

    options.register ('mass',
                      1000, 
                      VarParsing.VarParsing.multiplicity.singleton,
                      VarParsing.VarParsing.varType.float,          
                      "mass")
    options.register ('comEnergy',
                        13, 
                      VarParsing.VarParsing.multiplicity.singleton,
                      VarParsing.VarParsing.varType.float,          
                      "Centre of Mass Energy (in TeV)")
    options.register ('outFile',
                      "output.root", 
                      VarParsing.VarParsing.multiplicity.singleton,
                      VarParsing.VarParsing.varType.string,          
                      "output filename (without tags)")
    options.register ('pdfSet',
                      13, 
                      VarParsing.VarParsing.multiplicity.singleton,
                      VarParsing.VarParsing.varType.int,          
                      "pythia pdf set")
