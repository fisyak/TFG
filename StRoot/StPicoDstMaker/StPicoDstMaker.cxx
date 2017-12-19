#include <algorithm>
#include <unordered_map>
#include <string>
#include <vector>

#include "TRegexp.h"
#include "TChain.h"
#include "TClonesArray.h"
#include "TTree.h"
#include "TBranch.h"
#include "TObjectSet.h"
#include "TSystem.h"
#include "StChain/StChain.h"
#include "StChain/StChainOpt.h"
#include "St_base/StMessMgr.h"
#include "StarRoot/TAttr.h"

#include "StEvent/StBTofHeader.h"
#include "StEvent/StDcaGeometry.h"
#include "StEvent/StEmcCollection.h"
#include "StEvent/StEmcCluster.h"
#include "StEvent/StEmcDetector.h"
#include "StEvent/StEmcModule.h"
#include "StEvent/StEmcRawHit.h"

#include "StMuDSTMaker/COMMON/StMuDst.h"
#include "StMuDSTMaker/COMMON/StMuEvent.h"
#include "StMuDSTMaker/COMMON/StMuBTofHit.h"
#include "StMuDSTMaker/COMMON/StMuTrack.h"
#include "StMuDSTMaker/COMMON/StMuPrimaryVertex.h"
#include "StMuDSTMaker/COMMON/StMuMtdHit.h"
#include "StMuDSTMaker/COMMON/StMuMtdPidTraits.h"
#include "StMuDSTMaker/COMMON/StMuEmcCollection.h"
#include "StMuDSTMaker/COMMON/StMuEmcPoint.h"
#include "StMuDSTMaker/COMMON/StMuFmsUtil.h"
#include "StMuDSTMaker/COMMON/StMuMcTrack.h"
#include "StMuDSTMaker/COMMON/StMuMcVertex.h"

#include "StTriggerUtilities/StTriggerSimuMaker.h"
#include "StTriggerUtilities/Bemc/StBemcTriggerSimu.h"

#include "StEmcUtil/geometry/StEmcGeom.h"
#include "StEmcUtil/others/emcDetectorName.h"
#include "StEmcUtil/projection/StEmcPosition.h"
#include "StEmcRawMaker/defines.h"
#include "StEmcRawMaker/StBemcTables.h"

#include "StFmsDbMaker/StFmsDbMaker.h"

#include "tables/St_mtdModuleToQTmap_Table.h"
#include "tables/St_mtdQTSlewingCorr_Table.h"
#include "tables/St_mtdQTSlewingCorrPart2_Table.h"

#include "StPicoEvent/StPicoEvent.h"
#include "StPicoEvent/StPicoTrack.h"
#include "StPicoEvent/StPicoEmcTrigger.h"
#include "StPicoEvent/StPicoMtdTrigger.h"
#include "StPicoEvent/StPicoBbcTile.h"
#include "StPicoEvent/StPicoEpdTile.h"
#include "StPicoEvent/StPicoBTowHit.h"
#include "StPicoEvent/StPicoBTofHit.h"
#include "StPicoEvent/StPicoMtdHit.h"
#include "StPicoEvent/StPicoFmsHit.h"
#include "StPicoEvent/StPicoBEmcPidTraits.h"
#include "StPicoEvent/StPicoBTofPidTraits.h"
#include "StPicoEvent/StPicoMtdPidTraits.h"
#include "StPicoDstMaker/StPicoDstMaker.h"
#include "StPicoDstMaker/StPicoArrays.h"
#include "StPicoDstMaker/StPicoDst.h"
#include "TH1.h"
#include "TH2.h"
//#define __HIST_PV__
#ifdef __HIST_PV__
static TH1F *hists[3] = {0};
static TH2F *pVrZ = 0;
static TH2F *pVxy = 0;
#endif /* __HIST_PV__ */
static Int_t _debug = 0;
Double_t StPicoDstMaker::fgerMax;    // 50 um
Double_t StPicoDstMaker::fgdca3Dmax; // 50 cm
vector<Int_t> StPicoDstMaker::fGoodTriggerIds;
Double_t StPicoDstMaker::fgVxXmin, StPicoDstMaker::fgVxXmax, StPicoDstMaker::fgVxYmin, StPicoDstMaker::fgVxYmax;
Double_t StPicoDstMaker::fgVxZmin, StPicoDstMaker::fgVxZmax, StPicoDstMaker::fgVxRmax;
//_____________________________________________________________________________
StPicoDstMaker::StPicoDstMaker(char const* name) : StMaker(name),
  mMuDst(nullptr), mPicoDst(new StPicoDst()),
  mEmcCollection(nullptr), mEmcPosition(nullptr),
  mEmcGeom{}, mEmcIndex{},
  mTpcVpdVzDiffCut(3),
  mBField(0),
  mVtxMode(PicoVtxMode::NotSet), // This should always be ::NotSet, do not change it, see ::Init()
  mInputFileName(), mOutputFileName(), mOutputFile(nullptr),
  mChain(nullptr), mTTree(nullptr), mEventCounter(0), mSplit(99), mCompression(9), mBufferSize(65536 * 4),
  mModuleToQT{}, mModuleToQTPos{}, mQTtoModule{}, mQTSlewBinEdge{}, mQTSlewCorr{},
  mPicoArrays{}, mStatusArrays{},
  mBbcFiller(*mPicoDst),
  mEpdFiller(*mPicoDst),
  mFmsFiller(*mPicoDst)
{
  streamerOff();
  createArrays();
  LOG_INFO << "StPicoDstMaker::StPicoDstMaker: Set Default cuts" << endm;
  SetGoodTriggers("520001, 520011, 520021, 520031, 520041, 520051");
  SetMaxTrackDca(50);
  SetMaxVertexTransError(0);
  SetVxXYrange(0,0,0,0);
  SetVxZrange(0,0);
  SetVxRmax(0);
  std::fill_n(mStatusArrays, sizeof(mStatusArrays) / sizeof(mStatusArrays[0]), 1);
}
//_____________________________________________________________________________
StPicoDstMaker::StPicoDstMaker(Int_t ioMode, char const* fileName, char const* name) : StPicoDstMaker(name)
{
  StMaker::m_Mode = ioMode;
  mInputFileName = fileName;
}

//_________________
StPicoDstMaker::~StPicoDstMaker()
{
  delete mChain;
  delete mPicoDst;
}

//_________________
void StPicoDstMaker::clearArrays()
{
  for (int i = 0; i < StPicoArrays::NAllPicoArrays; ++i)
  {
    mPicoArrays[i]->Clear();
  }
}


/**
 * Allows one to disable/enable branches matching a simple regex pattern
 * `branchNameRegex` when reading picoDst.root files. For example,
 *
 * ~~~ {.cpp}
 * StPicoDstMaker::SetStatus("*", 0);      // Disables all branches
 * StPicoDstMaker::SetStatus("Emc*", 1);   // Enables branches starting with "Emc"
 * ~~~
 *
 * Note that if the first two characters in `branchNameRegex` are "St" they will
 * be ignored, i.e. "StBTof*" is the same as "BTof*".
 */
