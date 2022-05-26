import FWCore.ParameterSet.Config as cms

#---
# @brief Fill histograms with output of boosted decision trees (BDTs)
#        used in 2lss channel of HH->multilepton analysis (HIG-21-002).
#        Three different BDTs have been trained: for non-resonant HH production 
#        and for the resonant production of H boson pairs, originating from decays of new heavy particles X of spin 0 and of spin 2.
# @param The BDT for non-resonant HH production is parametrized by the coupling scenario.
#        The coupling scenarios are described in the papers arXiv:1507.02245, arXiv:1908.08923, and arXiv:1806.05162,
#        and are referred to as { "SM", "JHEP04BM1",..., "JHEP04BM12", "JHEP04BM8a", "JHEP03BM1",..., "JHEP03BM7" }.
#        The BDTs for resonant HH production are parametrized by the mass mX (also referred to as "gen_mHH") of the heavy particles X.
#---

from TallinnNtupleProducer.EvtWeightTools.HHCoupling_cfi import hhCoupling as config_hhCoupling

datacard_HH_2lss_nonresonant = cms.PSet(
    pluginType = cms.string('BDTHistogramFiller_HH_multilepton'),
    histogramDir = cms.string("nonresonant"),
    mode = cms.string("nonresonant"),
    nonRes_BMs = cms.vstring([ 'SM', 'JHEP04BM1', 'JHEP04BM2', 'JHEP04BM3', 'JHEP04BM4', 'JHEP04BM5', 'JHEP04BM6', 'JHEP04BM7', 'JHEP04BM8', 'JHEP04BM9', 'JHEP04BM10', 'JHEP04BM11', 'JHEP04BM12', 'JHEP04BM8a', 'JHEP03BM1', 'JHEP03BM2', 'JHEP03BM3', 'JHEP03BM4', 'JHEP03BM5', 'JHEP03BM6', 'JHEP03BM7' ]),
    mvaFileName_odd = cms.string('TallinnAnalysis/MLTools/data/HH_2lss/2lss_leq1tau_even_half_model_nonres_default.xml'),
    mvaFileName_even = cms.string('TallinnAnalysis/MLTools/data/HH_2lss/2lss_leq1tau_odd_half_model_nonres_default.xml'),
    # CV: set transform_mvaOutput to True if using XML files converted from xgboost trainings,
    #     set transform_mvaOutput to False if using XML files trained with TMVA
    transform_mvaOutput = cms.bool(True),
    mvaInputVariables = cms.vstring(
        "m_ll",
        "dR_ll",
        "lep1_conePt",
        "lep2_conePt",
        "min_dR_lep1_jet",
        "ht",
        "min_dR_lep2_jet",
        "min_dR_lep_Wjets",
        "htmiss",
        "m_jj_W1"
    ),
    hhCoupling = config_hhCoupling,
    fitFunctionFileName = cms.string(''),
    fitFunctionVariable = cms.string(''),
    xAxis = cms.PSet(
        numBins = cms.uint32(100),
        xMin = cms.double(0.),
        xMax = cms.double(1.)
    ),
    jsonFileName = cms.string("datacard_HH_2lss_nonresonant_newSoftware.json"),
    isDEBUG = cms.bool(True)
)

datacard_HH_2lss_resonant_spin0 = datacard_HH_2lss_nonresonant.clone(
    histogramDir = cms.string("resonant_spin0"),
    mode = cms.string("resonant"),
    gen_mHH = cms.vdouble([ 250., 260., 270., 280., 300., 320., 350., 400., 450., 500., 550., 600., 650., 700., 750., 800., 850., 900., 1000. ]),
    mvaFileName_odd = cms.string('TallinnAnalysis/MLTools/data/HH_2lss/2lss_leq1tau_even_half_model_spin0.xml'),
    mvaFileName_even = cms.string('TallinnAnalysis/MLTools/data/HH_2lss/2lss_leq1tau_odd_half_model_spin0.xml'),
    mvaInputVariables = cms.vstring(
        "m_ll",
        "mHH_vis",
        "met_LD",
        "dR_ll",
        "min_dR_lep_Wjets",
        "min_dR_lep_leadWjet",
        "max_dR_lep_Wjets",
        "max_dR_lep_leadWjet",
        "dR_jj_W1",
        "mT_lep1"
    ),
    fitFunctionFileName = cms.string('TallinnAnalysis/MLTools/data/HH_2lss/2lss_leq1tau_TProfile_signal_fit_func_spin0.root'),
    fitFunctionVariable = cms.string('gen_mHH'),
    isDEBUG = cms.bool(False)
)

datacard_HH_2lss_resonant_spin2 = datacard_HH_2lss_resonant_spin0.clone(
    histogramDir = cms.string("resonant_spin2"),
    mvaFileName_odd = cms.string('TallinnAnalysis/MLTools/data/HH_2lss/2lss_leq1tau_even_half_model_spin2.xml'),
    mvaFileName_even = cms.string('TallinnAnalysis/MLTools/data/HH_2lss/2lss_leq1tau_odd_half_model_spin2.xml'),
    mvaInputVariables = cms.vstring(
        "m_ll",
        "mHH_vis",
        "met_LD",
        "dR_ll",
        "min_dR_lep_Wjets",
        "min_dR_lep_leadWjet",
        "max_dR_lep_Wjets",
        "max_dR_lep_leadWjet",
        "dR_jj_W1",
        "mT_lep1"
    ),
    fitFunctionFileName = cms.string('TallinnAnalysis/MLTools/data/HH_2lss/2lss_leq1tau_TProfile_signal_fit_func_spin2.root'),
    isDEBUG = cms.bool(False)
)
