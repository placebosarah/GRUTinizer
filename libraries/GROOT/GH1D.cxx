#include "GH1D.h"

#include <iostream>
#include <fstream>
#include <cstring>

#include "TVirtualPad.h"
#include "TString.h"
#include "TF1.h"
#include "TFrame.h"
#include "TClass.h"
#include "TFunction.h"
#include "TMethodCall.h"
//#include "TROOT.h"
//#include "TSystem.h"

#include "GCanvas.h"
#include "GH2I.h"
#include "GH2D.h"
#include "GRootCommands.h"

GH1D::GH1D(const TH1& source)
  : parent(NULL), projection_axis(-1),fFillClass(0),fFillMethod(0)  {
  source.Copy(*this);
}


GH1D::GH1D(const TF1& function,Int_t nbinsx,Double_t xlow,Double_t xup) :
  TH1D(Form("%s_hist",function.GetName()),Form("%s_hist",function.GetName()),nbinsx, xlow, xup), parent(NULL), projection_axis(-1),fFillClass(0),fFillMethod(0) {

  //TF1 *f = (TF1*)function.Clone();
  //f->SetRange(xlow,xup);

  for(int i=1;i<=nbinsx;i++) {
    double x = GetBinCenter(i);
    SetBinContent(i,function.Eval(x));
  }
  //f->Delete();
}

bool GH1D::WriteDatFile(const char *outFile){
  if(strlen(outFile)<1) return 0;

  std::ofstream out;
  out.open(outFile);

  if(!(out.is_open())) return 0;

  for(int i=0;i<GetNbinsX();i++){
    out << GetXaxis()->GetBinCenter(i) << "\t" << GetBinContent(i) << std::endl;
  }
  out << std::endl;
  out.close();

  return 1;
}


/*
GH1D::GH1D(const TH1 *source)
  : parent(NULL), projection_axis(-1) {
  if(source->GetDiminsion()>1) {
    return;
  }

  // Can copy from any 1-d TH1, not just a TH1D
  source->Copy(*this);

  // Force a refresh of any parameters stored in the option string.
  SetOption(GetOption());
}

void GH1D::SetOption(Option_t* opt) {
  fOption = opt;

  TString sopt = opt;
  if(sopt.Index("axis:")) {
    projection_axis = 0;// TODO
  }
}
*/

void GH1D::Clear(Option_t* opt) {
  TH1D::Clear(opt);
  parent = NULL;
}

void GH1D::Print(Option_t* opt) const {
  TH1D::Print(opt);
  std::cout << "\tParent: " << parent.GetObject() << std::endl;
}

void GH1D::Copy(TObject& obj) const {
  TH1D::Copy(obj);

  ((GH1D&)obj).parent = parent;
}


void GH1D::Draw(Option_t* opt) {
  TString option(opt);
  if(option.Contains("new",TString::kIgnoreCase)) {
    option.ReplaceAll("new","");
    new GCanvas;
  }
  TH1D::Draw(option.Data());
  if(gPad) {
    gPad->Update();
    gPad->GetFrame()->SetBit(TBox::kCannotMove);
  }
}

TH1 *GH1D::DrawCopy(Option_t *opt) const {
  TH1 *h = TH1D::DrawCopy(opt);
  if(gPad) {
    gPad->Update();
    gPad->GetFrame()->SetBit(TBox::kCannotMove);
  }
  return h;
}

TH1 *GH1D::DrawNormalized(Option_t *opt,Double_t norm) const {
  TH1 *h = TH1D::DrawNormalized(opt,norm);
  if(gPad) {
    gPad->Update();
    gPad->GetFrame()->SetBit(TBox::kCannotMove);
  }
  return h;
}



GH1D* GH1D::GetPrevious(bool DrawEmpty) const {
  if(parent.GetObject() && parent.GetObject()->InheritsFrom(GH2Base::Class())) {
    GH2D* gpar = (GH2D*)parent.GetObject();
    int first = GetXaxis()->GetFirst();
    int last =  GetXaxis()->GetLast();
    GH1D *prev = gpar->GetPrevious(this,DrawEmpty);
    prev->GetXaxis()->SetRange(first,last);
    return prev; //gpar->GetPrevious(this,DrawEmpty);
  } else {
    return NULL;
  }
}

GH1D* GH1D::GetNext(bool DrawEmpty) const {
  if(parent.GetObject() && parent.GetObject()->InheritsFrom(GH2Base::Class())) {
    GH2D* gpar = (GH2D*)parent.GetObject();
    int first = GetXaxis()->GetFirst();
    int last =  GetXaxis()->GetLast();
    GH1D *next = gpar->GetNext(this,DrawEmpty);
    next->GetXaxis()->SetRange(first,last);
    return next; //gpar->GetNext(this,DrawEmpty);
  } else {
    return NULL;
  }
}