void StPicoDstMaker::SetStatus(char const* branchNameRegex, int enable)
{
  if (strncmp(branchNameRegex, "St", 2) == 0)
    branchNameRegex += 2; //Ignore first "St"

  TRegexp re(branchNameRegex, 1);
  for (int i = 0; i < StPicoArrays::NAllPicoArrays; ++i)
  {
    Ssiz_t len;
    if (re.Index(StPicoArrays::picoArrayNames[i], &len) < 0)   continue;
    LOG_INFO << "StPicoDstMaker::SetStatus " << enable << " to " << StPicoArrays::picoArrayNames[i] << endm;
    mStatusArrays[i] = enable;
  }

  if (StMaker::m_Mode == PicoIoMode::IoRead)
    setBranchAddresses(mChain);
}

//_________________
void StPicoDstMaker::setBranchAddresses(TChain* chain)
{
  if (!chain) return;
  chain->SetBranchStatus("*", 0);
  TString ts;
  for (int i = 0; i < StPicoArrays::NAllPicoArrays; ++i)
  {
    if (mStatusArrays[i] == 0) continue;
    char const* bname = StPicoArrays::picoArrayNames[i];
    TBranch* tb = chain->GetBranch(bname);
    if (!tb)
    {
      LOG_WARN << "setBranchAddress: Branch name " << bname << " does not exist!" << endm;
      continue;
    }
    ts = bname;
    ts += "*";
    chain->SetBranchStatus(ts, 1);
    chain->SetBranchAddress(bname, mPicoArrays + i);
    assert(tb->GetAddress() == (char*)(mPicoArrays + i));
  }
  mTTree = mChain->GetTree();
}

//_________________
void StPicoDstMaker::streamerOff()
{
  // This is to to save space on the file. No need for TObject bits for this structure.
  // see: https://root.cern.ch/doc/master/classTClass.html#a606b0442d6fec4b1cd52f43bca73aa51
  StPicoEvent::Class()->IgnoreTObjectStreamer();
  StPicoTrack::Class()->IgnoreTObjectStreamer();
  StPicoBTofHit::Class()->IgnoreTObjectStreamer();
  StPicoBTowHit::Class()->IgnoreTObjectStreamer();
  StPicoMtdHit::Class()->IgnoreTObjectStreamer();
  StPicoBbcTile::Class()->IgnoreTObjectStreamer();
  StPicoEpdTile::Class()->IgnoreTObjectStreamer();
  StPicoFmsHit::Class()->IgnoreTObjectStreamer();
  StPicoEmcTrigger::Class()->IgnoreTObjectStreamer();
  StPicoMtdTrigger::Class()->IgnoreTObjectStreamer();
  StPicoBTofPidTraits::Class()->IgnoreTObjectStreamer();
  StPicoBEmcPidTraits::Class()->IgnoreTObjectStreamer();
  StPicoMtdPidTraits::Class()->IgnoreTObjectStreamer();
}

//_________________
void StPicoDstMaker::createArrays()
{
  for (int i = 0; i < StPicoArrays::NAllPicoArrays; ++i)
  {
    mPicoArrays[i] = new TClonesArray(StPicoArrays::picoArrayTypes[i], StPicoArrays::picoArraySizes[i]);
  }

  mPicoDst->set(mPicoArrays);
}
//_____________________________________________________________________________
Int_t StPicoDstMaker::Init() {
  TString file;
  Int_t l;
  switch (StMaker::m_Mode)  {
  case PicoIoMode::IoWrite:
    if (mVtxMode == PicoVtxMode::NotSet) {
      if (setVtxModeAttr() != kStOK) {
	LOG_ERROR << "Pico Vertex Mode is not set ... " << endm;
	return kStErr;
      }
    }
    if (mInputFileName.Length() == 0) mInputFileName = GetChainOpt()->GetFileOut();
    file = gSystem->BaseName(mInputFileName);
    l = file.Index(".");
    if (l < 0) l = file.Length();
    mInputFileName = mInputFileName(mInputFileName.Index("st_"), mInputFileName.Length());
    mOutputFileName = TString(file.Data(),l);
    mOutputFileName += ".picoDst.root";
    openWrite();
    initEmc();
    break;
  case PicoIoMode::IoRead:
    openRead();
    break;
    
  default:
    LOG_ERROR << "Pico IO mode is not set ... " << endm;
    return kStErr;
  }
#ifdef __HIST_PV__
  if (GetTFile()) {
    GetTFile()->cd();
    hists[0] = new TH1F("dca3D","global track dca3D wrt best Vx",2000,0,200);
    hists[1] = new TH1F("dca3DLam","global track from Lambda",2000,0,200);
    hists[2] = new TH1F("dca3DK0s","global track from K0s",2000,0,200);
    pVrZ     = new TH2F("pVrZ","rho VS Z for primary Vertex",200,-20,20,100,0,2);
    pVxy     = new TH2F("pVxy","Y Vs X for primary Vertex",2000,-2,2,2000,-2,2);
  }
#endif /* __HIST_PV__ */
  return kStOK;
}

//_________________
int StPicoDstMaker::setVtxModeAttr()
{
  mTpcVpdVzDiffCut = DAttr("TpcVpdVzDiffCut"); //Read the Tpc-Vpd cut from the input

  if (strcasecmp(SAttr("PicoVtxMode"), "PicoVtxDefault") == 0)
  {
    setVtxMode(PicoVtxMode::Default);
    LOG_INFO << " PicoVtxDefault is being used " << endm;
    return kStOK;
  }
  else if (strcasecmp(SAttr("PicoVtxMode"), "PicoVtxVpd") == 0)
  {
    setVtxMode(PicoVtxMode::Vpd);
    LOG_INFO << " PicoVtxVpd is being used " << endm;
    return kStOK;
  }
  else if (strcasecmp(SAttr("PicoVtxMode"), "PicoVtxVpdOrDefault") == 0)
  {
    setVtxMode(PicoVtxMode::VpdOrDefault);
    LOG_INFO << " PicoVtxVpdOrDefault is being used " << endm;
    return kStOK;
  }

  return kStErr;
}

//_________________
Int_t StPicoDstMaker::InitRun(Int_t const runnumber)
{
  if (StMaker::m_Mode == PicoIoMode::IoWrite)
  {
    if (!initMtd(runnumber))
    {
      LOG_ERROR << " MTD initialization error!!! " << endm;
      return kStErr;
    }
  }
  return kStOK;
}

