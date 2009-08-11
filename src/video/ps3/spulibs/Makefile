# This Makefile is for building the CELL BE SPU libs
# libfb_writer_spu.so, libyuv2rgb_spu.so, libbilin_scaler_spu.so

# Toolchain
PPU_LD=/usr/bin/ld
SPU_SRCDIR=$(srcdir)/src/video/ps3/spulibs
SPU_LIBDIR=$(srcdir)/src/video/ps3/spulibs/libs
SPU_CFLAGS=-g -W -Wall -Winline -Wno-main -I. -I /usr/spu/include -I /opt/cell/sdk/usr/spu/include -finline-limit=10000 -Winline -ftree-vectorize -funroll-loops -fmodulo-sched -ffast-math -fPIC -O2


ps3libs: $(SPU_LIBDIR)/libfb_writer_spu.a $(SPU_LIBDIR)/libfb_writer_spu.so \
				$(SPU_LIBDIR)/libyuv2rgb_spu.so $(SPU_LIBDIR)/libyuv2rgb_spu.a \
				$(SPU_LIBDIR)/libbilin_scaler_spu.so $(SPU_LIBDIR)/libbilin_scaler_spu.a

$(SPU_LIBDIR):
	$(SHELL) $(auxdir)/mkinstalldirs $(SPU_LIBDIR)

# fb_writer (basically copying from a to b)
$(SPU_LIBDIR)/fb_writer_spu-embed.o: $(SPU_LIBDIR) $(SPU_SRCDIR)/fb_writer.c $(SPU_SRCDIR)/spu_common.h
	$(SPU_GCC) $(SPU_CFLAGS) -o $(SPU_LIBDIR)/fb_writer_spu $(SPU_SRCDIR)/fb_writer.c -lm
	$(EMBEDSPU) -m32 fb_writer_spu $(SPU_LIBDIR)/fb_writer_spu $(SPU_LIBDIR)/fb_writer_spu-embed.o

$(SPU_LIBDIR)/libfb_writer_spu.a: $(SPU_LIBDIR)/fb_writer_spu-embed.o
	$(AR) -qcs $(SPU_LIBDIR)/libfb_writer_spu.a $(SPU_LIBDIR)/fb_writer_spu-embed.o

$(SPU_LIBDIR)/libfb_writer_spu.so: $(SPU_LIBDIR)/fb_writer_spu-embed.o
	$(PPU_LD) -o $(SPU_LIBDIR)/libfb_writer_spu.so -shared -soname=libfb_writer_spu.so $(SPU_LIBDIR)/fb_writer_spu-embed.o


# yuv2rgb_converter (converting YV12/IYUV to RGB)
$(SPU_LIBDIR)/yuv2rgb_spu-embed.o: $(SPU_LIBDIR) $(SPU_SRCDIR)/yuv2rgb_converter.c $(SPU_SRCDIR)/spu_common.h
	$(SPU_GCC) $(SPU_CFLAGS) -o $(SPU_LIBDIR)/yuv2rgb_spu $(SPU_SRCDIR)/yuv2rgb_converter.c -lm
	$(EMBEDSPU) -m32 yuv2rgb_spu $(SPU_LIBDIR)/yuv2rgb_spu $(SPU_LIBDIR)/yuv2rgb_spu-embed.o

$(SPU_LIBDIR)/libyuv2rgb_spu.a: $(SPU_LIBDIR)/yuv2rgb_spu-embed.o
	$(AR) -qcs $(SPU_LIBDIR)/libyuv2rgb_spu.a $(SPU_LIBDIR)/yuv2rgb_spu-embed.o

$(SPU_LIBDIR)/libyuv2rgb_spu.so: $(SPU_LIBDIR)/yuv2rgb_spu-embed.o
	$(PPU_LD) -o $(SPU_LIBDIR)/libyuv2rgb_spu.so -shared -soname=libyuv2rgb_spu.so $(SPU_LIBDIR)/yuv2rgb_spu-embed.o


# bilin_scaler (scaling bilinear YV12/IYUV pictures with resolutions /16)
$(SPU_LIBDIR)/bilin_scaler_spu-embed.o: $(SPU_LIBDIR) $(SPU_SRCDIR)/bilin_scaler.c $(SPU_SRCDIR)/spu_common.h
	$(SPU_GCC) $(SPU_CFLAGS) -o $(SPU_LIBDIR)/bilin_scaler_spu $(SPU_SRCDIR)/bilin_scaler.c -lm
	$(EMBEDSPU) -m32 bilin_scaler_spu $(SPU_LIBDIR)/bilin_scaler_spu $(SPU_LIBDIR)/bilin_scaler_spu-embed.o

$(SPU_LIBDIR)/libbilin_scaler_spu.a: $(SPU_LIBDIR)/bilin_scaler_spu-embed.o
	$(AR) -qcs $(SPU_LIBDIR)/libbilin_scaler_spu.a $(SPU_LIBDIR)/bilin_scaler_spu-embed.o

$(SPU_LIBDIR)/libbilin_scaler_spu.so: $(SPU_LIBDIR)/bilin_scaler_spu-embed.o
	$(PPU_LD) -o $(SPU_LIBDIR)/libbilin_scaler_spu.so -shared -soname=libbilin_scaler_spu.so $(SPU_LIBDIR)/bilin_scaler_spu-embed.o


ps3libs-install: $(SPU_LIBDIR)/libfb_writer_spu.a $(SPU_LIBDIR)/libfb_writer_spu.so \
				$(SPU_LIBDIR)/libyuv2rgb_spu.so $(SPU_LIBDIR)/libyuv2rgb_spu.a \
				$(SPU_LIBDIR)/libbilin_scaler_spu.so $(SPU_LIBDIR)/libbilin_scaler_spu.a
	$(INSTALL) -c -m 0755 $(SPU_LIBDIR)/libfb_writer_spu.so $(DESTDIR)$(libdir)/.
	$(INSTALL) -c -m 0655 $(SPU_LIBDIR)/libfb_writer_spu.a $(DESTDIR)$(libdir)/.
	$(INSTALL) -c -m 0755 $(SPU_LIBDIR)/libyuv2rgb_spu.so $(DESTDIR)$(libdir)/.
	$(INSTALL) -c -m 0655 $(SPU_LIBDIR)/libyuv2rgb_spu.a $(DESTDIR)$(libdir)/.
	$(INSTALL) -c -m 0755 $(SPU_LIBDIR)/libbilin_scaler_spu.so $(DESTDIR)$(libdir)/.
	$(INSTALL) -c -m 0655 $(SPU_LIBDIR)/libbilin_scaler_spu.a $(DESTDIR)$(libdir)/.

ps3libs-uninstall:
	rm -f $(DESTDIR)$(libdir)/libfb_writer_spu.a
	rm -f $(DESTDIR)$(libdir)/libfb_writer_spu.so
	rm -f $(DESTDIR)$(libdir)/libyuv2rgb_spu.so
	rm -f $(DESTDIR)$(libdir)/libyuv2rgb_spu.a
	rm -f $(DESTDIR)$(libdir)/libbilin_scaler_spu.so
	rm -f $(DESTDIR)$(libdir)/libbilin_scaler_spu.a

ps3libs-clean:
	rm -f $(SPU_LIBDIR)/* 
