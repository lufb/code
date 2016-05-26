################OPTION###################
CCOMPILE = gcc -m32 
CPPCOMPILE = g++ -m32
COMPILEOPTION = -c -w -g -static -D LINUXCODE -D __stdcall -D __stdcall -D _DEBUG
INCLUDEDIR = -I./ \
			-I./compress/			\
			-I./mask/				\
			-I./log				\
			-I./sock				\
			-I./base				\
			-I./global			\
			-I./core				\
			-I./pro				\
			-I./option			\
			-I./openssl/include	\
			-I./zlib/include	\
			-I./lib
 
LINK = g++ -m32
LINKOPTION = -o argo_cli
LIBDIRS =  -lssl -lz -pthread 

OBJS:=
OBJS+=./main.o
OBJS+=./argo_cli.o
OBJS+=./base/argo_base.o
OBJS+=./base/argo_ckcrc16.o
OBJS+=./base/MCheckCode.o
OBJS+=./base/MDateTime.o
OBJS+=./base/MDll.o
OBJS+=./base/MErrorCode.o
OBJS+=./base/MFile.o
OBJS+=./base/MIniFile.o
OBJS+=./base/MinuteStatistics.o
OBJS+=./base/MMutex.o
OBJS+=./base/MSelfRiseArray.o
OBJS+=./base/MString.o
OBJS+=./base/MSystemInfo.o
OBJS+=./base/MThread.o
OBJS+=./base/MTypeDefine.o
OBJS+=./compress/zlib_compress.o
OBJS+=./core/argo_cli_mgr.o
OBJS+=./global/argo_global.o
OBJS+=./mask/mask_decom.o
OBJS+=./option/argo_option.o
OBJS+=./sock/argo_sock.o
OBJS+=./log/argo_log.o



OUTPUT = ./argo_cli
################OPTION END################
$(OUTPUT):$(OBJS)
	$(LINK)  $(OBJS) $(LINKOPTION) $(LIBDIRS)

clean: 
	rm -f $(OBJS)
	rm -f $(OUTPUT)
all: clean $(OUTPUT)
.SUFFIXES:
.SUFFIXES:  .c .o .cpp

.cpp.o:
	$(CPPCOMPILE) -o $*.o $(COMPILEOPTION) $(INCLUDEDIR)  $*.cpp

.c.o:
	$(CCOMPILE) -o $*.o $(COMPILEOPTION) $(INCLUDEDIR) $*.c
