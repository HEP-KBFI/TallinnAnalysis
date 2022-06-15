import FWCore.ParameterSet.Config as cms

from TallinnAnalysis.HistogramTools.controlPlot1D_cfi import controlPlot1D as histograms_controlPlot1D
from TallinnAnalysis.HistogramTools.controlPlot2D_cfi import controlPlot2D as histograms_controlPlot2D
from TallinnAnalysis.HistogramTools.datacard_HH_2lss_cfi import datacard_HH_2lss_nonresonant as histograms_datacard_HH_2lss_nonresonant, datacard_HH_2lss_resonant_spin0 as histograms_datacard_HH_2lss_resonant_spin0, datacard_HH_2lss_resonant_spin2 as histograms_datacard_HH_2lss_resonant_spin2

process = cms.PSet()

process.fwliteInput = cms.PSet(
    fileNames = cms.vstring()
)

process.fwliteOutput = cms.PSet(
    fileName = cms.string('')
)

process.analyze = cms.PSet(
    treeName = cms.string('Events'),

    process = cms.string(''),
 
    central_or_shifts = cms.vstring('central'),

    selection = cms.string('nlep == 2 & ntau == 1'),

    histogramPlugins = cms.VPSet(
#        histograms_controlPlot1D,
#        histograms_controlPlot2D,
        histograms_datacard_HH_2lss_nonresonant,
#        histograms_datacard_HH_2lss_resonant_spin0,
#        histograms_datacard_HH_2lss_resonant_spin2
    ),

    evtWeights = cms.vstring([ "evtWeight" ]),

    selEventsFileName_input = cms.string(''),
    selEventsFileName_output = cms.string(''),

    isDEBUG = cms.bool(False)
)

process.fwliteInput.fileNames = cms.vstring(['/home/veelken/newTallinnAnalysis/CMSSW_12_3_1/src/TallinnNtupleProducer/Framework/test/produceNtuple.root'])
process.fwliteOutput.fileName = cms.string('analyze.root')
process.analyze.process                                        = cms.string('signal_ggf_nonresonant_hh')
process.analyze.central_or_shifts                              = cms.vstring('central')
##process.analyze.selection                                      = cms.string('nlep == 2 && ntau == 1 && passesTrigger && lep1_pt > 25. && lep1_isTight && lep1_tightCharge >= 2 && !(lep1_isFake || lep1_isFlip) && lep2_pt > 15. && lep2_isTight && lep2_tightCharge >= 2 && !(lep2_isFake || lep2_isFlip) && lep1_charge*lep2_charge > 0 && tau1_pt > 20. && tau1_isTight && !tau1_isFake && (njetAK4 >= 2 || njetAK8Wjj >= 1) && njetAK4bL <= 1 && njetAK4bM == 0 && (lep1_pdgId == 13 || lep2_pdgId == 13 || met_LD > 30.) && passesLowMassLeptonPairVeto && passesZbosonVeto && passesHtoZZto4lVeto && passesMEtFilters && ntightlep == 2 && ntighttau == 1')
process.analyze.selection                                      = cms.string('nlep == 2 && ntau == 1 && passesTrigger && lep1_pt > 25. && lep1_isTight && lep1_tightCharge >= 2 && lep2_pt > 15. && lep2_isTight && lep2_tightCharge >= 2 && lep1_charge*lep2_charge > 0 && tau1_pt > 20. && tau1_isTight && (njetAK4 >= 2 || njetAK8Wjj >= 1) && njetAK4bL <= 1 && njetAK4bM == 0 && (lep1_pdgId == 13 || lep2_pdgId == 13 || met_LD > 30.) && passesLowMassLeptonPairVeto && passesZbosonVeto && passesHtoZZto4lVeto && passesMEtFilters && ntightlep == 2 && ntighttau == 1')
process.analyze.evtWeights                                     = cms.vstring([ "evtWeight" ])
#process.analyze.selEventsFileName_input                  = cms.string('selEvents_HH_2lss_oldSoftware.txt')
process.analyze.selEventsFileName_output                       = cms.string('selEvents_HH_2lss_newSoftware.txt')
process.analyze.isDEBUG                                        = cms.bool(False)
