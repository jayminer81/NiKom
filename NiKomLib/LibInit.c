#define  _USEOLDEXEC_ 1
#include <exec/types.h>
#include <exec/nodes.h>
#include <exec/memory.h>
#include <exec/resident.h>
#include <exec/libraries.h>
#include <exec/execbase.h>
#include <libraries/dos.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <string.h>

#include "NiKomBase.h"

/* Prototypes */
ULONG __saveds __asm _LibExpunge( register __a6 struct NiKomBase *libbase );
ULONG __saveds __asm _LibInit(register __a0 APTR seglist,
			      register __d0 struct NiKomBase *libbase );

int   __saveds __asm __UserLibInit(register __a6 struct NiKomBase * NiKomBase);
void  __saveds __asm __UserLibCleanup(register __a6 struct NiKomBase * NiKomBase);

typedef LONG (*myPFL)();   /* pointer to function returning 32-bit int        */

/* library initialization table, used for AUTOINIT libraries                */
struct InitTable {
        ULONG        *it_DataSize;       /* library data space size         */
        myPFL        *it_FuncTable;      /* table of entry points           */
        APTR         it_DataInit;        /* table of data initializers      */
        myPFL        it_InitFunc;        /* initialization function to run  */
};


/* symbols generated by blink */
extern char __far _LibID[];             /* ID string                        */
extern char __far _LibName[];           /* Name string                      */
extern char __far RESLEN;               /* size of init data                */
extern long __far NEWDATAL;             /* size of global data              */
extern long __far NUMJMPS;              /* number of jmp vectors to copy    */
extern myPFL _LibFuncTab[];             /* my function table                */
extern long __far _LibVersion;          /* Version of library               */
extern long __far _LibRevision;         /* Revision of library              */
#define MYVERSION ((long)&_LibVersion)
#define MYREVISION ((long)&_LibRevision)
#define DATAWORDS ((long)&NEWDATAL)     /* magic to get right type of reloc */


/* From libent.o, needed to determine where data is loaded by loadseg       */
extern long far _Libmergeddata;

#define NIKOMBASESIZE ((sizeof(struct NiKomBase) +3) & ~3)



struct InitTable __far _LibInitTab =  {
        (long *)(&RESLEN+NIKOMBASESIZE),
        _LibFuncTab,
        NULL,                        /* will initialize my own data */
        _LibInit,
};

ULONG __saveds __asm
_LibInit(register __a0 APTR seglist,
	 register __d0 struct NiKomBase *NiKomBase )
{
#if 0
    long *reloc;
    long *sdata;
    char *ddata;
    long nrelocs;
#endif

    NiKomBase->seglist = (ULONG) seglist;

    /* init. library structure (since I don't do automatic data init.) */
    NiKomBase->lib.lib_Node.ln_Type = NT_LIBRARY;
    NiKomBase->lib.lib_Node.ln_Name =  _LibName;
    NiKomBase->lib.lib_Flags = LIBF_SUMUSED | LIBF_CHANGED;
    NiKomBase->lib.lib_Version = MYVERSION;
    NiKomBase->lib.lib_Revision = MYREVISION;
    NiKomBase->lib.lib_IdString = (APTR) _LibID;

#if 0
     /* Start of copy of global data after structure */
    ddata = (char *)NiKomBase + NIKOMBASESIZE;

    sdata = (long *)&_Libmergeddata; /* where loadseg loaded the data */
    memcpy(ddata, (void *)sdata, DATAWORDS*4);

    /* perform relocs if we want one global section for all programs */
    /* that have this lib open. If we want a global section for each */
    /* open, copy the relocs, and do them on each open call.         */
    sdata = sdata + DATAWORDS;
    nrelocs = *sdata;
    sdata++;
    while (nrelocs > 0)
    {
       reloc = (long *)((long)ddata + *sdata++);
       *reloc += (long)ddata;
       nrelocs--;
    }
#endif

    if (__UserLibInit(NiKomBase) != 0) {
       return NULL;   /* abort if user init failed */
    }
    return((ULONG)NiKomBase);
}

LONG __saveds __asm
_LibOpen( register __a6 struct NiKomBase *NiKomBase )
{
    /* mark us as having another customer */
    NiKomBase->lib.lib_OpenCnt++;

    /* clear delayed expunges (standard procedure)                */
    NiKomBase->lib.lib_Flags &= ~LIBF_DELEXP;

    return ( (LONG) NiKomBase );
}

ULONG __saveds __asm
_LibClose( register __a6 struct NiKomBase *NiKomBase )
{
    ULONG retval = 0;


    if (( --NiKomBase->lib.lib_OpenCnt == 0 ) &&
                        ( NiKomBase->lib.lib_Flags & LIBF_DELEXP ))
    {
        /* no more people have me open,
         * and I have a delayed expunge pending
         */
         retval = _LibExpunge( NiKomBase ); /* return segment list        */
    }

    return (retval);
}

ULONG __saveds __asm
_LibExpunge( register __a6 struct NiKomBase *NiKomBase )
{
    ULONG seglist = 0;
    LONG  libsize;

    NiKomBase->lib.lib_Flags |= LIBF_DELEXP;
    if ( NiKomBase->lib.lib_OpenCnt == 0 )
    {
	__UserLibCleanup(NiKomBase);

        /* really expunge: remove NiKomBase and freemem        */
        seglist = NiKomBase->seglist;

        Remove( (struct Node *) NiKomBase);

        libsize = NiKomBase->lib.lib_NegSize + NiKomBase->lib.lib_PosSize;
        FreeMem( (char *) NiKomBase - NiKomBase->lib.lib_NegSize,(LONG) libsize );
    }

    /* return NULL or real seglist                                */
    return ( (ULONG) seglist );
}
