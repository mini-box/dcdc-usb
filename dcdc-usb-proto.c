#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <usb.h>

#include "dcdc-usb.h"

#define P(t, v...) fprintf(stderr, t "\n", ##v)

static int dcdc_send_command(struct usb_dev_handle *h, unsigned char cmd, unsigned char val)
{
    unsigned char c[5];
    c[0] = DCDCUSB_CMD_OUT;
    c[1] = cmd;
    c[2] = val;
    c[3] = 0;
    c[4] = 0;
    
    return dcdc_send(h, c, 5);
}

int dcdc_get_status(struct usb_dev_handle *h, unsigned char **data)
{
    unsigned char c[2];
    int ret = 0;
    
    *data = (unsigned char *)malloc(24);
    if (*data == NULL)
	return -1;
    
    c[0] = DCDCUSB_GET_ALL_VALUES;
    c[1] = 0;
    
    if (dcdc_send(h, c, 2) < 0)
    {
	fprintf(stderr, "Cannot send command to device\n");
	return -2;
    }
    
    if ((ret = dcdc_recv(h, *data, 24, 1000)) < 0)
    {
	fprintf(stderr, "Cannot get device status\n");
	return -3;
    }
    
    return ret;
}

int dcdc_parse_data(struct usb_dev_handle *h, unsigned char *data, int size)
{

    int i, mode, state, status;
    float ignition_voltage, input_voltage, output_voltage;
    
    if ((h == NULL) || (data == NULL))
	return -1;

    for (i = 0; i < size; i++)
    {
	if (i % 8 == 0) fprintf(stderr, "\n");
	fprintf(stderr, "[%02d] = 0x%02x ", i, data[i]);
    }
    fprintf(stderr, "\n");
    
    switch(data[0])
    {
	case DCDCUSB_RECV_ALL_VALUES:
	{
	    mode = data[1];
	    state = data[2];
	    input_voltage = (float) data[3] * 0.1558f;
	    ignition_voltage = (float) data[4] * 0.1558f;
	    output_voltage = (float) data[5] * 0.1170f;
	    status = data[6];
	    P("mode: %d", mode & 0x03);
	    P("time config: %d", (mode >> 5) & 0x07);
	    P("voltage config: %d", (mode >> 2) & 0x07);
	    P("state: %d", state);
	    P("input voltage: %.2f", input_voltage);
	    P("ignition voltage: %.2f", ignition_voltage);
	    P("output voltage: %2f", output_voltage);
	    P("power switch: %s", ((status & 0x04) ? "On":"Off"));
	    P("output enable: %s", ((status & 0x08) ? "On":"Off"));
	    P("aux vin enable %s", ((status & 0x10) ? "On":"Off"));
	    P("version: %d.%d", ((data[23] >> 5) & 0x07), (data[23] & 0x1F));
	}
	break;
	case DCDCUSB_CMD_IN:
	{
	}
	break;
	default:
	    fprintf(stderr, "Unknown message\n");
    }
    
    return 0;
}
