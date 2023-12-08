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
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include <usb.h>

#include "dcdc-usb.h"

void showhelp(char *prgname)
{

    printf ("Usage: %s [OPTION]\n", prgname);
    printf ("Options:.\n");
    printf (" -a \t show all device settings\n");
    printf (" -h \t show help message\n");
    printf (" -v \t set voltage out value (float)\n");
    printf (" -o \t continously output ignition voltage in 1 second intervals\n");
}

int main(int argc, char **argv)
{
    struct usb_dev_handle *h;
    unsigned char data[MAX_TRANSFER_SIZE];
    int ret;
    char *s;
    int arg = 0, showall = 0, setvoltage = 0, monitor = 0;
    double vout = 5.0;
    
    while ( ++arg < argc ) 
    {
	s = argv[arg];
	if (strncmp(s, "-a", 2) == 0)
	    showall = 1;
	if (strncmp(s, "-h", 2) == 0)
	{
	    showhelp(argv[0]);
	    return 0;
	}
	if (strncmp(s, "-v", 2) == 0)
	    if (arg + 1 < argc)
	    {
		arg++;
		vout = strtod(argv[arg], NULL);
		if (vout != 0)
		    setvoltage = 1;
	    }
	if (strncmp(s, "-o", 2) == 0)
	{
		monitor = 1;
	}
    }
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
    
    if (showall)
    {
	if ((ret = dcdc_get_status(h, data, MAX_TRANSFER_SIZE)) <= 0)
	{
	    fprintf(stderr, "Failed to get status from device\n");
	    return 3;
	}
	dcdc_parse_data(data, ret);
    }

    if (monitor)
    {
	while (true)
	{
        	if ((ret = dcdc_get_status(h, data, MAX_TRANSFER_SIZE)) <= 0)
        	{
            		fprintf(stderr, "Failed to get status from device\n");
            		return 3;
        	}
	        dcdc_parse_ignition(data);
		sleep(1);
	}
    }
    
    if (setvoltage)
    {
	fprintf(stderr, "setting output voltage to: %.2f\n", vout);
	dcdc_set_vout(h, vout);
	
    }
    
    if ((ret = dcdc_get_vout(h, data, MAX_TRANSFER_SIZE)) <= 0)
    {
	    fprintf(stderr, "Failed to get voltage from device\n");
	    return 3;
    }
    dcdc_parse_data(data, ret);
    
    return 0;
}