//_________________
Bool_t StPicoDstMaker::initMtd(Int_t const runnumber)
{
  // Oct. 1st (approx. 273rd day) is the start of a new running year
  int year = runnumber / 1e6 + 1999;
  if ((runnumber % 1000000) / 1000 >= 273) year += 1;
  LOG_INFO << "Run = " << runnumber << " year = " << year << endm;

  // obtain maps from DB
  for (Int_t i = 0; i < 30; ++i)
  {
    for (Int_t j = 0; j < 5; ++j)
    {
      mModuleToQT[i][j]    = -1;
      mModuleToQTPos[i][j] = -1;
    }
  }
  for (Int_t i = 0; i < 8; ++i)
  {
    for (Int_t j = 0; j < 8; ++j)
    {
      mQTtoModule[i][j]    = -1;
    }
  }


  LOG_INFO << "Retrieving mtdModuleToQTmap table from database ..." << endm;
  TDataSet* dataset = GetDataBase("Geometry/mtd/mtdModuleToQTmap");
  if (! dataset) {
    LOG_ERROR << "No mtdModuleToQTmap table found in database" << endm;
    return kStErr;
  }
  St_mtdModuleToQTmap* mtdModuleToQTmap = static_cast<St_mtdModuleToQTmap*>(dataset->Find("mtdModuleToQTmap"));
  if (!mtdModuleToQTmap)
  {
    LOG_ERROR << "No mtdModuleToQTmap table found in database" << endm;
    return kStErr;
  }
  mtdModuleToQTmap_st* mtdModuleToQTtable = static_cast<mtdModuleToQTmap_st*>(mtdModuleToQTmap->GetTable());

  for (Int_t i = 0; i < 30; ++i)
  {
    for (Int_t j = 0; j < 5; ++j)
    {
      Int_t index = i * 5 + j;
      Int_t qt = mtdModuleToQTtable->qtBoardId[index];
      Int_t channel = mtdModuleToQTtable->qtChannelId[index];
      mModuleToQT[i][j]    = qt;
      if (channel < 0)
      {
        mModuleToQTPos[i][j] = channel;
      }
      else
      {
        if (channel % 8 == 1) mModuleToQTPos[i][j] = 1 + channel / 8 * 2;
        else             mModuleToQTPos[i][j] = 2 + channel / 8 * 2;
      }
      if (mModuleToQT[i][j] > 0 && mModuleToQTPos[i][j] > 0)
        mQTtoModule[mModuleToQT[i][j] - 1][mModuleToQTPos[i][j] - 1] = j + 1;
    }
  }

  // online slewing correction for QT board
  for (int j = 0; j < 8; ++j)
  {
    for (int i = 0; i < 16; ++i)
    {
      for (Int_t k = 0; k < 8; ++k)
      {
        mQTSlewBinEdge[j][i][k] = -1;
        mQTSlewCorr[j][i][k]    = -1;
      }
    }
  }

  LOG_INFO << "Retrieving mtdQTSlewingCorr table from database ..." << endm;
  dataset = GetDataBase("Calibrations/mtd/mtdQTSlewingCorr");
  St_mtdQTSlewingCorr* mtdQTSlewingCorr = static_cast<St_mtdQTSlewingCorr*>(dataset->Find("mtdQTSlewingCorr"));
  if (!mtdQTSlewingCorr)
  {
    LOG_ERROR << "No mtdQTSlewingCorr table found in database" << endm;
    return kStErr;
  }
  mtdQTSlewingCorr_st* mtdQTSlewingCorrtable = static_cast<mtdQTSlewingCorr_st*>(mtdQTSlewingCorr->GetTable());
  for (int j = 0; j < 4; ++j)
  {
    for (int i = 0; i < 16; ++i)
    {
      for (Int_t k = 0; k < 8; ++k)
      {
        Int_t index = j * 16 * 8 + i * 8 + k;
        mQTSlewBinEdge[j][i][k] = (int) mtdQTSlewingCorrtable->slewingBinEdge[index];
        mQTSlewCorr[j][i][k] = (int) mtdQTSlewingCorrtable->slewingCorr[index];
      }
    }
  }
  if (year == 2016)
  {
    dataset = GetDataBase("Calibrations/mtd/mtdQTSlewingCorrPart2");
    if (dataset)
    {
      St_mtdQTSlewingCorrPart2* mtdQTSlewingCorr2 = static_cast<St_mtdQTSlewingCorrPart2*>(dataset->Find("mtdQTSlewingCorrPart2"));
      mtdQTSlewingCorrPart2_st* mtdQTSlewingCorrtable2 = static_cast<mtdQTSlewingCorrPart2_st*>(mtdQTSlewingCorr2->GetTable());
      for (int j = 0; j < 4; ++j)
      {
        for (int i = 0; i < 16; ++i)
        {
          for (Int_t k = 0; k < 8; ++k)
          {
            Int_t index = j * 16 * 8 + i * 8 + k;
            mQTSlewBinEdge[j + 4][i][k] = (int) mtdQTSlewingCorrtable2->slewingBinEdge[index];
            mQTSlewCorr[j + 4][i][k] = (int) mtdQTSlewingCorrtable2->slewingCorr[index];
          }
        }
      }
    }
  }

  return kTRUE;
}

//_________________
Int_t StPicoDstMaker::Finish()
{
  if (StMaker::m_Mode == PicoIoMode::IoRead)
  {
    closeRead();
  }
  else if (StMaker::m_Mode == PicoIoMode::IoWrite)
  {
    closeWrite();
    finishEmc();
  }
  return kStOK;
}

//_________________
Int_t StPicoDstMaker::openRead()
{
  if (!mChain) mChain = new TChain("PicoDst");

  string const dirFile = mInputFileName.Data();
  if (dirFile.find(".list") != string::npos)
  {
    ifstream inputStream(dirFile.c_str());

    if (!inputStream)
    {
      LOG_ERROR << "ERROR: Cannot open list file " << dirFile << endm;
      return kStErr;
    }

    int nFile = 0;
    string file;
    while (getline(inputStream, file))
    {
      if (file.find(".picoDst.root") != string::npos)
      {
        TFile* ftmp = TFile::Open(file.c_str());
        if (ftmp && !ftmp->IsZombie() && ftmp->GetNkeys())
        {
          LOG_INFO << " Read in picoDst file " << file << endm;
          mChain->Add(file.c_str());
          ++nFile;
        }

        if (ftmp) ftmp->Close();
      }
    }

    LOG_INFO << " Total " << nFile << " files have been read in. " << endm;
  }
  else if (dirFile.find(".picoDst.root") != string::npos)
  {
    mChain->Add(dirFile.c_str());
  }
  else
  {
    LOG_WARN << " No good input file to read ... " << endm;
  }

  if (mChain)
  {
    setBranchAddresses(mChain);
    mChain->SetCacheSize(50e6);
    mChain->AddBranchToCache("*");
    mPicoDst->set(mPicoArrays);
  }

  return kStOK;
}

//_________________
void StPicoDstMaker::openWrite()
{

  mOutputFile = new TFile(mOutputFileName.Data(), "RECREATE");
  LOG_INFO << " Output file: " << mOutputFileName.Data() << " created." << endm;
  mOutputFile->SetCompressionLevel(mCompression);
  int bufsize = mBufferSize;
  if (mSplit) bufsize /= 4;
  mTTree = new TTree("PicoDst", "StPicoDst", mSplit);
  mTTree->SetAutoSave(1000000);
  for (int i = 0; i < StPicoArrays::NAllPicoArrays; ++i)
  {
    if (mStatusArrays[i] == 0)
    {
      LOG_INFO << " Branch " << StPicoArrays::picoArrayNames[i] << " status is OFF! " << endm;
      continue;
    }

    mTTree->Branch(StPicoArrays::picoArrayNames[i], &mPicoArrays[i], bufsize, mSplit);
  }
}

