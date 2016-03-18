#ifndef GH2BASE__H
#define GH2BASE__H

#include <cstdio>
#include <map>

#include <TNamed.h>
#include <TH2.h>
#include <TList.h>

class GH1D;

enum kBackgroundSubtraction {
  kNoBackground,
  kRegionBackground,
  kMatchedLowerMarker,
  kSplitTwoMarker,
  kTotalFraction
};

enum kDirection{
  kXDirection,
  kYDirection
};

class GH2Base {

public:
  GH2Base() { Init(); }
  
  virtual ~GH2Base();
 
  virtual void GH2Clear(Option_t *opt="");

  virtual TH2 *GetTH2() = 0;

  GH1D* Projection_Background(int axis=0,
                              int firstbin = 0,
                              int lastbin = -1,
                              int firstbackground_bin = 0,
                              int lastbackground_bin = -1,
                              kBackgroundSubtraction mode = kRegionBackground);

  GH1D* SummaryProject(int binnum);

  GH1D* ProjectionX_Background(int firstbin = 0,
                               int lastbin = -1,
                               int firstbackground_bin = 0,
                               int lastbackground_bin = -1,
                               kBackgroundSubtraction mode = kRegionBackground); // *MENU*
  
  GH1D* GH2ProjectionX(const char* name="_px",
                    int firstbin = 0,
                    int lastbin = -1,
                    Option_t* option=""); // *MENU*

  GH1D* GH2ProjectionY(const char* name="_py",
                    int firstbin = 0,
                    int lastbin = -1,
                    Option_t* option=""); // *MENU*
  
  GH1D* ProjectionY_Background(int firstbin = 0,
                               int lastbin = -1,
                               int firstbackground_bin = 0,
                               int lastbackground_bin = -1,
                               kBackgroundSubtraction mode = kRegionBackground); // *MENU*

  GH1D* GetPrevious(const GH1D* curr);
  GH1D* GetPrevSummary(const GH1D* curr);
  GH1D* GetNext(const GH1D* curr);
  GH1D* GetNextSummary(const GH1D* curr);

  TList* GetProjections() { return fProjections; }
  TList* GetSummaryProjections() { return fSummaryProjections; }

  void SetSummary(bool is_summary = true) { fIsSummary = is_summary; }
  bool GetSummary() const { return fIsSummary; }

  void SetSummaryDirection(kDirection dir) { fSummaryDirection = dir; }
  kDirection GetSummaryDirection() const { return fSummaryDirection; }

private:
  void Init();
  TList* fProjections;

  TList* fSummaryProjections; //!
  bool fIsSummary;
  kDirection fSummaryDirection;

  ClassDef(GH2Base,1)
};

#endif
