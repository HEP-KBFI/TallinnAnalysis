import FWCore.ParameterSet.Config as cms

branchVarHistogramFiller1D = cms.PSet(
    pluginType = cms.string("BranchVarHistogramFiller"),
    histogramDir = cms.string('met_LD'),
    branches = cms.vstring('met_LD'),
    xAxis = cms.PSet(
        numBins = cms.uint32(20),
        xMin = cms.double(0.),
        xMax = cms.double(200.)
    )
)

branchVarHistogramFiller2D = cms.PSet(
    pluginType = cms.string("BranchVarHistogramFiller"),
    histogramDir = cms.string('htmiss_vs_met'),
    branches = cms.vstring('met', 'htmiss'),
    xAxis = cms.PSet(
        numBins = cms.uint32(20),
        xMin = cms.double(0.),
        xMax = cms.double(200.)
    ),
    yAxis = cms.PSet(
        numBins = cms.uint32(20),
        yMin = cms.double(0.),
        yMax = cms.double(200.)
    )
)
