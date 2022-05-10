import FWCore.ParameterSet.Config as cms

# example for artificial neural network (ANN) used in 2lss+1tau channel of ttH multilepton analysis (HIG-19-008)

tensorflowHistogramFiller = cms.PSet(
    pluginType = cms.string("TensorFlowHistogramFiller"),
    classes = cms.vstring('predictions_ttH',  'predictions_rest', 'predictions_tH' ])
    histogramDirs = cms.PSet(
        predictions_ttH = cms.string('ttH'),
        predictions_rest = cms.string('otherBkg'),
        predictions_tH = cms.string('tH')

    ),
    mvaInputVariables = cms.vstring(),    
    xAxis = cms.PSet(
        numBins = cms.uint32(100),
        xMin = cms.double(0.),
        xMax = cms.double(1.)
    )
)
