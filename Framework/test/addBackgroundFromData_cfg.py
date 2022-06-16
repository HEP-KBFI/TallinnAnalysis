import FWCore.ParameterSet.Config as cms

import os

process = cms.PSet()

process.fwliteInput = cms.PSet(
    fileNames = cms.vstring(),
    maxEvents = cms.int32(-1),
    outputEvery = cms.uint32(100000)
)

process.fwliteOutput = cms.PSet(
    fileName = cms.string('')
)

process.addBackgroundFromData = cms.PSet(

    processData = cms.string('data_obs'),
    processFakes_or_Flips = cms.string('data_fakes'),
    processesToSubtract = cms.vstring(),
    
    central_or_shifts = cms.vstring('central')

    makeBinContentsPositive = cms.bool(False)
)
