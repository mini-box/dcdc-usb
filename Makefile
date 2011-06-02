PACKAGES=libusb
LIBS=`pkg-config --libs ${PACKAGES}`
INCS=`pkg-config --cflags ${PACKAGES}` -Wall

.c.o:
	$(CC) $(CFLAGS) $(INCS) -c $*.c

OBJS=dcdc-usb-main.o dcdc-usb-comm.o dcdc-usb-proto.o

dcdc-usb: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

$(OBJS): dcdc-usb.h

clean:
	rm -rf *.o dcdc-usb
