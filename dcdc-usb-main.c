#include <stdio.h>
#include <usb.h>

#include "dcdc-usb.h"


int main()
{
    struct usb_dev_handle *h;
    unsigned char *data;
    int ret;
    
    h = dcdc_connect();
    
    if (h == NULL) 
    {
	fprintf(stderr, "Cannot connect to DCDC-USB\n");
	return 1;
    }
    
    if (dcdc_setup(h) < 0)
    {
	fprintf(stderr, "Cannot setup device\n");
	return 2;
    }
    
    if ((ret = dcdc_get_status(h, &data)) < 0)
    {
	fprintf(stderr, "Failed to get status from device\n");
	return 3;
    }
    
    dcdc_parse_data(h, data, ret);
    
    free(data);
    return 0;
}