#include "TGFrame.h"
#include "StThreeVector.hh"
#include "StiGui/EventDisplay.h"
#include "StiGui/EventDisplayParameters.h"
#include "StiMaker/TileFrame.h"
#include "TRootEmbeddedCanvas.h"
#include "StiGui/StiRootDrawableTrack.h"
#include "StiGui/FileMenuGroup.h"
#include "StiGui/OptionMenuGroup.h"
///#include "StiGui/ViewMenuGroup.h"
#include "StiGui/NavigationMenuGroup.h"
#include "StiGui/TrackingMenuGroup.h"
#include "StiGui/PrintMenuGroup.h"
#include "StiGui/HelpMenuGroup.h"
#include "StiGui/StiRootDrawableDetector.h"

#include "Sti/StiTrack.h"
#include "Sti/StiMcTrack.h"
#include "Sti/StiDetector.h" 
#include "Sti/StiHit.h" 
#include "Sti/StiDefaultTrackFilter.h" 
#include "Sti/StiDetectorContainer.h"
#include "Sti/StiHitContainer.h"
#include "Sti/StiTrackContainer.h"
#include "Sti/StiMasterDetectorBuilder.h"

EventDisplay::EventDisplay(const string& name, const string & description, StiToolkit * toolkit, const TGWindow *p, UInt_t w, UInt_t h)
  : Named(name),
    Described(description), 
    TGMainFrame(p, w, h),
    _initialized(false),
    _toolkit(toolkit),
    _client(0),
    _canvas(0),
    _node(0),
    _fileMenuGroup(0),
    _optionMenuGroup(0),
    _viewMenuGroup(0),
    _navigationMenuGroup(0),
    _trackingMenuGroup(0),
    _printMenuGroup(0),
    _helpMenuGroup(0),
    _chain(0), 
    _ioMaker(0),
    _detectorContainer(0),
    _hitContainer(0),    
    _mcHitContainer(0),  
    _trackContainer(0),  
    _mcTrackContainer(0),
    _hitFilter(0),
    _trackFilter(0),
    _mcTrackFilter(0),
    _hitDrawingPolicy(0),
    _trackDrawingPolicy(0),
    _mcTrackDrawingPolicy(0),
    _defaultHitDrawingPolicy(0),
    _defaultTrackDrawingPolicy(0),
    _defaultMcTrackDrawingPolicy(0),
    _messenger(*Messenger::instance(MessageType::kTrackMessage))  //needs a fix...
{
  cout << "EventDisplay::EventDisplay( ) -I- Started"<<endl;
  _detectorContainer = toolkit->getDetectorContainer();
  _hitContainer      = toolkit->getHitContainer();
  _mcHitContainer    = toolkit->getMcHitContainer();
  _trackContainer    = toolkit->getTrackContainer();
  _mcTrackContainer  = toolkit->getMcTrackContainer();
  _options = new EventDisplayParameters();
  Observer * obs = dynamic_cast<Observer *>(this);
  dynamic_cast<Subject*>(_options)->attach(obs);
  createFilters();
  createPolicies();
  createMenu();
  createCanvasFrame();  
  AddFrame(_trackingMenuGroup->getCompositeFrame(), new TGLayoutHints(kLHintsBottom | kLHintsExpandX, 0, 0, 1, 0));
  SetWindowName(description.c_str());
  MapSubwindows();
  Resize(GetDefaultSize());
  MapWindow();
  cout << "EventDisplay::EventDisplay( ) -I- Done"<<endl;
}

EventDisplay::~EventDisplay()
{}


