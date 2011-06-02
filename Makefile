PACKAGES=libusb
LIBS=`pkg-config --libs ${PACKAGES}`
INCS=`pkg-config --cflags ${PACKAGES}` -Wall

LIBSRC=dcdc-usb-comm.c dcdc-usb-proto.c
MAINSRC=dcdc-usb-main.c
OBJS=dcdc-usb-main.o

all: libdcdc-usb.so dcdc-usb
install: all
	install -v -m755 dcdc-usb /usr/local/bin/
	install -v -m755  libdcdc-usb.so  /usr/local/lib
	ldconfig

$(OBJS): dcdc-usb.h
.c.o:
	$(CC) $(CFLAGS) $(INCS) -c $(LIBSRC) $(MAINSRC)

libdcdc-usb.so: $(LIBSRC)
	$(CC) $(CFLAGS) -shared -fPIC -L. $(LIBSRC) -Wl,-soname,$@ -o $@ 

dcdc-usb: $(MAINSRC)
	$(CC) $(CFLAGS) $(MAINSRC) -o $@ $(LIBS) -L. -ldcdc-usb

clean:
	rm -rf *.o *.so dcdc-usb
