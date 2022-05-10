import FWCore.ParameterSet.Config as cms

# example boosted decision tree (BDT) used in non-resonant 2lss channel of HH->multilepton analysis (HIG-21-002)

tmvaHistogramFiller = cms.PSet(
    pluginType = cms.string('TMVAHistogramFiller'),
    histogramDir = cms.string(),
    mvaFileName_odd = cms.string('hhAnalysis/multilepton/data/BDT_2lss_leq1tau/nonres_default_v7p2_BDT_wtrainVarOptNew_sortedSignificance_SMNLO/2lss_leq1tau_even_half_model_nonres_default.xml'),
    mvaFileName_even = cms.string('hhAnalysis/multilepton/data/BDT_2lss_leq1tau/nonres_default_v7p2_BDT_wtrainVarOptNew_sortedSignificance_SMNLO/2lss_leq1tau_odd_half_model_nonres_default.xml'),
    # CV: set enableBDTTransform to True if using XML files converted from xgboost trainings,
    #     set enableBDTTransform to False if using XML files trained with TMVA
    enableBDTTransform = cms.bool(True),
    mvaInputVariables = cms.vstring(
        "m_ll",
        "dR_ll",
        "lep1_conePt",
        "lep2_conePt",
        "min_dR_lep1_jet",
        "HT",
        "min_dR_lep2_jet",
        "min_dR_lep_Wjets",
        "htmiss",
        "m_Wjj1",
        # CV: "SM", "BM1",..."BM12" inputs with one-hot encoding need to the augmented
    ),
    fitFunctionFileName = cms.string(''),
    fitFunctionVariable = cms.string('gen_mHH'),
    xAxis = cms.PSet(
        numBins = cms.uint32(100),
        xMin = cms.double(0.),
        xMax = cms.double(1.)
    )
)
