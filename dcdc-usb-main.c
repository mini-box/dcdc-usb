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