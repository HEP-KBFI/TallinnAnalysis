import FWCore.ParameterSet.Config as cms

#---
# @brief Example for two-dimensional control plot
# @param The binning of the two-dimensional histogram is specified via the attributes 'xAxis.numBins', 'xAxis.xMin', and 'xAxis.xMax' for the x-axis
#        and 'yAxis.numBins', 'yAxis.yMin', and 'yAxis.yMax' for the y-axis.
#        The observables to be plotted need to be specified via the attributes 'xAxis.observable.branchName' and 'yAxis.observable.branchName'
#        and need to correspond to a branch in the plain Ntuple that is read by the analysis job.
# 
#        The example below makes a control plot of HTmiss vs ETmiss. 
#        The observables HTmiss and ETmiss are described in Section 4 of the paper arXiv:2011.03652.
#
#        This example does not use event categorization.
#---

controlPlot2D = cms.PSet(
    pluginType = cms.string('HistogramFiller2D'),
    histogramDir = cms.string("controlPlots/htmiss_vs_met_pt"),
    xAxis = cms.PSet(
        numBins = cms.uint32(20),
        xMin = cms.double(0.),
        xMax = cms.double(200.),
        observable = cms.PSet(
            pluginType = cms.string('BranchVarExtractor'),
            branchName = cms.string('met_pt')
        )
    ),
    yAxis = cms.PSet(
        numBins = cms.uint32(20),
        yMin = cms.double(0.),
        yMax = cms.double(200.),
        observable = cms.PSet(
            pluginType = cms.string('BranchVarExtractor'),
            branchName = cms.string('htmiss')
        )
    )
)
