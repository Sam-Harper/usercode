

import FWCore.ParameterSet.Config as cms

dPhi1SParams=cms.PSet(
    bins= cms.VPSet(
        cms.PSet(binType=cms.string("AbsEtaClusParamBin"),minEta=cms.double(0),maxEta=cms.double(1.5),minNrClus=cms.int32(1),maxNrClus=cms.int32(1),etaFunc=cms.PSet(funcType=cms.string("Pol3"),p0=cms.double(0.000981),p1=cms.double(0.000898),p2=cms.double(-0.00128),p3=cms.double(0.000897))),
        cms.PSet(binType=cms.string("AbsEtaClusParamBin"),minEta=cms.double(0),maxEta=cms.double(1.5),minNrClus=cms.int32(2),maxNrClus=cms.int32(2),etaFunc=cms.PSet(funcType=cms.string("Pol3"),p0=cms.double(0.00222),p1=cms.double(0.000196),p2=cms.double(-0.000203),p3=cms.double(0.000447))),
         cms.PSet(binType=cms.string("AbsEtaClusParamBin"),minEta=cms.double(0),maxEta=cms.double(1.5),minNrClus=cms.int32(3),maxNrClus=cms.int32(99999),etaFunc=cms.PSet(funcType=cms.string("Pol3"),p0=cms.double(0.00236),p1=cms.double(0.000691),p2=cms.double(0.000199),p3=cms.double(0.000416))),
         cms.PSet(binType=cms.string("AbsEtaClusParamBin"),minEta=cms.double(1.5),maxEta=cms.double(3.0),minNrClus=cms.int32(1),maxNrClus=cms.int32(1),etaFunc=cms.PSet(funcType=cms.string("Pol1"),p0=cms.double(0.00562),p1=cms.double(-0.00166))),
        cms.PSet(binType=cms.string("AbsEtaClusParamBin"),minEta=cms.double(1.5),maxEta=cms.double(3.0),minNrClus=cms.int32(2),maxNrClus=cms.int32(2),etaFunc=cms.PSet(funcType=cms.string("Pol1"),p0=cms.double(0.00866),p1=cms.double(-0.00271))), 
        cms.PSet(binType=cms.string("AbsEtaClusParamBin"),minEta=cms.double(1.5),maxEta=cms.double(3.0),minNrClus=cms.int32(3),maxNrClus=cms.int32(99999),etaFunc=cms.PSet(funcType=cms.string("Pol1"),p0=cms.double(0.0132),p1=cms.double(-0.0048)))
        )
    )

dRZ2SParams=cms.PSet(
    bins= cms.VPSet(
        cms.PSet(binType=cms.string("AbsEtaClusWithClusCorrParamBin"),minEta=cms.double(0),maxEta=cms.double(1.5),minNrClus=cms.int32(1),maxNrClus=cms.int32(99999),corrPerClus=cms.double(0.0034),clusNrOffset=cms.int32(4),etaFunc=cms.PSet(funcType=cms.string("Pol2"),p0=cms.double(0.00317),p1=cms.double(-0.00337),p2=cms.double(0.00395))),
         cms.PSet(binType=cms.string("AbsEtaClusParamBin"),minEta=cms.double(1.5),maxEta=cms.double(3.0),minNrClus=cms.int32(1),maxNrClus=cms.int32(1),etaFunc=cms.PSet(funcType=cms.string("GausPlusConst"),constTerm=cms.double(0.00435),norm=cms.double(0.00289),mean=cms.double(1.65),sigma=cms.double(0.22))),
        cms.PSet(binType=cms.string("AbsEtaClusParamBin"),minEta=cms.double(1.5),maxEta=cms.double(3.0),minNrClus=cms.int32(2),maxNrClus=cms.int32(2),etaFunc=cms.PSet(funcType=cms.string("GausPlusConst"),constTerm=cms.double(0.00541),norm=cms.double(0.00404),mean=cms.double(1.76),sigma=cms.double(0.22))), 
        cms.PSet(binType=cms.string("AbsEtaClusParamBin"),minEta=cms.double(1.5),maxEta=cms.double(3.0),minNrClus=cms.int32(3),maxNrClus=cms.int32(3),etaFunc=cms.PSet(funcType=cms.string("GausPlusConst"),constTerm=cms.double(0.00856),norm=cms.double(0.00351),mean=cms.double(1.79),sigma=cms.double(0.138))),
        cms.PSet(binType=cms.string("AbsEtaClusParamBin"),minEta=cms.double(1.5),maxEta=cms.double(3.0),minNrClus=cms.int32(4),maxNrClus=cms.int32(99999),etaFunc=cms.PSet(funcType=cms.string("GausPlusConst"),constTerm=cms.double(0.00669),norm=cms.double(0.0123),mean=cms.double(1.75),sigma=cms.double(0.236)))
        )
    )

dPhi2SParams=cms.PSet(
    bins=cms.VPSet(
        cms.PSet(binType=cms.string("AbsEtaClusParamBin"),minEta=cms.double(0),maxEta=cms.double(3.0),minNrClus=cms.int32(1),maxNrClus=cms.int32(99999),etaFunc=cms.PSet(funcType=cms.string("Pol0"),p0=cms.double(0.00011))),
        )
)
