/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xil_io.h"
#include "my_rotary_en.h"
#include "sleep.h"
#include "xgpiops.h"
#include "iodefine.h"
#define MASK 0b111111111
#define CHANNEL_PS 2
#define GPIO_DEVICE_ID_PS XPAR_XGPIOPS_0_DEVICE_ID
u32 Output_Pin;
XGpioPs GpioPs;

/*
 * GPIO PS PINs:
 * [7]	M14
 * [6]	L14
 * [3]	N14
 * [2]	N13
 */

int main()
{
	XGpioPs_Config *GPIOConfigPtr;
    int rotations = 0;
    int old_rotations = rotations;
	int Status;
    init_platform();

    GPIOConfigPtr = XGpioPs_LookupConfig(GPIO_DEVICE_ID_PS);
    Status = XGpioPs_CfgInitialize(&GpioPs, GPIOConfigPtr,GPIOConfigPtr->BaseAddr);

    	if (Status != XST_SUCCESS) {
    		printf("status error \n\r");
    		return XST_FAILURE;
    	}
	XGpioPs_SetDirection(&GpioPs, CHANNEL_PS, MASK);
	XGpioPs_SetOutputEnable(&GpioPs, CHANNEL_PS, MASK);
	int value = 0b0001;
	int position = 0;
    while(1){
    	rotations = MY_ROTARY_EN_mReadReg (XPAR_MY_ROTARY_EN_0_S00_AXI_BASEADDR, MY_ROTARY_EN_S00_AXI_SLV_REG3_OFFSET);
    	if(rotations/4 > old_rotations/4){
    		//gaat naar rechts
    		if(position < 3){
        		position++;
    		}

    	}else if(rotations/4 < old_rotations/4){
    		//gaat naar links
    		if(position > 0){
        		position--;
    		}

    	}
    	XGpioPs_Write(&GpioPs,CHANNEL_PS,value<<position);
    	if(old_rotations/4 != rotations/4){
    		printf("%d \n\r", position);
    	}
		old_rotations = rotations;
    }
    cleanup_platform();
    return 0;
}