//_________________
void StPicoDstMaker::initEmc()
{
  mEmcPosition = new StEmcPosition();

  for (int i = 0; i < 4; ++i)
  {
    mEmcGeom[i] = StEmcGeom::getEmcGeom(detname[i].Data());
  }
}

//_________________
void StPicoDstMaker::buildEmcIndex()
{
  StEmcDetector* mEmcDet = mMuDst->emcCollection()->detector(kBarrelEmcTowerId);
  std::fill_n(mEmcIndex, sizeof(mEmcIndex) / sizeof(mEmcIndex[0]), nullptr);

  if (!mEmcDet) return;
  for (size_t iMod = 1; iMod <= mEmcDet->numberOfModules(); ++iMod)
  {
    StSPtrVecEmcRawHit& modHits = mEmcDet->module(iMod)->hits();
    for (size_t iHit = 0; iHit < modHits.size(); ++iHit)
    {
      StEmcRawHit* rawHit = modHits[iHit];
      if (!rawHit) continue;
      UInt_t softId = rawHit->softId(1);
      if (mEmcGeom[0]->checkId(softId) == 0) // OK
      {
        mEmcIndex[softId - 1] = rawHit;
      }

    }
  }
}

//_________________
void StPicoDstMaker::finishEmc()
{
  delete mEmcPosition;
  mEmcPosition = nullptr;

  std::fill_n(mEmcGeom, 4, nullptr);
}

//_________________
void StPicoDstMaker::Clear(char const*)
{
  if (StMaker::m_Mode == PicoIoMode::IoRead)
    return;
  clearArrays();
}

//_________________
void StPicoDstMaker::closeRead()
{
  delete mChain;
  mChain = nullptr;
}

//_________________
void StPicoDstMaker::closeWrite()
{
  if (StMaker::m_Mode == PicoIoMode::IoWrite)
  {
    if (mOutputFile)
    {
      mOutputFile->Write();
      mOutputFile->Close();
    }
  }
}

//_________________
int StPicoDstMaker::Make()
{
  int returnStarCode = kStOK;

  if (StMaker::m_Mode == PicoIoMode::IoWrite)
  {
    returnStarCode = MakeWrite();
  }
  else if (StMaker::m_Mode == PicoIoMode::IoRead)
    returnStarCode = MakeRead();

  return returnStarCode;
}

//_________________
Int_t StPicoDstMaker::MakeRead()
{
  if (!mChain)
  {
    LOG_WARN << " No input files ... ! EXIT" << endm;
    return kStWarn;
  }
  if (mChain->GetEntry(mEventCounter++) <= 0) {
    return kStEOF;
  }
  fillEventHeader();

  return kStOK;
}

//_________________
Int_t StPicoDstMaker::MakeWrite()
{
  mMuDst = StMuDst::instance();

  if (!mMuDst)
  {
    LOG_ERROR << "No \"StMuDst\" object found in this event. It is usually created by StMuDstMaker" << endm;
    return kStErr;
  }

  StMuEvent* muEvent = mMuDst->event();

  if (!muEvent)
  {
    LOG_WARN << " No MuEvent " << endm;
    return kStWarn;
  }

  int const originalVertexId = mMuDst->currentVertexIndex();
  if (! mMuDst->numberOfMcVertices()) { // for MC it might be no Vpd
    if (!selectVertex())  {
      static Int_t count = 0;
      count++;
      if (count < 13) {
	LOG_INFO << "Vertex is not valid" << endm;
      }
      mMuDst->setVertexIndex(originalVertexId);
      return kStOK;
    }
  }
  mBField = muEvent->magneticField();


  mEmcCollection = mMuDst->emcCollection();

  if (mEmcCollection) 
  {
    // build EmcIndex before ::fillTracks()
    buildEmcIndex();
    // fill BTOW hits only if ::buildEmcIndex() has been called for this event
    fillBTowHits();
  }

  fillTracks();
  fillEvent();
  fillEmcTrigger();

  fillMtdTrigger();
  fillBTofHits();
  fillMtdHits();
  mBbcFiller.fill(*mMuDst);
  mEpdFiller.fill(*mMuDst);

  // Could be a good idea to move this call to Init() or InitRun()
  StFmsDbMaker* fmsDbMaker = static_cast<StFmsDbMaker*>(GetMaker("fmsDb"));

  if (fmsDbMaker)
    StMuFmsUtil::recoverMuFmsCollection(*mMuDst, fmsDbMaker);

  mFmsFiller.fill(*mMuDst);

  if (Debug()) mPicoDst->printTracks();

  mTTree->Fill();

  mMuDst->setVertexIndex(originalVertexId);

  return kStOK;
}

//_________________
void StPicoDstMaker::fillEventHeader() const
{
  StPicoEvent* event=StPicoDst::event();
  if(!event)
    return;

  StEvtHddr* header=GetEvtHddr();//get or create
  header->SetRunNumber(event->runId());
  header->SetEventNumber(event->eventId());
  header->SetGMTime( (UInt_t) (event->time()) );
}

