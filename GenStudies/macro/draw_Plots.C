#include "TFile.h"
#include "TStyle.h"
#include "TH1.h"
#include "TH2.h"
#include "TTree.h" 
#include "TCanvas.h"
#include "TLegend.h"
#include "TColor.h"
#include "TLatex.h"
#include "TGraphAsymmErrors.h"
#include "TGaxis.h"
#include "TPaletteAxis.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TKey.h"

#include "../bin/FPCanvasStyle.C"
#include "../bin/setStyle.C"

#include<iostream>
#include<string>
#include<fstream>

using namespace std;

void compareHistos(TH1F* h, TH1F* h2, string outputDir, string unit, string label1, string label2);

void draw_Plots() {
  
  gStyle->SetOptStat(0);

  // inputs
  //  TFile* inFile1 = TFile::Open("/afs/cern.ch/user/b/bmarzocc/www/GluGluHToGG_M-125_13TeV_powheg_pythia8/GEN/genParticles/GenParticlesAnalysis.root");
  //  TFile* inFile2 = TFile::Open("/afs/cern.ch/user/b/bmarzocc/www/GluGluHToGX_M_125_TuneCUEP8M1_13TeV_pythia8_RunIIFall17/GEN-SIM/genParticles/GenParticlesAnalysis.root");
  TFile* inFile1 = TFile::Open("/home/gouranga/Downloads/temp/histos_photon_26x.root");
  TFile* inFile2 = TFile::Open("/home/gouranga/Downloads/temp/histos_photon_24x.root");
  vector<TH1F*> histos_f1;
  vector<TH1F*> histos_f2;

  TH1F* h_tmp;

  TList* list = inFile1->GetListOfKeys() ;
  if (!list) { printf("<E> No keys found in file\n") ; exit(1) ; }
  TIter next(list) ;
  TKey* key ;
  TObject* obj ;
      
  while ( (key = (TKey*)next()) ) {
    obj = key->ReadObj() ;
    if (    (strcmp(obj->IsA()->GetName(),"TProfile")!=0)
         && (!obj->InheritsFrom("TH2"))
	 && (!obj->InheritsFrom("TH1")) 
       ) {
      printf("<W> Object %s is not 1D or 2D histogram : "
             "will not be converted\n",obj->GetName()) ;
    }
    //printf("Histo name:%s title:%s\n",obj->GetName(),obj->GetTitle());
    cout << "obj->IsA()  " << obj->IsA()->GetName() << endl; // TTree
    if(obj->InheritsFrom("TTree")) continue;
    h_tmp = (TH1F*)inFile1->Get(obj->GetName());
    histos_f1.push_back(h_tmp);
  }
  
  list = inFile2->GetListOfKeys() ;
  if (!list) { printf("<E> No keys found in file\n") ; exit(1) ; }
  TIter next2(list) ;
    
  while ( (key = (TKey*)next2()) ) {
    obj = key->ReadObj() ;
    if (    (strcmp(obj->IsA()->GetName(),"TProfile")!=0)
         && (!obj->InheritsFrom("TH2"))
	 && (!obj->InheritsFrom("TH1")) 
       ) {
      printf("<W> Object %s is not 1D or 2D histogram : "
             "will not be converted\n",obj->GetName()) ;
    }
    printf("Histo name:%s title:%s\n",obj->GetName(),obj->GetTitle());
    if(obj->InheritsFrom("TTree")) continue;
    h_tmp = (TH1F*)inFile2->Get(obj->GetName());
    histos_f2.push_back(h_tmp);
  }

  
  for(unsigned int ii=0; ii<histos_f1.size(); ii++)
      for(unsigned int jj=0; jj<histos_f2.size(); jj++) {
          if(string(histos_f1.at(ii)->GetName()) != string(histos_f2.at(jj)->GetName())) continue;
          compareHistos(histos_f1.at(ii), histos_f2.at(jj), string("../output"), string("GeV"), string("MG_26x"), string("MG_24x"));
      }
  
}

void compareHistos(TH1F* h, TH1F* h2, string outputDir = "../output", string unit = "GeV", string label1 = "file1", string label2 = "file2")
{
     h->SetLineColor(kBlack);
     h->SetLineWidth(2);
     h2->SetLineColor(kRed);
     h2->SetLineWidth(2);

     h->Scale(h2->Integral()/h->Integral());

     //cout << "h->GetYaxis()->Get"
     // plotting
     setStyle();
     /*
     int nbins = h->GetNbinsX();
     float xmin = h->GetBinCenter(1)-h->GetBinWidth(1)/2;
     float xmax = h->GetBinCenter(nbins)+h->GetBinWidth(nbins)/2;
     float ymax = 1.1*h->GetBinContent(h->GetMaximumBin());
     TH2F* H2 = new TH2F("H2","",nbins,xmin,xmax,1000,0.,ymax);
     if(string(h->GetName()).find("eta")!=std::string::npos || string(h->GetName()).find("phi")!=std::string::npos || string(h->GetName()).find("delta")!=std::string::npos || string(h->GetName()).find("Fraction")!=std::string::npos) H2->GetXaxis()->SetTitle((string(h->GetName())).c_str());
     else H2->GetXaxis()->SetTitle((string(h->GetName())+" ("+unit+")").c_str());
     H2->GetYaxis()->SetTitle("events");

     float maximum = h->GetMaximum();
     if(maximum < h2->GetMaximum()) maximum = h2->GetMaximum();
     H2->GetYaxis()->SetRangeUser(0.9,1.01*maximum);
     */
     TLegend *leg;
     leg = new TLegend(0.68,0.70,0.88,0.90);
     leg->SetFillStyle(0);
     leg->SetBorderSize(0);
     leg->SetTextSize(0.03);
     leg->SetFillColor(0);
     leg->AddEntry(h, label1.c_str(), "l");
     leg->AddEntry(h2, label2.c_str(), "l");
     
     TCanvas* c1 = new TCanvas("c1","c1",1);
     FPCanvasStyle(c1);
     //H2->Draw();
     h2->Draw("hist");
     h->Draw("hist Same");
     leg->Draw("");
     c1->SaveAs((outputDir+"/"+string(h->GetName())+".png").c_str());
     c1->SaveAs((outputDir+"/"+string(h->GetName())+".pdf").c_str());
     
     //TCanvas* c2 = new TCanvas("c2","c2",1);
     /*
     FPCanvasStyle(c2);
     c2->SetLogy();
     H2->Draw();
     h2->Draw("H,same");
     h->Draw("H,same");
     leg->Draw("same");
     c2->SaveAs((outputDir+"/"+string(h->GetName())+"_log.png").c_str());
     c2->SaveAs((outputDir+"/"+string(h->GetName())+"_log.pdf").c_str());
     */
     delete c1;
     //delete c2;
     //delete H2;
     delete leg;
}

