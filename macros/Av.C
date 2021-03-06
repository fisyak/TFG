#include "TString.h"
#include "Riostream.h"
#include "TMath.h"
using namespace std;
//________________________________________________________________________________
void Av() {
#if 0
  const Char_t *Lines[36] = {
    "2003;WA; -0.2426+/- 0.0020;-0.1854+/- 0.0015;  54.5509+/- 0.0012;  -0.01+/- 0.03", //Old
    "2003;WB; -0.2421+/- 0.0019;-0.1884+/- 0.0015; 118.7541+/- 0.0012;  -0.04+/- 0.02",
    "2003;WD; -0.2450+/- 0.0032;-0.1737+/- 0.0023; 190.5694+/- 0.0020;  -0.03+/- 0.02",
    "2003;EA; -0.2518+/- 0.0011;-0.1624+/- 0.0009;  54.5577+/- 0.0007;   0.40+/- 0.04",
    "2003;EB; -0.2519+/- 0.0014;-0.1670+/- 0.0010; 118.7556+/- 0.0008;   0.36+/- 0.04",
    "2003;ED; -0.2603+/- 0.0018;-0.1685+/- 0.0014; 190.5796+/- 0.0011;   0.32+/- 0.05",

    "2004;WA; -0.2125+/- 0.0020;-0.1542+/- 0.0015;  54.5506+/- 0.0012;   0.15+/- 0.03",
    "2004;WB; -0.2114+/- 0.0019;-0.1569+/- 0.0014; 118.7537+/- 0.0012;   0.12+/- 0.02",
    "2004;WD; -0.2148+/- 0.0031;-0.1430+/- 0.0022; 190.5686+/- 0.0019;   0.12+/- 0.03",
    "2004;EA; -0.2228+/- 0.0010;-0.1296+/- 0.0008;  54.5577+/- 0.0006;   0.57+/- 0.05",
    "2004;EB; -0.2223+/- 0.0013;-0.1327+/- 0.0010; 118.7563+/- 0.0008;   0.52+/- 0.05",
    "2004;ED; -0.2303+/- 0.0017;-0.1363+/- 0.0013; 190.5793+/- 0.0011;   0.47+/- 0.07",

    "2013;WA; -0.2297+/- 0.0042;-0.1780+/- 0.0044;  54.5527+/- 0.0030;  -0.01+/- 0.03",
    "2013;WB; -0.2289+/- 0.0041;-0.1817+/- 0.0041; 118.7558+/- 0.0029;  -0.05+/- 0.02",
    "2013;WD; -0.2297+/- 0.0060;-0.1686+/- 0.0053; 190.5705+/- 0.0041;  -0.04+/- 0.02",
    "2013;EA; -0.2231+/- 0.0041;-0.1794+/- 0.0041;  54.5589+/- 0.0029;   0.40+/- 0.04",
    "2013;EB; -0.2287+/- 0.0042;-0.1852+/- 0.0044; 118.7620+/- 0.0030;   0.35+/- 0.04",
    "2013;ED; -0.2319+/- 0.0063;-0.1863+/- 0.0047; 190.5802+/- 0.0038;   0.32+/- 0.05",

    "2003;WA;  0.0064+/- 0.0020;-0.0111+/- 0.0015;  54.5509+/- 0.0012;  -0.15+/- 0.03",//New
    "2003;WB;  0.0069+/- 0.0019;-0.0141+/- 0.0015; 118.7541+/- 0.0012;  -0.19+/- 0.02",
    "2003;WD;  0.0040+/- 0.0032; 0.0006+/- 0.0023; 190.5694+/- 0.0020;  -0.17+/- 0.03",
    "2003;EA; -0.0028+/- 0.0011; 0.0119+/- 0.0009;  54.5577+/- 0.0007;   0.26+/- 0.03",
    "2003;EB; -0.0029+/- 0.0014; 0.0073+/- 0.0010; 118.7556+/- 0.0008;   0.21+/- 0.03",
    "2003;ED; -0.0113+/- 0.0018; 0.0058+/- 0.0014; 190.5796+/- 0.0011;   0.18+/- 0.03",

    "2004;WA;  0.0065+/- 0.0020;-0.0121+/- 0.0015;  54.5506+/- 0.0012;  -0.15+/- 0.03",
    "2004;WB;  0.0076+/- 0.0019;-0.0148+/- 0.0014; 118.7537+/- 0.0012;  -0.19+/- 0.02",
    "2004;WD;  0.0042+/- 0.0031;-0.0008+/- 0.0022; 190.5686+/- 0.0019;  -0.17+/- 0.03",
    "2004;EA; -0.0038+/- 0.0010; 0.0125+/- 0.0008;  54.5577+/- 0.0006;   0.26+/- 0.04",
    "2004;EB; -0.0033+/- 0.0013; 0.0093+/- 0.0010; 118.7563+/- 0.0008;   0.22+/- 0.03",
    "2004;ED; -0.0112+/- 0.0017; 0.0058+/- 0.0013; 190.5793+/- 0.0011;   0.19+/- 0.03",

    "2013;WA; -0.0011+/- 0.0042; 0.0019+/- 0.0044;  54.5527+/- 0.0030;  -0.16+/- 0.03",
    "2013;WB; -0.0003+/- 0.0041;-0.0018+/- 0.0041; 118.7558+/- 0.0029;  -0.19+/- 0.02",
    "2013;WD; -0.0011+/- 0.0060; 0.0113+/- 0.0053; 190.5705+/- 0.0041;  -0.18+/- 0.03",
    "2013;EA;  0.0055+/- 0.0041; 0.0005+/- 0.0041;  54.5589+/- 0.0029;   0.25+/- 0.03",
    "2013;EB; -0.0001+/- 0.0042;-0.0053+/- 0.0044; 118.7620+/- 0.0030;   0.20+/- 0.03",
    "2013;ED; -0.0033+/- 0.0063;-0.0064+/- 0.0047; 190.5802+/- 0.0038;   0.18+/- 0.03"
  };
  const Char_t *FMTI = "%*8s %f+/-%f;%f+/-%f;%f+/-%f;%f+/-%f";
  Int_t NON = 2;
#else
  const Char_t *Lines[18] = {
    "TpcWAO22003  x0 =  0.0006 +/-  0.0020 y0 = -0.0029 +/-  0.0015 R = 54.5509 +/-  0.0012 gamma =  0.00 +/-  0.03",
    "TpcWBO12003  x0 =  0.0011 +/-  0.0019 y0 = -0.0059 +/-  0.0015 R =118.7541 +/-  0.0012 gamma = -0.03 +/-  0.02",
    "TpcWDO12003  x0 = -0.0018 +/-  0.0032 y0 =  0.0088 +/-  0.0023 R =190.5694 +/-  0.0020 gamma = -0.02 +/-  0.02",

    "TpcEAO132003 x0 =  0.0029 +/-  0.0011 y0 =  0.0036 +/-  0.0009 R = 54.5577 +/-  0.0007 gamma =  0.05 +/-  0.03",
    "TpcEBO132003 x0 =  0.0028 +/-  0.0014 y0 = -0.0010 +/-  0.0010 R =118.7556 +/-  0.0008 gamma =  0.01 +/-  0.02",
    "TpcEDO132003 x0 = -0.0056 +/-  0.0018 y0 = -0.0025 +/-  0.0014 R =190.5796 +/-  0.0011 gamma = -0.01 +/-  0.02",

    "TpcWAO22004  x0 =  0.0004 +/-  0.0020 y0 = -0.0029 +/-  0.0015 R = 54.5506 +/-  0.0012 gamma =  0.01 +/-  0.02",
    "TpcWBO12004  x0 =  0.0015 +/-  0.0019 y0 = -0.0056 +/-  0.0014 R =118.7537 +/-  0.0012 gamma = -0.03 +/-  0.02",
    "TpcWDO12004  x0 = -0.0019 +/-  0.0031 y0 =  0.0083 +/-  0.0022 R =190.5686 +/-  0.0019 gamma = -0.02 +/-  0.02",

    "TpcEAO132004 x0 =  0.0023 +/-  0.0010 y0 =  0.0033 +/-  0.0008 R = 54.5577 +/-  0.0006 gamma =  0.06 +/-  0.03",
    "TpcEBO132004 x0 =  0.0028 +/-  0.0013 y0 =  0.0002 +/-  0.0010 R =118.7563 +/-  0.0008 gamma =  0.02 +/-  0.02",
    "TpcEDO132004 x0 = -0.0052 +/-  0.0017 y0 = -0.0034 +/-  0.0013 R =190.5793 +/-  0.0011 gamma = -0.01 +/-  0.02",

    "TpcWAO022013 x0 = -0.0003 +/-  0.0042 y0 = -0.0019 +/-  0.0044 R = 54.5527 +/-  0.0030 gamma =  0.01 +/-  0.03",
    "TpcWBO012013 x0 =  0.0005 +/-  0.0041 y0 = -0.0056 +/-  0.0041 R =118.7558 +/-  0.0029 gamma = -0.03 +/-  0.02",
    "TpcWDO012013 x0 = -0.0003 +/-  0.0060 y0 =  0.0075 +/-  0.0053 R =190.5705 +/-  0.0041 gamma = -0.02 +/-  0.02",

    "TpcEAO132013 x0 =  0.0048 +/-  0.0041 y0 =  0.0042 +/-  0.0041 R = 54.5589 +/-  0.0029 gamma =  0.06 +/-  0.03",
    "TpcEBO132013 x0 = -0.0008 +/-  0.0042 y0 = -0.0016 +/-  0.0044 R =118.7620 +/-  0.0030 gamma =  0.01 +/-  0.02",
    "TpcEDO132013 x0 = -0.0040 +/-  0.0063 y0 = -0.0027 +/-  0.0047 R =190.5802 +/-  0.0038 gamma = -0.01 +/-  0.02"};
  const Char_t *FMTI = "%*12s x0 = %f +/- %f y0 = %f +/- %f R =%f +/- %f gamma = %f +/- %f";
  Int_t NON = 1;
#endif
  Float_t x, dx, y, dy, r, dr, g, dg;
  Int_t i = 0;
  const Char_t *FMT = "%8.4f+/-%7.4f;%7.4f+/-%7.4f; N/A               ;%7.2f+/-%5.2f";
  for (Int_t on = 0; on < NON; on++) {// old new
    for (Int_t Y = 0; Y < 3; Y++) {// 2003 2004 2013
      Int_t NTpc = 0;
      Double_t xTpcW = 0, yTpcW = 0, gTpcW = 0;
      Double_t xTpcWW = 0, yTpcWW = 0, gTpcWW = 0;
      for (Int_t s = 0; s < 2; s++) {// West East
	Int_t N = 0;
	Double_t xW = 0, yW = 0, gW = 0;
	Double_t xWW = 0, yWW = 0, gWW = 0;
	for (Int_t j = 0; j < 3; j++, i++) {
#if 0
	  if (! on ) cout << "Old\t";
	  else       cout << "New\t";
#endif
	  cout << Lines[i] << endl;
	  //	  Int_t n = 
	  sscanf(Lines[i],FMTI,&x,&dx,&y,&dy,&r,&dr,&g,&dg);
	  //	  cout << n << "\t" << Form(" %f+/-%f;%f+/-%f;%f+/-%f;%f+/-%f\n",x,dx,y,dy,r,dr,g,dg);
#if 0
	  xW  +=  x/(dx*dx);
	  xWW += 1./(dx*dx);
	  yW  +=  y/(dy*dy);
	  yWW += 1./(dy*dy);
	  gW  +=  g/(dg*dg);
	  gWW += 1./(dg*dg);
#else
          N++; NTpc++;
	  xW += x;
	  yW += y;
	  gW += g;
	  xTpcW += x;
	  yTpcW += y;
	  gTpcW += g;
#endif
	}
#if 0
	xW = xW/xWW; xWW = 1./TMath::Sqrt(xWW);
	yW = yW/yWW; yWW = 1./TMath::Sqrt(yWW);
	gW = gW/gWW; gWW = 1./TMath::Sqrt(gWW);
#else
	xW /= N;
	yW /= N;
	gW /= N;
#endif
#if 0
	if (! on) cout << "Old\t";
	else      cout << "New\t";
#endif
	cout << "Av;";
	if (! s) cout << "West;";
	else     cout << "East;";
	cout << Form(FMT,xW,xWW,yW,yWW,gW,gWW) << endl;
#if 0
	xTpcW += xW/(xWW*xWW); xTpcWW += 1./(xWW*xWW);
	yTpcW += yW/(yWW*yWW); yTpcWW += 1./(yWW*yWW);
	gTpcW += gW/(gWW*gWW); gTpcWW += 1./(gWW*gWW);
#endif
      }
#if 0
      if (! on) cout << "Old\t";
      else      cout << "New\t";
#endif
      cout << "Av;Tpc ;";
#if 0
      xTpcW = xTpcW/xTpcWW; xTpcWW = 1./TMath::Sqrt(xTpcWW);
      yTpcW = yTpcW/yTpcWW; yTpcWW = 1./TMath::Sqrt(yTpcWW);
      gTpcW = gTpcW/gTpcWW; gTpcWW = 1./TMath::Sqrt(gTpcWW);
#else
      xTpcW /= NTpc;
      yTpcW /= NTpc;
      gTpcW /= NTpc;
#endif
      cout << Form(FMT,xTpcW,xTpcWW,yTpcW,yTpcWW,gTpcW,gTpcWW) << endl;
    }
  }
}
