#include <iostream>
#include "StiHit.h"
#include "Messenger.h"
#include "StiDetector.h"
#include "StiTrackNode.h"

ostream& operator<<(ostream& os, const StiDetector& d);

StiDetector * StiTrackNode::getDetector() const
{
    if (hit==0)
	{
	    if (detector==0)
		*(Messenger::instance(MessageType::kNodeMessage)) << "StiTrackNode::getDetector() - Fatal Error - Detector improperly set to '0'" << endl;
	    else
		*(Messenger::instance(MessageType::kNodeMessage)) << "StiTrackNode::getDetector() - Detector  :" << *detector << endl;
	}
    else
	{
	    StiDetector * d =  hit->detector();
	    if (d==0)
		*(Messenger::instance(MessageType::kNodeMessage)) << "StiTrackNode::getDetector() - Fatal Error - Node has hit that has no associated detector" << endl;
	    else
		*(Messenger::instance(MessageType::kNodeMessage)) << "StiTrackNode::getDetector() - Detector associated with hit :" << *d << endl;
	}
    return( hit==0 ? detector : hit->detector() );
}

void StiTrackNode::setDetector(const StiDetector *pDetector){
    //if(hit==0){ 
    detector = pDetector; 
    if (detector!=0)
	*(Messenger::instance(MessageType::kNodeMessage)) << "StiTrackNode::setDetector() - Detector set to :" << *detector << endl;
    else
	*(Messenger::instance(MessageType::kNodeMessage)) << "StiTrackNode::setDetector() - Fatal Error - Detector improperly set to '0'" << endl;
}


//_____________________________________________________________________________
void StiTrackNode::reset()
{ 
    StiDefaultMutableTreeNode::reset();
    hit      = 0;
    detector = 0;
}


//_____________________________________________________________________________
void StiTrackNode::set(int depth, StiHit * h)
{
    StiDefaultMutableTreeNode::set(depth);
    hit = h;
}

//_____________________________________________________________________________
void StiTrackNode::setAsCopyOf(const StiTrackNode * node)
{
    StiDefaultMutableTreeNode::setAsCopyOf(node);
    hit = node->hit;
}