//_____________________________________________________________________________
Int_t StPicoDstMaker::fillTracks()
{
  if (! mMuDst->primaryVertex()) return 1;
  std::unordered_map<UInt_t, UInt_t> index2Primary;

  Int_t nPrimarys = mMuDst->numberOfPrimaryTracks();
  for (int i = 0; i < nPrimarys; ++i)
  {
    StMuTrack* pTrk = (StMuTrack*)mMuDst->primaryTracks(i);
    if (!pTrk) continue;
    if (! mMuDst->Accept(pTrk)) continue;
    index2Primary[pTrk->id()] = i;
  }

  Int_t nGlobals = mMuDst->numberOfGlobalTracks();
  for (int i = 0; i < nGlobals; ++i)
  {
    StMuTrack* gTrk = (StMuTrack*)mMuDst->globalTracks(i);
    if (!gTrk) continue;
    if (! mMuDst->Accept(gTrk)) continue;

    StMuTrack const* const pTrk = index2Primary.find(gTrk->id()) != index2Primary.end() ?
                                  (StMuTrack*)mMuDst->primaryTracks(index2Primary[gTrk->id()]) : nullptr;

    if (gTrk->index2Cov() < 0) continue;

    StDcaGeometry* dcaG = mMuDst->covGlobTracks(gTrk->index2Cov());
    if (!dcaG)
    {
      LOG_WARN << "No dca Geometry for this track !!! " << i << endm;
      continue;
    }
    // Cut large Dca
    THelixTrack t = dcaG->thelix();
    StThreeVectorD V(mMuDst->primaryVertex()->position());
    Double_t dca3D = t.Dca(V.xyz());
#ifdef __HIST_PV__
    if (hists[0]) {
      hists[0]->Fill(dca3D);
      Int_t IdMc = gTrk->idTruth();
      if (IdMc) {
	StMuMcTrack *mcTrk = StMuDst::instance()->MCtrack(IdMc-1);
	assert(mcTrk);
	assert(IdMc == mcTrk->Id());
	Int_t IdVx = mcTrk->IdVx(); // parent vertex
	assert(IdVx);
	StMuMcVertex *MuVx = StMuDst::instance()->MCvertex(IdVx-1);
	assert(MuVx);
	Int_t IdParentTk = MuVx->IdParTrk();
	if (IdParentTk) {
	  StMuMcTrack *mcParentTrk = StMuDst::instance()->MCtrack(IdParentTk-1);
	  assert(mcParentTrk);
#if 1
	  Int_t pdg = mcParentTrk->Pdg();
	  assert(pdg);
	  if (TMath::Abs(pdg) ==  3122) {
	    hists[1]->Fill(dca3D);
	  } else if (pdg == 310) {
	    hists[2]->Fill(dca3D);
	  }
#else
	  Int_t gePid = mcParentTrk->GePid();
	  if (gePid == 18 || gePid == 26) {
	    hists[1]->Fill(dca3D);
	  } else if (gePid == 16) {
	    hists[2]->Fill(dca3D);
	  }
#endif
	}
      }
    }
#endif /* __HIST_PV__ */
    if (fgdca3Dmax > 0 && dca3D > fgdca3Dmax) continue;
    int counter = mPicoArrays[StPicoArrays::Track]->GetEntries();
    new((*(mPicoArrays[StPicoArrays::Track]))[counter]) StPicoTrack(gTrk, pTrk, mBField, mMuDst->primaryVertex()->position(), *dcaG);

    StPicoTrack* picoTrk = (StPicoTrack*)mPicoArrays[StPicoArrays::Track]->At(counter);
    if (_debug) {
      cout << "StPicoDstMaker::fillTracks: MuTrack " << Form("%4i %8.3f %8.3f %8.3f",i,gTrk->p().x() ,gTrk->p().y() ,gTrk->p().z())
	   << Form("\te/pi/K/p\t%8.3f %8.3f %8.3f %8.3f",gTrk->nSigmaElectron(),gTrk->nSigmaPion(), gTrk->nSigmaKaon(),gTrk->nSigmaProton()) << endl
	   << "                          PicoTrack " << Form("%4i %8.3f %8.3f %8.3f",i,picoTrk->gMom().x() ,picoTrk->gMom().y() ,picoTrk->gMom().z())
	   << Form("\te/pi/K/p\t%8.3f %8.3f %8.3f %8.3f",picoTrk->nSigmaElectron(),picoTrk->nSigmaPion(), picoTrk->nSigmaKaon(),picoTrk->nSigmaProton()) << endl;
    }
    // Fill pid traits
    if (mEmcCollection)
    {
      int id = -1;
      int adc0;
      float e[5];
      float dist[4];
      int nhit[2];
      int ntow[3];

      getBEMC(gTrk, &id, &adc0, e, dist, nhit, ntow);

      if (id >= 0)
      {
        Int_t bemc_index = mPicoArrays[StPicoArrays::BEmcPidTraits]->GetEntries();
        new((*(mPicoArrays[StPicoArrays::BEmcPidTraits]))[bemc_index]) StPicoBEmcPidTraits(counter, id, adc0, e, dist, nhit, ntow);
        picoTrk->setBEmcPidTraitsIndex(bemc_index);
      }
    }

    if (gTrk->tofHit())
    {
      Int_t btof_index = mPicoArrays[StPicoArrays::BTofPidTraits]->GetEntries();
      new((*(mPicoArrays[StPicoArrays::BTofPidTraits]))[btof_index]) StPicoBTofPidTraits(gTrk, pTrk, counter);
      picoTrk->setBTofPidTraitsIndex(btof_index);
    }

    if (gTrk->mtdHit())
    {
      Int_t mtd_index = mPicoArrays[StPicoArrays::MtdPidTraits]->GetEntries();
      new((*(mPicoArrays[StPicoArrays::MtdPidTraits]))[mtd_index]) StPicoMtdPidTraits(gTrk->mtdHit(), &(gTrk->mtdPidTraits()), counter);
      picoTrk->setMtdPidTraitsIndex(mtd_index);
    }
  }
  return 0;
}

