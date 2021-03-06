######################################################################
# Automatically generated by qmake (1.07a) Thu Jun 16 21:02:09 2005
######################################################################


TEMPLATE = lib

CONFIG += thread dll
CONFIG += create_prl

TARGET = QtGed

QMAKE_RPATH=
macx: QMAKE_LFLAGS_SONAME = -Wl,-install_name,@rpath/

QTROOTSYSPATHINSTALL = $(QTROOTSYSDIR)

QT_VERSION=$$[QT_VERSION]
contains( QT_VERSION, "^4.*" ) {
    QTROOTSYSPATHINSTALL = $$(QTROOTSYSDIR)
}

isEmpty(DESTDIR) {
  DESTDIR=..
}
isEmpty(QTROOTSYSPATHINSTALL) {
  QTROOTSYSPATHINSTALL = $$DESTDIR
}

GQTDIRI   = ../qt/inc

exists ($$GQTDIRI) {
  INCLUDEPATH *= $$GQTDIRI
  DEPENDPATH  *= "$$GQTDIRI"
}
exists (../qtgui/inc) {
  INCLUDEPATH *=  ../qtgui/inc
  DEPENDPATH  *=  ../qtgui/inc
}

QTGEDDIRS   = src
QTGEDDIRI   = inc

DEPENDPATH  += $$QTGEDDIRI $$QTGEDDIRS
INCLUDEPATH += $$QTGEDDIRI $$QTGEDDIRS

exists ($(ROOTSYS)/include){
   DEPENDPATH  *= $(ROOTSYS)/include
   INCLUDEPATH *= $(ROOTSYS)/include
}

QTGEDL      = $$QTGEDDIRI/LinkDef.h

# Input
HEADERS += $$QTGEDDIRI/TQtLineEditor.h  \
           $$QTGEDDIRI/TQtGraphEditor.h \
           $$QTGEDDIRI/TQtAxisEditor.h  \
           $$QTGEDDIRI/TQtArrowEditor.h \
           $$QTGEDDIRI/TQtGedFrames.h   \
           $$QTGEDDIRI/TQtGedAttFrame.h \
           $$QTGEDDIRI/TQtGedEditor.h   \
           $$QTGEDDIRI/TQtMarkerEditor.h\
           $$QTGEDDIRI/TQtPadEditor.h
           
SOURCES += $$QTGEDDIRS/TQtLineEditor.cxx  \
           $$QTGEDDIRS/TQtGraphEditor.cxx \
           $$QTGEDDIRS/TQtMarkerEditor.cxx\
           $$QTGEDDIRS/TQtAxisEditor.cxx  \
           $$QTGEDDIRS/TQtArrowEditor.cxx \
           $$QTGEDDIRS/TQtGedEditor.cxx   \
           $$QTGEDDIRS/TQtPadEditor.cxx   \
           $$QTGEDDIRS/TQtGedFrames.cxx   \
           $$QTGEDDIRS/TQtGedAttFrame.cxx
           
unix {
  LIBS += -L..
}

CREATE_ROOT_DICT_FOR_CLASSES  = $$HEADERS $$QTGEDL

    !exists ($$GQTDIRI/rootcintrule.pri){
        message "The rootcintrule.pri was not found"
    }
    exists ($$GQTDIRI/rootcintrule.pri){
       include ($$GQTDIRI/rootcintrule.pri)
    }

mac {
    !exists ($$GQTDIRI/rootlibs.pri){
        message "The rootlibs.pri was not found"
    }
    exists ($$GQTDIRI/rootlibs.pri){
       include ($$GQTDIRI/rootlibs.pri)
    }
    LIBS *= -L.. -lQtRootGui
}


QTROOTSYSDIRLIB = $$(QTROOTSYSDIR_LIB)
isEmpty( QTROOTSYSDIRLIB  ) {
    QTROOTSYSDIRLIB=lib
} 
target.path = $$QTROOTSYSPATHINSTALL/$$QTROOTSYSDIRLIB

headerfiles.path  = $$QTROOTSYSPATHINSTALL/include/
headerfiles.files = $$QTGEDDIRI/*.h
headerfiles.files -= $$QTGEDDIRI/LinkDef.h

plugins.path   = $$QTROOTSYSPATHINSTALL/etc/plugins/TVirtualPadEditor/
plugins.files  = plugins/TVirtualPadEditor/*.C

INSTALLS += headerfiles target plugins

DISTFILES += LICENSE.QPL Module.mk

unix {
#  -- working directory 
  
  ROOTCINT_DIR = .rootcint
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj

}
#The following line was inserted by qt3to4
QT +=  qt3support 
#The following line was inserted by qt3to4
QT +=  
