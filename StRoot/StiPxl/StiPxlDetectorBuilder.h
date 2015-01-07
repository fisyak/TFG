#ifndef StiPxlDetectorBuilder_h
#define StiPxlDetectorBuilder_h

#include "Sti/StiDetectorBuilder.h"

class StPxlDb;


class StiPxlDetectorBuilder : public StiDetectorBuilder
{
public:

   StiPxlDetectorBuilder(bool active, bool buildIdealGeom=true);
   virtual void buildDetectors(StMaker &source);
   virtual void useVMCGeometry();
   const StiDetector* getActiveDetector(int sector, int ladder, int sensorHalf) const;

protected:

   bool         mBuildIdealGeom;
   StPxlDb     *mPxlDb;

private:

   void buildInactiveVolumes();
   static void convertSensor2StiId(int sector, int ladder, int sensorHalf, int& stiRow, int& stiSensor);
};

#endif
