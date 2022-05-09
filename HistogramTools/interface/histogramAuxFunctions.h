#ifndef TallinnAnalysis_HistogramTools_histogramAuxFunctions_h
#define TallinnAnalysis_HistogramTools_histogramAuxFunctions_h

// forward declarations
class TH1;
class TH2;

void
fill(TH1 * histogram,
     double x,
     double evtWeight,
     double evtWeightErr = 0.);

void
fillWithOverFlow(TH1* histogram,
                 double x,
                 double evtWeight,
                 double evtWeightErr = 0.);

void
fill2d(TH2 * histogram,
       double x,
       double y,
       double evtWeight,
       double evtWeightErr = 0.);

void
fillWithOverFlow2d(TH2* histogram,
                   double x,
                   double y,
                   double evtWeight,
                   double evtWeightErr = 0.);

#endif // TallinnAnalysis_HistogramTools_histogramAuxFunctions_h