void EventDisplay::createMenu()
{
  //cout << " EventDisplay::createMenu() -I- Started"<<endl;
  TGLayoutHints * menuItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft,0, 4, 0, 0);
  TGLayoutHints * menuBarLayout  = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,0, 0, 1, 1);
  TGLayoutHints * helpItemLayout = new TGLayoutHints(kLHintsTop | kLHintsRight);
  _fileMenuGroup = new FileMenuGroup("File","File",this,0);
  _optionMenuGroup= new OptionMenuGroup("Option","Option",this,100);
  //_viewMenuGroup= new ViewMenuGroup("View","View",this,200);
  _navigationMenuGroup= new NavigationMenuGroup("Navigation","Navigation",this,300);
  _trackingMenuGroup= new TrackingMenuGroup("Tracking","Tracking",this,400);
  _printMenuGroup= new PrintMenuGroup("Print","Print",this,500);
  _helpMenuGroup= new HelpMenuGroup("Help","Help",this,600);
  TGMenuBar * menuBar = new TGMenuBar(this, 1, 1, kHorizontalFrame);
  _fileMenuGroup->create(menuBar,menuItemLayout); 
  _optionMenuGroup->create(menuBar,menuItemLayout);
  //_viewMenuGroup->create(menuBar,menuItemLayout);  
  _navigationMenuGroup->create(menuBar,menuItemLayout);
  _trackingMenuGroup->create(menuBar,menuItemLayout);
  _printMenuGroup->create(menuBar,menuItemLayout);
  _helpMenuGroup->create(menuBar,helpItemLayout);
  AddFrame(menuBar, menuBarLayout);
  //cout << " EventDisplay::createMenu() -I- Started"<<endl;
}

void EventDisplay::createCanvasFrame()
{
  //cout << "EventDisplay::createCanvasFrame() -I- Started"<<endl;
  TRootEmbeddedCanvas * embeddedCanvas = new TRootEmbeddedCanvas("Embedded Canvas", this, 650, 600);
  TileFrame * tileFrame = new TileFrame(embeddedCanvas->GetViewPort());
  tileFrame->SetCanvas(embeddedCanvas);
  embeddedCanvas->SetContainer(tileFrame);
  _canvas = embeddedCanvas->GetCanvas();
  _canvas->cd();
  _node = new TVolume();
  _node->SetName("mainnode");
  _node->SetTitle("mainnode");
  AddFrame(embeddedCanvas, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 0, 0, 2, 2));
  //cout << "EventDisplay::createCanvasFrame() -I- Done"<<endl;
}



/// Handle messages send to the EventDisplay object. E.g. all menu button
/// messages.
Bool_t EventDisplay::ProcessMessage(Long_t msg, Long_t option1, Long_t option2)
{
  //cout << "EventDisplay::ProcessMessage() -I- msg:"<<msg<<" Option1:"<<option1<<" option2:"<<option2<<endl;
  switch (GET_MSG(msg)) 
    {
    case kC_COMMAND:
      switch (GET_SUBMSG(msg)) 
	{
	case kCM_BUTTON:
	  _trackingMenuGroup->dispatch(option1);
	  _navigationMenuGroup->dispatch(option1);
	  break;
	case kCM_MENUSELECT:
	  break;
	case kCM_MENU:
	  _fileMenuGroup->dispatch(option1);
	  _optionMenuGroup->dispatch(option1);
	  //_viewMenuGroup->dispatch(option1);
	  _navigationMenuGroup->dispatch(option1);
	  _trackingMenuGroup->dispatch(option1);
	  _printMenuGroup->dispatch(option1);
	  _helpMenuGroup->dispatch(option1);
	  break;
	}
    }
  return true;
}

void EventDisplay::update()
{
  draw();
}

void EventDisplay::draw()
{ 
  //cout << "EventDisplay::draw() -I- Started"<<endl;
  reset();
  _canvas->cd();
  _canvas->Update();
  if (!_initialized)
    {
      cout << "EventDisplay::draw() -I- Initialize detector display"<<endl;
      draw(_detectorContainer);
      _initialized = true;
    }

  if (_options->getDetectorVisible())  _node->Draw();
  if (_options->getHitVisible())       draw(_hitContainer,    _hitFilter,    _hitDrawingPolicy);
  //if (_options->getMcHitVisible())     draw(_mcHitContainer,  _mcHitFilter,  _mcHitDrawingPolicy);
  if (_options->getMcTrackVisible())   draw(_mcTrackContainer,_mcTrackFilter,_mcTrackDrawingPolicy);
  if (_options->getTrackVisible())     draw(_trackContainer,  _trackFilter,  _trackDrawingPolicy); 
  _canvas->Update();
  //cout << "EventDisplay::draw() -I- Started"<<endl;
}

