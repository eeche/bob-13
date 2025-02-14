TARGET=sc
EXE_NAME=$(TARGET).exe

CC=cl
INC_PATH=
CFLAGS=$(INC_PATH) /nologo /Zi /c /EHsc /W3 /D _CRT_SECURE_NO_DEPRECATE /TP

LINK=link
LIBS=Kernel32.lib User32.lib
LIB_PATH=
OUTDIR=./rst
LDFLAGS=$(LIBS) $(LIB_PATH) /nologo /DEBUG /PDB:$(OUTDIR)/$(TARGET).pdb /Out:$(OUTDIR)/$(EXE_NAME) /map

OBJ=main.obj sc.obj
EXE_OBJ=$(OBJ)

SRC=$(OBJ:.obj=.cc)

$(EXE_NAME): $(OBJ)
	$(LINK) $(LDFLAGS) $(EXE_OBJ)

%.obj:%.cc
	$(CC) $(CFLAGS) /Fo$@ $<

clean:
	del /f /q *.obj
	del /f /q .\rst\*.*