//_____________________________________________________________________________
Bool_t StPicoDstMaker::getBEMC(const StMuTrack* t, int* id, int* adc, float* ene, float* d, int* nep, int* towid)
{
  *id = -1;
  *adc = 0;

  std::fill(ene, ene+5, 0.);
  std::fill(d, d+4, 1.e9);
  std::fill(nep, nep+2, 0);
  std::fill(towid, towid+3, -1);

  if (!mEmcCollection)
  {
    LOG_WARN << " No Emc Collection for this event " << endm;
    return kFALSE;
  }

  StThreeVectorD position, momentum;
  StThreeVectorD positionBSMDE, momentumBSMDE;
  StThreeVectorD positionBSMDP, momentumBSMDP;

  double magneticField = mBField * kilogauss / tesla; // in Tesla

  Bool_t ok       = false;
  Bool_t okBSMDE  = false;
  Bool_t okBSMDP  = false;

  if (mEmcPosition)
  {
    ok      = mEmcPosition->projTrack(&position,      &momentum,      t, magneticField, mEmcGeom[0]->Radius());
    okBSMDE = mEmcPosition->projTrack(&positionBSMDE, &momentumBSMDE, t, magneticField, mEmcGeom[2]->Radius());
    okBSMDP = mEmcPosition->projTrack(&positionBSMDP, &momentumBSMDP, t, magneticField, mEmcGeom[3]->Radius());
  }

  if (!ok)
  {
    LOG_WARN << " Projection failed for this track ... " << endm;
    return kFALSE;
  }

  if (ok && okBSMDE && okBSMDP)
  {

    Int_t mod = 0, eta = 0, sub = 0;
    StSPtrVecEmcPoint& bEmcPoints = mEmcCollection->barrelPoints();
    int index = 0;
    float mindist = 1.e9;
    mEmcGeom[0]->getBin(positionBSMDP.phi(), positionBSMDE.pseudoRapidity(), mod, eta, sub); //project on SMD plan
    // Loop over all BEMC measurements, aka "points"
    for (StSPtrVecEmcPointIterator it = bEmcPoints.begin(); it != bEmcPoints.end(); ++it, ++index)
    {
      Bool_t associated = false;
      // Consider only BEMC clusters
      StPtrVecEmcCluster& bEmcClusters = (*it)->cluster(kBarrelEmcTowerId);
      if (bEmcClusters.size() == 0) continue;
      if (bEmcClusters[0] == NULL) continue;
      // Loop over all BEMC clusters
      for (StPtrVecEmcClusterIterator cIter = bEmcClusters.begin(); cIter != bEmcClusters.end(); ++cIter)
      {
        StPtrVecEmcRawHit& bEmcHits = (*cIter)->hit();
        // Loop over all hits/towers in the BEMC cluster
        for (StPtrVecEmcRawHitIterator hIter = bEmcHits.begin(); hIter != bEmcHits.end(); ++hIter)
        {
          // Find BEMC hit matching the track projection to BEMC
          if (mod == (Int_t)(*hIter)->module() && eta == (Int_t)(*hIter)->eta() && sub == (Int_t)(*hIter)->sub())
          {
            associated = true;
            break;
          }
        }
        if (associated)
        {
          // Loop over all hits/towers in the BEMC cluster again
          for (StPtrVecEmcRawHitIterator hitit = bEmcHits.begin(); hitit != bEmcHits.end(); ++hitit)
          {
            // Save the highest energy among the towers in the BEMC cluster to ene[0]
            if ((*hitit)->energy() > ene[0]) ene[0] = (*hitit)->energy();
            // Save the highest ADC among the towers in the BEMC cluster to adc
            if ((int)(*hitit)->adc() > (*adc)) *adc = (*hitit)->adc();
          }
        }
      }

      StPtrVecEmcCluster& smdeClusters = (*it)->cluster(kBarrelSmdEtaStripId);
      StPtrVecEmcCluster& smdpClusters = (*it)->cluster(kBarrelSmdPhiStripId);

      if (associated)
      {
        *id = index;
        ene[1] = ene[1] + (*it)->energy(); //use point's energy, not tower cluster's energy

        float deltaphi = (*it)->position().phi() - positionBSMDP.phi();
        if (deltaphi >= TMath::Pi()) deltaphi = deltaphi - TMath::TwoPi();
        if (deltaphi < -TMath::Pi()) deltaphi = deltaphi + TMath::TwoPi();

        float rsmdp = mEmcGeom[3]->Radius();
        float pointz = (*it)->position().z();
        float deltaz = pointz - positionBSMDE.z();
        if (sqrt(deltaphi * deltaphi * rsmdp * rsmdp + deltaz * deltaz) < mindist)
        {
          d[1] = deltaphi;
          d[0] = deltaz;
          if (smdeClusters.size() >= 1) nep[0] = smdeClusters[0]->nHits();
          if (smdpClusters.size() >= 1) nep[1] = smdpClusters[0]->nHits();
          mindist = sqrt(deltaphi * deltaphi * rsmdp * rsmdp + deltaz * deltaz);
        }
      }//associated
    }

  } // end if (ok && okBSMDE && okBSMDP)

  //Get BEMC tower energy from matched tower + 2 nearest towers

  int towerId = 0;
  int localTowerId = -1;
  int localId1 = -1;
  int localId2 = -1;
  double energy1 = 0, energy2 = 0;
  double energyTemp = 0;
  double dist1 = 1000, dist2 = 1000;
  double distTemp = 0;
  Float_t etaTemp = 0, phiTemp = 0;

  if (mEmcGeom[0]->getId(position.phi(), position.pseudoRapidity(), towerId) == 1) return kTRUE;
  for (int ieta = -1; ieta < 2; ++ieta)
  {
    for (int iphi = -1; iphi < 2; ++iphi)
    {
      localTowerId++;//loops from 0 to 8
      int nextTowerId = mEmcPosition->getNextTowerId(towerId, ieta, iphi);
      if (nextTowerId < 1 || nextTowerId > 4800) continue;
      StEmcRawHit* emcHit = mEmcIndex[nextTowerId - 1];
      if (emcHit == 0) continue;
      if (emcHit->energy() < 0.2) continue; // don't include any noise tower
      if (ieta == 0 && iphi == 0)
      {
        mEmcGeom[0]->getEta(nextTowerId, etaTemp);
        mEmcGeom[0]->getPhi(nextTowerId, phiTemp);
        ene[2] = emcHit->energy();
        d[2] = position.pseudoRapidity() - etaTemp;
        d[3] = position.phi() - phiTemp;
      }
      else
      {
        energyTemp = emcHit->energy();
        mEmcGeom[0]->getEta(nextTowerId, etaTemp);
        mEmcGeom[0]->getPhi(nextTowerId, phiTemp);
        distTemp = sqrt((etaTemp - position.pseudoRapidity()) * (etaTemp - position.pseudoRapidity()) + (phiTemp - position.phi()) * (phiTemp - position.phi()));
        if (distTemp < dist1)
        {
          dist2 = dist1;
          dist1 = distTemp;
          energy2 = energy1;
          energy1 = energyTemp;
          localId1 = localTowerId;
        }
        else if (distTemp < dist2)
        {
          dist2 = distTemp;
          energy2 = energyTemp;
          localId2 = localTowerId;
        }
      }
    }
  }
  towid[0] = towerId;
  ene[3] = energy1;//closest tower
  towid[1] = localId1;
  ene[4] = energy2;//2nd closest tower
  towid[2] = localId2;

  LOG_DEBUG << " ====== BEMC results ====== " << "\n"
            << " Energy = " << ene[0] << " " << ene[1] << " " << ene[2] << " " << ene[3] << " " << ene[4] << "\n"
            << " BSMD = " << nep[0] << " " << nep[1] << "\n"
            << " TowerId = " << towid[0] << " " << towid[1] << " " << towid[2] << endm;

  return kTRUE;
}

//_________________
void StPicoDstMaker::fillEvent()
{
  int counter = mPicoArrays[StPicoArrays::Event]->GetEntries();
  new((*(mPicoArrays[StPicoArrays::Event]))[counter]) StPicoEvent(*mMuDst);
}

//_________________
void StPicoDstMaker::fillEmcTrigger()
{

  // test for EMC trigger
  StTriggerSimuMaker* trigSimu = (StTriggerSimuMaker*)GetMaker("StarTrigSimu");
  if (!trigSimu) return;

  // BEMC High Tower trigger
  if (!trigSimu->bemc) return;
  int bht0 = trigSimu->bemc->barrelHighTowerTh(0);
  int bht1 = trigSimu->bemc->barrelHighTowerTh(1);
  int bht2 = trigSimu->bemc->barrelHighTowerTh(2);
  int bht3 = trigSimu->bemc->barrelHighTowerTh(3);
  LOG_DEBUG << " bht thresholds " << bht0 << " " << bht1 << " " << bht2 << " " << bht3 << endm;
  for (int i = 0; i < 4; ++i) mPicoDst->event()->setHighTowerThreshold(i, trigSimu->bemc->barrelHighTowerTh(i));

  for (int towerId = 1; towerId <= 4800; ++towerId)
  {
    int status;
    trigSimu->bemc->getTables()->getStatus(BTOW, towerId, status);
    int adc = trigSimu->bemc->barrelHighTowerAdc(towerId);
    unsigned char flag = 0;

    if (adc > bht1)
    {
      LOG_DEBUG << " id = " << towerId << " adc = " << adc << endm;
      flag |= 1 << 1;
    }

    if (adc > bht2)
    {
      LOG_DEBUG << " id = " << towerId << " adc = " << adc << endm;
      flag |= 1 << 2;
    }

    if (adc > bht3)
    {
      LOG_DEBUG << " id = " << towerId << " adc = " << adc << endm;
      flag |= 1 << 3;
    }

    if (flag & 0xf)
    {
      int counter = mPicoArrays[StPicoArrays::EmcTrigger]->GetEntries();
      new((*(mPicoArrays[StPicoArrays::EmcTrigger]))[counter]) StPicoEmcTrigger(flag, towerId, adc);
    }
  }


  // BEMC Jet Patch trigger threshold
  int const bjpth0 = trigSimu->bemc->barrelJetPatchTh(0);
  int const bjpth1 = trigSimu->bemc->barrelJetPatchTh(1);
  int const bjpth2 = trigSimu->bemc->barrelJetPatchTh(2);

  for (int i = 0; i < 3; ++i)
    mPicoDst->event()->setJetPatchThreshold(i, trigSimu->bemc->barrelJetPatchTh(i));

  for (int jp = 0; jp < 18; ++jp)
  {
    // BEMC: 12 Jet Patch + 6 overlap Jet Patches. As no EEMC information is recorded in Pico tree, not EEMC trigger information also.
    int const jpAdc = trigSimu->bemc->barrelJetPatchAdc(jp);

    unsigned char flag = 0;
    if (jpAdc > bjpth0)
    {
      flag |= 1 << 4;
    }

    if (jpAdc > bjpth1)
    {
      flag |= 1 << 5;
    }

    if (jpAdc > bjpth2)
    {
      flag |= 1 << 6;
    }

    if (flag & 0x70)
    {
      int counter = mPicoArrays[StPicoArrays::EmcTrigger]->GetEntries();
      new((*(mPicoArrays[StPicoArrays::EmcTrigger]))[counter]) StPicoEmcTrigger(flag, jp, jpAdc);
    }
  }
}