void EventDisplay::draw(StiDetectorContainer * detectorContainer)
{
  StiDetector * detector; 
  StiRootDrawableDetector* rootDrawableDetector;
  StiMasterDetectorBuilder * master = getToolkit()->getDetectorBuilder();
  vector<StiDetectorBuilder*>::iterator bIter;
  for (bIter=master->begin();bIter!=master->end();bIter++)
    {
      //cout << "Detector:"<< (*bIter)->getName()<<endl;
      int nRows = (*bIter)->getNRows();
      for (int row=0;row<nRows;row++)
	{
	  int nSectors = (*bIter)->getNSectors(row);
	  for (int sector=0;sector<nSectors;sector++)
	    {
	      detector = (*bIter)->getDetector(row,sector);
	      if (detector)
		{
		  rootDrawableDetector = static_cast<StiRootDrawableDetector*>(detector);
		  if (rootDrawableDetector)
		    {
		      if (row==0 || row==(nRows-1) )
			rootDrawableDetector->setVisible(true);
		      else
			rootDrawableDetector->setVisible(false);
		      const StThreeVector<double>& pos = rootDrawableDetector->position();
		      _node->Add(rootDrawableDetector->volume(), 
				 pos.x(),
				 pos.y(),
				 pos.z(),
				 rootDrawableDetector->rotation());
		    }
		}
	    }
	}
    }
}

/// Loops over and draw the tracks in the given container
/// A filter can optionally be provided to select the tracks to be drawn
/// A drawing policy can optionally be provided to determine the attributes
/// of the track being drawn. 
/// If no filter is given, all tracks are drawn.
/// If no policy is given all tracks are shown with the same default policy.
/// <p>
/// container : ptr to container of tracks to be drawn
/// filter    : ptr to filter to be used to decide which track shall be drawn
/// policy    : ptr to drawing policy used to determine the color, style attributes of the track.
void EventDisplay::draw(StiTrackContainer * container,
			Filter<StiTrack>  * filter, 
			DrawingPolicy<StiDrawable> * policy)
{  
  //cout << "EventDisplay::draw() -I- Started" << endl;
  if (!container) return;
  //cout << "EventDisplay::draw() -I- Processing tracks from  container:"<< container->getName()<<endl;
  for (TrackMap::const_iterator i=container->begin();i!=container->end(); ++i) 
    {
      if (filter)
	{
	  if (!filter->filter( (*i).second) )
	      continue;
	}
      StiRootDrawableTrack * t = dynamic_cast<StiRootDrawableTrack *>((*i).second);
      if (!t) continue; // cast failed...
      t->setColor(1);
      t->setStyle(1);
      t->setSize(1.);
      if (policy)
	policy->police(t);
      t->draw(); 
    }
  //cout << "EventDisplay::draw() -I- Done" << endl;
}

/// drawHits loops over and draw the hits in the given container
/// A filter can optionally be provided to select the hits to be drawn
/// A drawing policy can optionally be provided to determine the attributes
/// of the hit being drawn. 
/// If no filter is given, all hits are drawn.
/// If no policy is given all hits are shown with the same default policy.
/// <p>
/// container : ptr to container of hits to be drawn
/// filter    : ptr to filter to be used to decide which hit shall be drawn
/// policy    : ptr to drawing policy used to determine the color, style attributes of the hit.
void EventDisplay::draw(StiHitContainer * container,
			Filter<StiHit>  * filter, 
			DrawingPolicy<StiDrawable> * policy)
{  
  if (!container) return;
  _usedHits.reset();
  _unusedHits.reset();
  const hitmap& map = container->hits();
  for (hitmap::const_iterator it=map.begin(); it!=map.end(); it++) 
    {
      const hitvector &hits = (*it).second.theHitVec;
       for (hitvector::const_iterator iter=hits.begin();iter!=hits.end();iter++)
	 {
	   const StiHit * hit = (*iter);
	   //if (!filter && !filter->filter(hit)) continue;
	   if (!hit) 
	       continue;
	   if (hit->timesUsed()>0)
	     {
	       _usedHits.add(hit->x_g(), hit->y_g(), hit->z_g() );
	     }
	   else
	     {
	       _unusedHits.add(hit->x_g(), hit->y_g(), hit->z_g() );
	     }
	 }
    }
  _usedHits.setColor(5);
  _usedHits.setStyle(8);
  //_usedHits.setSize(5.);
  _usedHits.draw();
  _unusedHits.setColor(6);
  _unusedHits.setStyle(7);
  //_unusedHits.setSize(5.);
  _unusedHits.draw();
  //cout << "EventDisplay::draw() -I- Done" << endl;
}