GH1D* GH1D::Project(double value_low, double value_high) const {

  if(parent.GetObject() && parent.GetObject()->InheritsFrom(GH2Base::Class()) &&
     projection_axis!=-1) {
    if(value_low > value_high){
      std::swap(value_low, value_high);
    }
    GH2D* gpar = (GH2D*)parent.GetObject();
    if(projection_axis == 0){
      int bin_low  = gpar->GetXaxis()->FindBin(value_low);
      int bin_high = gpar->GetXaxis()->FindBin(value_high);
      return gpar->ProjectionY("_py", bin_low, bin_high);
    } else {
      int bin_low  = gpar->GetYaxis()->FindBin(value_low);
      int bin_high = gpar->GetYaxis()->FindBin(value_high);
      return gpar->ProjectionX("_px", bin_low, bin_high);
    }
  } else {
    return NULL;
  }
}

GH1D* GH1D::Project_Background(double value_low, double value_high,
                               double bg_value_low, double bg_value_high,
                               kBackgroundSubtraction mode) const {
  if(parent.GetObject() && parent.GetObject()->InheritsFrom(GH2Base::Class()) &&
     projection_axis!=-1) {
    if(value_low > value_high){
      std::swap(value_low, value_high);
    }
    if(bg_value_low > bg_value_high){
      std::swap(bg_value_low, bg_value_high);
    }

    GH2D* gpar = (GH2D*)parent.GetObject();
    if(projection_axis == 0){
      int bin_low     = gpar->GetXaxis()->FindBin(value_low);
      int bin_high    = gpar->GetXaxis()->FindBin(value_high);
      int bg_bin_low  = gpar->GetXaxis()->FindBin(bg_value_low);
      int bg_bin_high = gpar->GetXaxis()->FindBin(bg_value_high);

      return gpar->ProjectionY_Background(bin_low, bin_high,
                                          bg_bin_low, bg_bin_high,
                                          mode);
    } else {
      int bin_low     = gpar->GetYaxis()->FindBin(value_low);
      int bin_high    = gpar->GetYaxis()->FindBin(value_high);
      int bg_bin_low  = gpar->GetYaxis()->FindBin(bg_value_low);
      int bg_bin_high = gpar->GetYaxis()->FindBin(bg_value_high);

      return gpar->ProjectionX_Background(bin_low, bin_high,
                                          bg_bin_low, bg_bin_high,
                                          mode);
    }
  } else {
    return NULL;
  }
}

GH1D *GH1D::Project(int bins) {
  GH1D *proj = 0;
  double ymax = GetMinimum();
  double ymin = GetMaximum();
  if(bins==-1) {
    bins = abs(ymax-ymin);
    if(bins<1)
      bins=100;
  }
  proj = new GH1D(Form("%s_y_axis_projection",GetName()),
                  Form("%s_y_axis_projection",GetName()),
                  bins,ymin,ymax);
  for(int x=0;x<GetNbinsX();x++) {
    if(GetBinContent(x)!=0)
      proj->Fill(GetBinContent(x));
  }

  return proj;
}

void GH1D::SetFillMethod(const char *classname,const char *methodname,const char *param) {
  fFillClass = TClass::GetClass(classname);
  if(!fFillClass)
    return;
  fFillMethod = new TMethodCall(fFillClass,methodname,param);
  //printf("class:  %s\n",fFillClass->GetName());
  //printf("method: %s\n",fFillMethod->GetMethod()->GetPrototype());
}


Int_t GH1D::Fill(const TObject* obj) {
  if(!fFillClass || !fFillMethod) {
    //printf("%p \t %p\n",fFillClass,fFillMethod);
    return -1;
  }
  if(obj->IsA()!=fFillClass) {
    //printf("%s \t %s\n", obj->Class()->GetName(),fFillClass->GetName());
    return -2;
  }
  Double_t storage;
  fFillMethod->Execute((void*)(obj),storage);
  return Fill(storage);
}



GPeak* GH1D::DoPhotoPeakFit(double xlow,double xhigh,Option_t *opt) {
  xl_last = xlow;
  xh_last = xhigh;
  return PhotoPeakFit((TH1*)this,xlow,xhigh,opt);
}


GPeak* GH1D::DoPhotoPeakFitNormBG(double xlow,double xhigh,Option_t *opt) {
  xl_last = xlow;
  xh_last = xhigh;
  return PhotoPeakFitNormBG((TH1*)this,xlow,xhigh,opt);
}
