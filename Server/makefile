CPU   = 68020
FLAGS = NMINC STRMERGE CPU=$(CPU) DEBUG=FULL ERROR=ALL IDIR=/Include IDIR=/UtilLib
OBJS  = NiKServer.o NiKHost.o NiKHost2.o NiKHost3.o NiKVersion.o NodeComm.o \
	Config.o Startup.o Shutdown.o

all: NiKServer

Debug:
  -makedir Debug

Debug/$(CPU): Debug
  -makedir Debug/$(CPU)

NoDebug:
  -makedir NoDebug

NoDebug/$(CPU): NoDebug
  -makedir NoDebug/$(CPU)

clean:
  -delete \#?.o
  -delete Debug NoDebug all

NiKServer: Debug/$(CPU) NoDebug/$(CPU) $(OBJS)
  slink FROM lib:c.o $(OBJS) TO Debug/$(CPU)/NiKServer \
        LIB lib:sc.lib+lib:amiga.lib+/UtilLib/Debug/$(CPU)/nikomutils.lib NOICONS
  slink FROM Debug/$(CPU)/NiKServer TO NoDebug/$(CPU)/NiKServer NODEBUG NOICONS

NiKServer.o: NiKServer.c /Include/NiKomStr.h
  sc $(FLAGS) NiKServer.c

Startup.o: Startup.c /Include/NiKomStr.h
  sc $(FLAGS) Startup.c

Shutdown.o: Shutdown.c /Include/NiKomStr.h
  sc $(FLAGS) Shutdown.c

NiKHost.o: NiKHost.c /Include/NiKomStr.h
  sc $(FLAGS) NiKHost.c

NiKHost2.o: NiKHost2.c /Include/NiKomStr.h
  sc $(FLAGS) NiKHost2.c

NiKHost3.o: NiKHost3.c /Include/NiKomStr.h
  sc $(FLAGS) NiKHost3.c

NiKVersion.o: NiKVersion.c /Include/VersionStrings.h
  sc $(FLAGS) NiKVersion.c

NodeComm.o: NodeComm.c /Include/NiKomStr.h
  sc $(FLAGS) NodeComm.c

Config.o: Config.c /Include/NiKomStr.h
  sc $(FLAGS) Config.c
