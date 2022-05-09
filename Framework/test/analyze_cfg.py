import FWCore.ParameterSet.Config as cms

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
 
    central_or_shift = cms.string('central'),

    selection = cms.string('nlep == 2 & ntau == 1'),

    histogramDir = cms.string(''),
    histogramType = cms.string('TH1D'),
    xAxis = cms.PSet(
        numBins = cms.uint32(20),
        xMin = cms.double(0.),
        xMax = cms.double(200.)
    )
   
    varExtractorPlugins = cms.VPSet(
        cms.PSet(
            pluginType = cms.string('BranchVarExtractorFloat'),
            branchName = cms.string('m_ll')
        )
    )

    evtWeights = cms.vstring([ "evtWeight" ]),

    selEventsFileName = cms.string(''),

    isDEBUG = cms.bool(False)
)

process.fwliteInput.fileNames = cms.vstring(['/home/veelken/newTallinnAnalysis/CMSSW_12_3_1/src/TallinnNtupleProducer/Framework/test/produceNtuple.root'])
process.fwliteOutput.fileName = cms.string('analyze.root')
process.analyze.process                                        = cms.string('signal_ggf_nonresonant_hh'),
process.analyze.central_or_shift                               = cms.string('central')
process.analyze.selection                                      = cms.string('nlep == 2 & ntau == 1')
process.analyze.evtWeights                                     = cms.vstring([ "evtWeight" ])
process.analyze.isDEBUG                                        = cms.bool(False)
#process.produceNtuple.selEventsFileName                        = cms.string('selEvents_DEBUG.txt')