//_________________
void StPicoDstMaker::fillMtdTrigger()
{
  int counter = mPicoArrays[StPicoArrays::MtdTrigger]->GetEntries();
  new((*(mPicoArrays[StPicoArrays::MtdTrigger]))[counter]) StPicoMtdTrigger(*mMuDst, mQTtoModule, mQTSlewBinEdge, mQTSlewCorr);
}

//_________________
void StPicoDstMaker::fillBTowHits()
{
  for (int i = 0; i < 4800; ++i)
  {
    StEmcRawHit* aHit = mEmcIndex[i];
    if (!aHit) continue;
    if (aHit->energy() < 0.2) continue; // remove noise towers

    int   softId = aHit->softId(1);
    int   adc    = aHit->adc();
    float energy = aHit->energy();

    int counter = mPicoArrays[StPicoArrays::BTowHit]->GetEntries();
    new((*(mPicoArrays[StPicoArrays::BTowHit]))[counter]) StPicoBTowHit(softId, adc, energy);
  }
}

//_________________
void StPicoDstMaker::fillBTofHits()
{
  for (UInt_t i = 0; i < mMuDst->numberOfBTofHit(); ++i)
  {
    StMuBTofHit* aHit = (StMuBTofHit*)mMuDst->btofHit(i);
    if (aHit->tray() > 120) continue;
    int cellId = (aHit->tray() - 1) * 192 + (aHit->module() - 1) * 6 + (aHit->cell() - 1);

    int counter = mPicoArrays[StPicoArrays::BTofHit]->GetEntries();
    new((*(mPicoArrays[StPicoArrays::BTofHit]))[counter]) StPicoBTofHit(cellId);
  }
}

//_________________
void StPicoDstMaker::fillMtdHits()
{
  // fill MTD hits
  Int_t nMtdHits = mMuDst->numberOfMTDHit();
  for (Int_t i = 0; i < nMtdHits; ++i)
  {
    StMuMtdHit* hit = (StMuMtdHit*)mMuDst->mtdHit(i);
    if (!hit) continue;
    Int_t counter = mPicoArrays[StPicoArrays::MtdHit]->GetEntries();
    new((*(mPicoArrays[StPicoArrays::MtdHit]))[counter]) StPicoMtdHit(hit);
  }
  UInt_t nHits = mPicoDst->numberOfMtdHits();

  // associated MTD hits with PidTraits
  UInt_t nMtdPidTraits = mPicoDst->numberOfMtdPidTraits();
  for (UInt_t i = 0; i < nMtdPidTraits; ++i)
  {
    StPicoMtdPidTraits* pidTrait = mPicoDst->mtdPidTraits(i);
    for (UInt_t j = 0; j < nHits; ++j)
    {
      StPicoMtdHit* hit = mPicoDst->mtdHit(j);
      if (pidTrait->gChannel() == hit->gChannel())
      {
        pidTrait->setMtdHitIndex(j);
        break;
      }
    }
  }


  // check the firing hits
  if (mPicoDst->numberOfMtdTriggers() != 1)
  {
    static Int_t count = 0;
    count++;
    if (count < 13) {
      LOG_ERROR << "There are " << mPicoDst->numberOfMtdTriggers() << " MTD trigger. Check it!" << endm;
    }
    return;
  }

  StPicoMtdTrigger* trigger = mPicoDst->mtdTrigger(0);
  Int_t triggerQT[8][2];
  Bool_t triggerBit[8][8];
  Int_t pos1 = 0, pos2 = 0;
  for (Int_t i = 0; i < 8; ++i)
  {
    for (Int_t j = 0; j < 2; ++j)
      triggerQT[i][j] = 0;
    for (Int_t j = 0; j < 8; ++j)
      triggerBit[i][j] = kFALSE;

    trigger->getMaximumQTtac(i + 1, pos1, pos2);
    triggerQT[i][0] = pos1;
    triggerQT[i][1] = pos2;
    for (Int_t j = 0; j < 2; ++j)
    {
      if (triggerQT[i][j] > 0 && ((trigger->getTF201TriggerBit() >> (i * 2 + j)) & 0x1))
      {
        triggerBit[i][triggerQT[i][j] - 1] = kTRUE;
      }
    }
  }

  vector<Int_t> triggerPos;
  vector<Int_t> hitIndex;

  for (UInt_t i = 0; i < nHits; ++i)
  {
    StPicoMtdHit* hit = mPicoDst->mtdHit(i);
    Int_t backleg = hit->backleg();
    Int_t module  = hit->module();
    Int_t qt = mModuleToQT[backleg - 1][module - 1];
    Int_t pos = mModuleToQTPos[backleg - 1][module - 1];
    if (qt >= 1 && qt <= 8 && pos > 0 && triggerBit[qt - 1][pos - 1])
    {
      triggerPos.push_back(qt * 10 + pos);
      hitIndex.push_back(i);
    }
    else
    {
      hit->setTriggerFlag(0);
    }
  }

  vector<Int_t> hits;

  while (triggerPos.size() > 0)
  {
    hits.clear();
    hits.push_back(0);
    for (Int_t j = 1; j < (Int_t)triggerPos.size(); ++j)
    {
      if (triggerPos[j] == triggerPos[0])
        hits.push_back(j);
    }

    for (Int_t k = (Int_t)hits.size() - 1; k > -1; k--)
    {
      StPicoMtdHit* hit = mPicoDst->mtdHit( hitIndex[hits[k]] );
      hit->setTriggerFlag( (Int_t)hits.size() );
      triggerPos.erase(triggerPos.begin() + hits[k]);
      hitIndex.erase(hitIndex.begin() + hits[k]);
    }
  }
}