void EventDisplay::reset()
{
}

/// Create hit and track filters used by this event display.
void EventDisplay::createFilters()
{
  //cout << "EventDisplay::createFilters() -I- Started" << endl;
  if (_hitFilter)
    {
    }
  else
    {
      cout << "EventDisplay::createFilters() -I- No hit filtered available" << endl;
    }	
  // Track Filter
  //cout << "EventDisplay::createFilters() -I- Setting up reco track filter" << endl;
  _trackFilter = new StiDefaultTrackFilter("TrackFilter","Reconstructed Tracks Filter"); 
  _trackFilter->add(new EditableParameter("Chi2Used", "Use Chi2",     false, false, 0,1,1,Parameter::Boolean, StiTrack::kChi2));
  _trackFilter->add(new EditableParameter("Chi2Min",  "Minimum Chi2", 0., 0., 0., 100.,1,Parameter::Double, StiTrack::kChi2));
  _trackFilter->add(new EditableParameter("Chi2Max",  "Maximum Chi2", 20., 20., 0., 100.,1,Parameter::Double, StiTrack::kChi2));
  _trackFilter->add(new EditableParameter("PhiUsed",  "Use Phi",     false, false, 0,1,1,Parameter::Boolean, StiTrack::kPhi));
  _trackFilter->add(new EditableParameter("PhiMin",   "Minimum Phi", 0.,   0.,  0., 6.3,2,Parameter::Double, StiTrack::kPhi));
  _trackFilter->add(new EditableParameter("PhiMax",   "Maximum Phi", 6.3, 6.3, 0., 6.3,2,Parameter::Double, StiTrack::kPhi));
  _trackFilter->add(new EditableParameter("PtUsed",   "Use Pt",     false, false, 0,1,1,Parameter::Boolean, StiTrack::kPt));
  _trackFilter->add(new EditableParameter("PtMin",    "Minimum Pt", 0., 0., 0., 100.,2,Parameter::Double, StiTrack::kPt));
  _trackFilter->add(new EditableParameter("PtMax",    "Maximum Pt", 10., 10., 0., 100.,2,Parameter::Double, StiTrack::kPt));
  _trackFilter->add(new EditableParameter("PUsed",    "Use P",     false, false, 0,1,1,Parameter::Boolean, StiTrack::kP));
  _trackFilter->add(new EditableParameter("PMin",     "Minimum P", 0., 0., 0., 100.,2,Parameter::Double, StiTrack::kP));
  _trackFilter->add(new EditableParameter("PMax",     "Maximum P", 10., 10., 0., 100.,2,Parameter::Double, StiTrack::kP));
  _trackFilter->add(new EditableParameter("EtaUsed",  "Use Eta",     false, false, 0,1,1,Parameter::Boolean, StiTrack::kPseudoRapidity));
  _trackFilter->add(new EditableParameter("EtaMin",   "Min Eta", -1.5, -1.5, -10., 10.,2,Parameter::Double, StiTrack::kPseudoRapidity));
  _trackFilter->add(new EditableParameter("EtaMax",   "Max Eta",  1.5,  1.5, -10., 10.,2,Parameter::Double, StiTrack::kPseudoRapidity));
  _trackFilter->add(new EditableParameter("nPtsUsed", "Use nPts",     false, false, 0,1,1,Parameter::Boolean, StiTrack::kPointCount));
  _trackFilter->add(new EditableParameter("nPtsMin",  "Minimum nPts", 0., 0., 0., 100.,1,Parameter::Integer, StiTrack::kPointCount));
  _trackFilter->add(new EditableParameter("nPtsMax",  "Maximum nPts", 60., 60., 0., 100.,1,Parameter::Integer, StiTrack::kPointCount));
  _trackFilter->add(new EditableParameter("nGapsUsed","Use nGaps",     false, false, 0,1,1,Parameter::Boolean, StiTrack::kGapCount));
  _trackFilter->add(new EditableParameter("nGapsMin", "Minimum nGaps", 0., 0., 0., 100.,1,Parameter::Integer, StiTrack::kGapCount));
  _trackFilter->add(new EditableParameter("nGapsMax", "Maximum nGaps", 60., 60., 0., 100.,1,Parameter::Integer, StiTrack::kGapCount));
  _trackFilter->add(new EditableParameter("chargeUsed","Use Charge",     false, false, 0,1,1,Parameter::Boolean, StiTrack::kCharge));
  _trackFilter->add(new EditableParameter("chargeMin", "Min Charge", -1., -1., -100.,   100.,1,Parameter::Integer, StiTrack::kCharge));
  _trackFilter->add(new EditableParameter("chargeMax", "Max Charge",  1.,  1., -100.,   100.,1,Parameter::Integer, StiTrack::kCharge));
  Observer * obs = dynamic_cast<Observer *>(this);
  dynamic_cast<Subject*>(_trackFilter)->attach(obs);
  //cout << "EventDisplay::createFilters() -I- Setup of reco track filter completed" << endl;
  //cout << "EventDisplay::createFilters() -I- Setting up mc track filter" << endl;
  _mcTrackFilter = new StiDefaultTrackFilter("McTrackFilter","MC Tracks Filter"); 
  _mcTrackFilter->add(new EditableParameter("PhiUsed",  "Use Phi",     false, false, 0,1,1,Parameter::Boolean, StiTrack::kPhi));
  _mcTrackFilter->add(new EditableParameter("PhiMin",   "Minimum Phi", 0.,   0.,  0., 6.3,2,Parameter::Double, StiTrack::kPhi));
  _mcTrackFilter->add(new EditableParameter("PhiMax",   "Maximum Phi", 6.3, 6.3, 0., 6.3,2,Parameter::Double, StiTrack::kPhi));
  _mcTrackFilter->add(new EditableParameter("PtUsed",   "Use Pt",     false, false, 0,1,1,Parameter::Boolean, StiTrack::kPt));
  _mcTrackFilter->add(new EditableParameter("PtMin",    "Minimum Pt", 0., 0., 0., 100.,2,Parameter::Double, StiTrack::kPt));
  _mcTrackFilter->add(new EditableParameter("PtMax",    "Maximum Pt", 10., 10., 0., 100.,2,Parameter::Double, StiTrack::kPt));
  _mcTrackFilter->add(new EditableParameter("PUsed",    "Use P",     false, false, 0,1,1,Parameter::Boolean, StiTrack::kP));
  _mcTrackFilter->add(new EditableParameter("PMin",     "Minimum P", 0., 0., 0., 100.,2,Parameter::Double, StiTrack::kP));
  _mcTrackFilter->add(new EditableParameter("PMax",     "Maximum P", 10., 10., 0., 100.,2,Parameter::Double, StiTrack::kP));
  _mcTrackFilter->add(new EditableParameter("EtaUsed",  "Use Eta",     false, false, 0,1,1,Parameter::Boolean, StiTrack::kPseudoRapidity));
  _mcTrackFilter->add(new EditableParameter("EtaMin",   "Min Eta", -1.5, -1.5, -10., 10.,2,Parameter::Double, StiTrack::kPseudoRapidity));
  _mcTrackFilter->add(new EditableParameter("EtaMax",   "Max Eta",  1.5,  1.5, -10., 10.,2,Parameter::Double, StiTrack::kPseudoRapidity));
  _mcTrackFilter->add(new EditableParameter("nPtsUsed", "Use nPts",     false, false, 0,1,1,Parameter::Boolean, StiTrack::kPointCount));
  _mcTrackFilter->add(new EditableParameter("nPtsMin",  "Minimum nPts", 0., 0., 0., 100.,1,Parameter::Integer, StiTrack::kPointCount));
  _mcTrackFilter->add(new EditableParameter("nPtsMax",  "Maximum nPts", 60., 60., 0., 100.,1,Parameter::Integer, StiTrack::kPointCount));
  _mcTrackFilter->add(new EditableParameter("chargeUsed","Use Charge",     false, false, 0,1,1,Parameter::Boolean, StiTrack::kCharge));
  _mcTrackFilter->add(new EditableParameter("chargeMin", "Min Charge", -1., -1., -100.,   100.,1,Parameter::Integer, StiTrack::kCharge));
  _mcTrackFilter->add(new EditableParameter("chargeMax", "Max Charge",  1.,  1., -100.,   100.,1,Parameter::Integer, StiTrack::kCharge));
  obs = dynamic_cast<Observer *>(this);
  dynamic_cast<Subject*>(_mcTrackFilter)->attach(obs);
  //cout << "EventDisplay::createFilters() -I- Setup of reco track filter completed" << endl;
  //cout << "EventDisplay::createFilters() -I- Done" << endl;
}


