#ifndef TallinnAnalysis_HistogramTools_fillWithOverFlow_h
#define TallinnAnalysis_HistogramTools_fillWithOverFlow_h

// forward declarations
class TH1;
class TH2;

void
fill1D(TH1 * histogram,
       double x,
       double evtWeight);

void
fillWithOverFlow1D(TH1* histogram,
                   double x,
                   double evtWeight);

void
fill2D(TH2 * histogram,
       double x,
       double y,
       double evtWeight);

void
fillWithOverFlow2D(TH2* histogram,
                   double x,
                   double y,
                   double evtWeight);

#endif // TallinnAnalysis_HistogramTools_fillWithOverFlow_h