/**
 * Selects a primary vertex from `muDst` vertex collection according to the
 * vertex selection mode `mVtxMode` specified by the user. The mode must be
 * set with StMaker::SetAttr("PicoVtxMode", "your_desired_vtx_mode") as by
 * default the selection mode is `PicoVtxMode::NotSet`.
 *
 * Returns `true` if the user has specified a valid vertex selection mode and
 * a valid vertex satisfying the corresponding predefined conditions is found in
 * the muDst vertex collection.
 *
 * Returns `false` otherwise.
 */
Bool_t StPicoDstMaker::selectVertex() {
  if (! mMuDst->numberOfPrimaryVertices()) return kFALSE;
  StMuPrimaryVertex* selectedVertex = nullptr;
  UInt_t Nt = fGoodTriggerIds.size();
  if (Nt) {
    const StTriggerId& triggers = StMuDst::instance()->event()->triggerIdCollection().l1();
    Int_t GoodTrigger = -1;
    Int_t NoAnyTriggers = 0;
    for (Int_t k = 0; k < 64; k++) {
      Int_t trig = triggers.triggerId(k);
      if (! trig) continue;
      NoAnyTriggers++;
      for (UInt_t l = 0; l < Nt; l++) {
	if (trig == fGoodTriggerIds[l]) {
	  GoodTrigger = trig;
	  break;
	}
	if (GoodTrigger > 0) break;
      }
      if (NoAnyTriggers && GoodTrigger < 0) {return selectedVertex;}
    }
  }
  for (UInt_t iVtx = 0; iVtx < mMuDst->numberOfPrimaryVertices(); ++iVtx)       {
    StMuPrimaryVertex* vtx = mMuDst->primaryVertex(iVtx);
    if (!vtx) continue;
    StThreeVectorD V(vtx->position());
#ifdef __HIST_PV__
    if (pVrZ) {
      pVrZ->Fill(V.z(),V.perp());
      pVxy->Fill(V.y(),V.x());
    }
#endif /* __HIST_PV__ */
    if (! mMuDst->numberOfMcVertices()) { // No cutss for MC event
      /* Cuts:
	 1.  -0.3 < X < 0.1 и -0.27 < Y < -0.13. Maksym
	 2.  sqrt(sigma_X**2 + sigma_Y**2) < 0.0050 cm
	 3.  const Char_t *triggersC = "520001, 520011, 520021, 520031, 520041, 520051"
	 4.  dca3D < 50 cm 
      */
      if (fgVxXmin < fgVxXmax && ! (fgVxXmin < V.x() && V.x() < fgVxXmax)) {continue;}
      if (fgVxYmin < fgVxYmax && ! (fgVxYmin < V.y() && V.y() < fgVxYmax)) {continue;}
      if (fgVxZmin < fgVxZmax && ! (fgVxZmin < V.z() && V.z() < fgVxZmax)) {continue;}
      if (fgVxRmax > 0 &&  V.perp() > fgVxRmax)                            {continue;}
      StThreeVectorD E(mMuDst->primaryVertex()->posError());
      const Double_t er = E.perp();
      if (fgerMax > 0 && er > fgerMax) {continue;}
    }
    // We save primary tracks associated with the selected primary vertex only
    // don't use StMuTrack::primary(), it returns primary tracks associated with
    // all vertices
    if (mVtxMode == PicoVtxMode::Default)  {
      // choose the default vertex, i.e. the first vertex
      mMuDst->setVertexIndex(iVtx);
      selectedVertex = vtx;
    } else if (mVtxMode == PicoVtxMode::Vpd || mVtxMode == PicoVtxMode::VpdOrDefault)  {
      StBTofHeader const* mBTofHeader = mMuDst->btofHeader();
      Float_t vzVpd = -999;
      if (mBTofHeader && TMath::Abs(mBTofHeader->vpdVz()) < 200) vzVpd = mBTofHeader->vpdVz();
      if (mVtxMode == PicoVtxMode::Vpd && vzVpd < 200) continue;
      if (vzVpd >= -200 && mTpcVpdVzDiffCut > 0 && TMath::Abs(vzVpd - vtx->position().z()) >= mTpcVpdVzDiffCut) continue;
      mMuDst->setVertexIndex(iVtx);
      selectedVertex = mMuDst->primaryVertex();
      break;
    } else {// default case
      LOG_ERROR << "Pico Vtx Mode not set!" << endm;
    }
  }
  // Retrun false if selected vertex is not valid
  return selectedVertex ? true : false;
}
//________________________________________________________________________________
void  StPicoDstMaker::SetGoodTriggers(const Char_t *trigList) {

  fGoodTriggerIds.clear();
  TString Trig(trigList);
  if (Trig == "") return;
  TObjArray *obj = Trig.Tokenize("[^ ;,:]");
  Int_t nParsed = obj->GetEntries();
  for (Int_t k = 0; k < nParsed; k++) {
    if (obj->At(k)) {
      LOG_INFO << "Trigger: " << k << "\t" << ((TObjString *) obj->At(k))->GetName() << endm;
      TString t(((TObjString *) obj->At(k))->GetName());
      Int_t trig = t.Atoi();
      if (! trig) continue;
      fGoodTriggerIds.push_back(trig);
    }
  }
  obj->SetOwner(kFALSE);
  delete obj;
}
//________________________________________________________________________________
void StPicoDstMaker::SetMaxTrackDca(Double_t cut) {
  fgdca3Dmax = cut;
  LOG_INFO << "StPicoDstMaker::SetMaxTrackDca = " << fgdca3Dmax << endm;
}
//________________________________________________________________________________
void StPicoDstMaker::SetMaxVertexTransError(Double_t cut) {
  fgerMax = cut;
  LOG_INFO << "StPicoDstMaker::SetMaxVertexTransError = " << fgerMax << endm;
}
//________________________________________________________________________________
void StPicoDstMaker::SetVxXYrange(Double_t xmin, Double_t xmax, Double_t ymin, Double_t ymax) {
  fgVxXmin = xmin;
  fgVxXmax = xmax;
  fgVxYmin = ymin;
  fgVxYmax = ymax;
  LOG_INFO << "StPicoDstMaker::SetVxXYrange for PV: x in [" 
	   << fgVxXmin << "," << fgVxXmax <<"], y in [" 
	   << fgVxYmin << "," << fgVxYmax << "]" << endm;
}
//________________________________________________________________________________
void StPicoDstMaker::SetVxZrange(Double_t zmin, Double_t zmax) {
  fgVxZmin = zmin;
  fgVxZmax = zmax;
  LOG_INFO << "StPicoDstMaker::SetVxZrange for PV: z in [" 
	   << fgVxZmin << "," << fgVxZmax <<"]" << endm;
}
//________________________________________________________________________________
void StPicoDstMaker::SetVxRmax(Double_t rmax) {
  fgVxRmax = rmax;
  LOG_INFO << "StPicoDstMaker::SetVxRmax for PV: rho < " << fgVxRmax << endm;
}
//________________________________________________________________________________
