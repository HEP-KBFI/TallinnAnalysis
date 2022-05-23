import FWCore.ParameterSet.Config as cms

#---
# @brief Example for one-dimensional control plot
# @param The binning of the one-dimensional histogram is specified via the attributes 'xAxis.numBins', 'xAxis.xMin', and 'xAxis.xMax'.
#        The observable to be plotted need to be specified via the attribute 'xAxis.observable.branchName' 
#        and needs to correspond to a branch in the plain Ntuple that is read by the analysis job.
# 
#        The example below makes a control plot of the mass of the lepton pair in events containing two leptons,
#        separately for ee, emu, and mumu events.
#---

controlPlot1D = cms.PSet(
    pluginType = cms.string('HistogramFiller1D'),
    histogramDir = cms.string("controlPlots/m_ll"),
    xAxis = cms.PSet(
        numBins = cms.uint32(20),
        xMin = cms.double(0.),
        xMax = cms.double(200.),
        observable = cms.PSet(
            pluginType = cms.string('BranchVarExtractor'),
            branchName = cms.string('m_ll')
        )
    ),
    categories = cms.VPSet(
        cms.PSet(
            histogramDir = cms.string('ee'),
            selection = cms.string('lep1_pdgId == 11 && lep2_pdgId == 11')
        ),
        cms.PSet(
            histogramDir = cms.string('em'),
            selection = cms.string('(lep1_pdgId == 11 && lep2_pdgId == 13) || (lep1_pdgId == 13 && lep2_pdgId == 11)')
        ),
        cms.PSet(
            histogramDir = cms.string('mm'),
            selection = cms.string('lep1_pdgId == 13 && lep2_pdgId == 13')
        )
    )
)
