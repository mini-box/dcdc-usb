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

#include "dcdc-usb.h"

#define P(t, v...) fprintf(stderr, t "\n", ##v)
static int bytes2int(unsigned char c1, unsigned char c2)
{
    int i = c1;
    i = i << 8;
    i = i | c2;
    
    return i;
}

static int byte2bits(unsigned char c)
{
    int i,n = 0;
    for (i = 0; i < 8; i++)
    {
	n = n * 10;
	if ((c >> i) & 1)
	    n = n + 1;
    }
    return n;
}

static double byte2vout(unsigned char c)
{
    double rpot = ((double) c) * CT_RP / (double)257 + CT_RW;
    double voltage = (double)80 * ((double) 1 + CT_R1/(rpot + CT_R2));
    voltage = floor(voltage);
    return voltage/100;
}

void dcdc_parse_values(unsigned char *data)
{
    int mode, state, status;
    float ignition_voltage, input_voltage, output_voltage;
    
    mode = data[1];
    state = data[2];
    input_voltage = (float) data[3] * 0.1558f;
    ignition_voltage = (float) data[4] * 0.1558f;
    output_voltage = (float) data[5] * 0.1170f;
    status = data[6];
    switch(mode & 0x03)
    {
	case 0: P("mode: 0 (dumb)"); break;
	case 1: P("mode: 1 (automotive)"); break;
	case 2: P("mode: 2 (script)"); break;
	case 3: P("mode: 3 (ups)");break;
    }
    P("time config: %d", (mode >> 5) & 0x07);
    P("voltage config: %d", (mode >> 2) & 0x07);
    P("state: %d", state);
    P("input voltage: %.2f", input_voltage);
    P("ignition voltage: %.2f", ignition_voltage);
    P("output voltage: %2f", output_voltage);
    P("power switch: %s", ((status & 0x04) ? "On":"Off"));
    P("output enable: %s", ((status & 0x08) ? "On":"Off"));
    P("aux vin enable %s", ((status & 0x10) ? "On":"Off"));
    P("status flags 1: %d", byte2bits(data[6]));
    P("status flags 2: %d", byte2bits(data[7]));
    P("voltage flags: %d", byte2bits(data[8]));
    P("timer flags: %d", byte2bits(data[9]));
    P("flash pointer: %d", data[10]);
    P("timer wait: %d", bytes2int(data[11], data[12]));
    P("timer vout: %d", bytes2int(data[13], data[14]));
    P("timer vaux: %d", bytes2int(data[15], data[16]));
    P("timer pw switch: %d", bytes2int(data[17], data[18]));
    P("timer off delay: %d", bytes2int(data[19], data[20]));
    P("timer hard off: %d", bytes2int(data[21], data[22]));
    P("version: %d.%d", ((data[23] >> 5) & 0x07), (data[23] & 0x1F));
    
}

void dcdc_parse_cmd(unsigned char *data)
{
    if (data[1] == 0)
    {
	if (data[2] == CMD_READ_REGULATOR_STEP)
	{
	    P("regulator step: %d", data[3]);
	}
	else
	{
	    P("output voltage: %.2f", byte2vout(data[3]));
	}
    }
    else 
    {
	if (data[2] == CMD_READ_REGULATOR_STEP)
	{
	    P("regulator step not defined");
	}
    }
}

void dcdc_parse_internal_msg(unsigned char *data)
{
    P("Parsing INTERNAL MESSAGE: Not implemented");
}

void dcdc_parse_mem(unsigned char *data)
{
    P("Parsing MEM READ IN: Not implemented");
}