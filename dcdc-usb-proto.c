/* 
 * Copyright (c) 2011 by Mini-Box.com, iTuner Networks Inc.
 * Written by Nicu Pavel <npavel@mini-box.com>
 * All Rights Reserved
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>

#include <usb.h>

#include "dcdc-usb.h"

/* transforms user value to a value understood by device */
static int vout2dev(double vout)
{
    double rpot = (double)0.8 * CT_R1 / (vout - (double)0.8) - CT_R2;
    double result = (257 * (rpot-CT_RW) / CT_RP);

    if (result<0) result = 0;
    if (result>255) result = 255;

    return (unsigned char)result;
}

static int dcdc_send_command(struct usb_dev_handle *h, unsigned char cmd, unsigned char val)
{
    unsigned char c[3];
    c[0] = DCDCUSB_CMD_OUT;
    c[1] = cmd;
    c[2] = val;
    
    return dcdc_send(h, c, 3);
}

int dcdc_get_status(struct usb_dev_handle *h, unsigned char *buf, int buflen)
{
    unsigned char c[2];
    int ret = 0;
    
    if (buflen < MAX_TRANSFER_SIZE)
	return -1;
	
    c[0] = DCDCUSB_GET_ALL_VALUES;
    c[1] = 0;
    
    if (dcdc_send(h, c, 2) < 0)
    {
	fprintf(stderr, "Cannot send command to device\n");
	return -2;
    }
    
    if ((ret = dcdc_recv(h, buf, MAX_TRANSFER_SIZE, 1000)) < 0)
    {
	fprintf(stderr, "Cannot get device status\n");
	return -3;
    }
    
    return ret;
}

int dcdc_set_vout(struct usb_dev_handle *h, double vout)
{
    if (vout < 5) vout = 5;
    if (vout > 24) vout = 24;
    
    return dcdc_send_command(h, CMD_WRITE_VOUT, vout2dev(vout));
}

/* value will be shown on dcdc_parse_data() */
int dcdc_get_vout(struct usb_dev_handle *h, unsigned char *buf, int buflen)
{
    int ret = 0;
    
    if (buflen < MAX_TRANSFER_SIZE)
	return -1;

    if ((ret = dcdc_send_command(h, CMD_READ_VOUT, 0)) < 0)
	return ret;

    ret = dcdc_recv(h, buf, MAX_TRANSFER_SIZE, 1000);

    return ret;
}

int dcdc_parse_data(unsigned char *data, int size)
{
    if (data == NULL)
	return -1;

#ifdef DEBUG
    int i;
    for (i = 0; i < size; i++)
    {
	if (i % 8 == 0) fprintf(stderr, "\n");
	fprintf(stderr, "[%02d] = 0x%02x ", i, data[i]);
    }
    fprintf(stderr, "\n");
#endif

    switch(data[0])
    {
	case DCDCUSB_RECV_ALL_VALUES: dcdc_parse_values(data);break;
	case DCDCUSB_CMD_IN: dcdc_parse_cmd(data); break;
	case INTERNAL_MESG: dcdc_parse_internal_msg(data); break;
	case DCDCUSB_MEM_READ_IN: dcdc_parse_mem(data); break;
	default:
	    fprintf(stderr, "Unknown message\n");
    }
    
    return 0;
}
