# complied objects
OBJECTS_DIR  = build/o

# generated files
MOC_DIR      = build/gen/moc
UI_DIR       = build/gen/ui
DESTDIR      = ./

# try to use some c++0x features
!win32:QMAKE_CXXFLAGS += -std=c++0x
