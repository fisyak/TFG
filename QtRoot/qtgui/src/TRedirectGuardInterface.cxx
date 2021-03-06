// @(#)root/gui:$Id: TRedirectGuardInterface.cxx,v 1.2 2013/08/30 16:00:26 perev Exp $
// Author: G. Ganis   10/10/2005

/*************************************************************************
 * Copyright (C) 1995-2005, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TRedirectGuardInterface                                                //
//                                                                      //
// This class provides output redirection to a TGTextView in guaranteed //
// exception safe way. Use like this:                                   //
// {                                                                    //
//    TRedirectGuardInterface guard(textview);                            //
//    ... // do something                                               //
//    guard.Update();                                                   //
//    ... // do something else                                          //
// }                                                                    //
// when guard goes out of scope, Update() is called to flush what left  //
// on the screed and the output is automatically redirected again to    //
// the standard units.                                                  //
// The exception mechanism takes care of calling the dtors              //
// of local objects so it is exception safe.                            //
// Optionally the output can also be saved into a file:                 //
// {                                                                    //
//    TRedirectGuardInterface guard(textview, file, mode);                //
//    ... // do something                                               //
// }                                                                    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <errno.h>
#include <sys/types.h>
#ifdef WIN32
#   include <io.h>
#else
#   include <unistd.h>
#endif

#include "TError.h"
#include "TRedirectGuardInterface.h"
#include "TSystem.h"

//_____________________________________________________________________________
TRedirectGuardInterface::TRedirectGuardInterface(const char *flog,
                                             const char *mode)
{
   // Create output redirection guard.
   // Text is added to the existing text via AddLine interface
   // If defined, 'flog' is interpreted as the path of a file
   // where to save the output; in such a case 'mode' if the
   // opening mode of the file (either "w" or "a").
   // By default a temporary file is used.

   fLogFile = flog;
   fTmpFile = kFALSE;
   fLogFileRead = 0;
   if (!flog) {
      // Create temporary file
      fLogFile = "RedirOutputGuard_";
      fLogFileRead = gSystem->TempFileName(fLogFile);
      if (!fLogFileRead) {
         Error("TRedirectGuardInterface", "could create temp file");
         return;
      }
      fTmpFile = kTRUE;

      // We need it in read mode
      fclose(fLogFileRead);
   } else {
      // Check permissions, if existing
      if (!gSystem->AccessPathName(flog, kFileExists)) {
         if (gSystem->AccessPathName(flog,
               (EAccessMode)(kWritePermission | kReadPermission))) {
            Error("TRedirectGuardInterface",
                  "no write or read permission on file: %s", flog);
            return;
         }
      }
   }

   // Redirect
   const char *m = (mode[0] != 'a' && mode[0] != 'w') ? "a" : mode;
   if (gSystem->RedirectOutput(fLogFile.Data(), m) == -1) {
      Error("TRedirectGuardInterface","could not redirect output");
      return;
   }

   // Open file in read mode
   if ((fLogFileRead = fopen(fLogFile.Data(),"r"))) {
      // Start reading from the present end
      lseek(fileno(fLogFileRead),(off_t)0, SEEK_END);
   } else {
      Error("TRedirectGuardInterface","could not open file in read mode");
      return;
   }

   return;
}

//_____________________________________________________________________________
TRedirectGuardInterface::~TRedirectGuardInterface()
{
   // Destructor.

   // Display last info
   Update();

   // Close the file
   if (fLogFileRead)
      fclose(fLogFileRead);

   // Unlink the file if we are the owners
   if (fTmpFile && fLogFile.Length() > 0)
      gSystem->Unlink(fLogFile);

   // Restore standard output
   gSystem->RedirectOutput(0);
}

//_____________________________________________________________________________
void TRedirectGuardInterface::Update()
{
   // Send to text frame the undisplayed content of the file.

   if (!fLogFileRead) {
      Warning("Update","no file open for reading");
      return;
   }

   // Make sure you get anything
   fflush(stdout);

   char line[4096];
   while (fgets(line,sizeof(line),fLogFileRead)) {

      // Get read of carriage return
      if (line[strlen(line)-1] == '\n')
         line[strlen(line)-1] = 0;

      // Send line to the TGTextView
      AddLine(line);
   }
}