void 	EventDisplay::createPolicies()
{
  //cout << "EventDisplay::createPolicies() -I- Started" << endl;
  _defaultHitDrawingPolicy = new DefaultDrawingPolicy("DefaultHitDrawingPolicy","Default Hit Draw Policy",1,1,0.2);
  _defaultTrackDrawingPolicy = new DefaultDrawingPolicy("DefaultTrackDrawingPolicy","Default Track Draw Policy",2,1,1.);
  _defaultMcTrackDrawingPolicy =  new DefaultDrawingPolicy("DefaultMcTrackDrawingPolicy","Default McTrack Draw Policy",4,2,1.);
  _hitDrawingPolicy = _defaultHitDrawingPolicy;
  _trackDrawingPolicy = _defaultTrackDrawingPolicy;
  _mcTrackDrawingPolicy = _defaultMcTrackDrawingPolicy;
  //_hitDrawingPolicies->add(_defaultHitDrawingPolicy);
  //_trackDrawingPolicies->add(_defaultTrackDrawingPolicy);
  //_mcTrackDrawingPolicies->add(_defaultMcTrackDrawingPolicy);
  //cout << "EventDisplay::createPolicies() -I- Done" << endl;
}

TGWindow * EventDisplay::getWindow()
{
  return dynamic_cast<TGWindow *>(this);
}

TGClient * EventDisplay::getClient()
{
  return fClient;
}

StChain * EventDisplay::getChain()
{
  return _chain;
}

StIOMaker * EventDisplay::getIoMaker()
{
  return _ioMaker;
}

StiToolkit * EventDisplay::getToolkit()
{
  return _toolkit;
}

StiDetectorContainer     * EventDisplay::getDetectorContainer()
{
  return _detectorContainer;
}

StiHitContainer          * EventDisplay::getHitContainer()
{
  return _hitContainer;
}

StiHitContainer          * EventDisplay::getMcHitContainer()
{
  return _mcHitContainer;
}

StiTrackContainer        * EventDisplay::getTrackContainer()
{
  return _trackContainer;
}

StiTrackContainer        * EventDisplay::getMcTrackContainer()
{
  return _mcTrackContainer;
}

EditableFilter<StiHit>   * EventDisplay::getHitFilter()
{
  return _hitFilter;
}

EditableFilter<StiTrack> * EventDisplay::getTrackFilter()
{
  return _trackFilter;
}

EditableFilter<StiTrack> * EventDisplay::getMcTrackFilter()
{
  return _mcTrackFilter;
}


EventDisplayParameters * EventDisplay::getOptions()
{
  return _options;
}


void EventDisplay::setStChain(StChain * chain)
{
  _chain = chain;
}

void EventDisplay::setIoMaker(StIOMaker * ioMaker)
{
  _ioMaker = ioMaker;
}

void EventDisplay::getNewState()
{
  draw();
}
