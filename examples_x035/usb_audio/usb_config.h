/* 

**********************
**********************
**********************
**********************
this code is mainly based on the usb HID exmaple
sources :
https://www.usb.org/sites/default/files/audio10.pdf
https://www.usb.org/sites/default/files/termt10.pdf
*/

#ifndef _USB_CONFIG_H
#define _USB_CONFIG_H

#include "funconfig.h"
#include "ch32fun.h"
//needs to go over this!
#define FUSB_CONFIG_EPS       4 // Include EP0 in this count
#define FUSB_SUPPORTS_SLEEP   0
#define FUSB_CURSED_TURBO_DMA 0 // Hacky, but seems fine, shaves 2.5us off filling 64-byte buffers.
#define FUSB_IO_PROFILE       1
#define FUSB_USE_HPE          FUNCONF_ENABLE_HPE
#define FUSB_5V_OPERATION     FUNCONF_5V_OPERATION

#include "usb_defines.h"

//Taken from http://www.usbmadesimple.co.uk/ums_ms_desc_dev.htm
static const uint8_t device_descriptor[] = {
	0x12, //Length
	1,  //Type (Device)
	0x00, 0x02, //Spec
	0x00, //Device Class
	0x0, //Device Subclass
	0x0, //Device Protocol  (000 = use config descriptor)
	64, //Max packet size for EP0 (This has to be 8 because of the USB Low-Speed Standard)
	0x09, 0x12, //ID Vendor
	0x35, 0xd0, //ID Product
	0x03, 0x00, //ID Rev
	1, //Manufacturer string
	2, //Product string
	3, //Serial string
	1, //Max number of configurations
};



/* Configuration Descriptor Set */
static const uint8_t config_descriptor[ ] =
{
	    /* Configuration Descriptor */																		//offset tracking
    0x09,                                                   // bLength 										0
    0x02,                                                   // bDescriptorType								1
    0x2B, 0x00,                                             // wTotalLength									2,3
    0x01,                                                   // bNumInterfaces (1)							4
    0x01,                                                   // bConfigurationValue							5
    0x01,                                                   // iConfiguration								6
    0x80,                                                   // bmAttributes: Bus Powered					7
    0x32,                                                   // MaxPower: 100mA								8
    /* Configuration Descriptor */                                                                          //offset tracking
    0x09,                                                   // bLength                                      0
    0x04,                                                   // bDescriptorType                              1
    0x01,                                                   // intrface number                              2
    0x00,                                                   // bAlternateSetting                            3
	0x00,                                                   // Num Endpoints    	 						4
    0x01,                                                   // Interface Class 						        5
    0x01,                                                   // sub class                                    6
    0x00,                                                   // interface Protocol					        7
    0x00,                                                   // intraface index			                    8
    /* Audio Header Descriptor */                                                                   	    //offset tracking
    0x09,                                                   // bLength                                      0                   
    0x24,                                                   // CS_INTERFACE                                 1                   
    0x01,                                                   // header subtype                               2                   
    0x01,0x01,                                              // audio device class spec                      3,4                 
    0x19,0x00,                                              // Total number of bytes returned               5,6                 
    0x01,                                                   // number or streams     	                    7                   
    0x01,                                                   // stream ID                                    7                   

    /* terminal 1 (out) Descriptor */                                                                //offset tracking
    0x09,                                                   // Length                              0                           
    0x24,                                                   // CS_INTERFACE                         1                           
    0x03,                                                   // OUTPUT_TERMINAL                      2                           
    0x01,                                                   // Terminal Id                          3                           
    0x01,0x01,             	                                // Terminal type (undefined)            4,5                         
    0x00,                                                   //Input Terminal this Output Terminal is associated           6                           
    0x01,                                                   // Output Terminal associated           7                           
    0x00,                                              // string index                         8,9                         

	//terminal 1 settings 
	0x09,		//len	   																					//0	
	0x24,		//CS int																					//1
	0x04,		//INTERFACE 																				//2
	0x01,		//termianl																			//3
	0x01,		//delay																		//4
	0x00,0x01		//FormatTag																			//5,6
};

#define STR_MANUFACTURER u"CNLohr"
#define STR_PRODUCT      u"ch32fun ch32x035 test"
#define STR_SERIAL       u"CUSTOMDEVICE000"

struct usb_string_descriptor_struct {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint16_t wString[];
};
const static struct usb_string_descriptor_struct string0 __attribute__((section(".rodata"))) = {
	4,
	3,
	{0x0409}
};
const static struct usb_string_descriptor_struct string1 __attribute__((section(".rodata")))  = {
	sizeof(STR_MANUFACTURER),
	3,
	STR_MANUFACTURER
};
const static struct usb_string_descriptor_struct string2 __attribute__((section(".rodata")))  = {
	sizeof(STR_PRODUCT),
	3,
	STR_PRODUCT
};
const static struct usb_string_descriptor_struct string3 __attribute__((section(".rodata")))  = {
	sizeof(STR_SERIAL),
	3,
	STR_SERIAL
};

// This table defines which descriptor data is sent for each specific
// request from the host (in wValue and wIndex).
const static struct descriptor_list_struct {
	uint32_t	lIndexValue;
	const uint8_t	*addr;
	uint8_t		length;
} descriptor_list[] = {
	{0x00000100, device_descriptor, sizeof(device_descriptor)},
	{0x00000200, config_descriptor, sizeof(config_descriptor)},
	//{0x00000200, audio_header, sizeof(audio_header)},
	//{0x00002400, CS_header, sizeof(CS_header)},
	// interface number // 2200 for hid descriptors.

	//{0x00002100, config_descriptor + 18, 9 }, // Not sure why, this seems to be useful for Windows + Android.

	{0x00000300, (const uint8_t *)&string0, 4},
	{0x04090301, (const uint8_t *)&string1, sizeof(STR_MANUFACTURER)},
	{0x04090302, (const uint8_t *)&string2, sizeof(STR_PRODUCT)},	
	{0x04090303, (const uint8_t *)&string3, sizeof(STR_SERIAL)}
};
#define DESCRIPTOR_LIST_ENTRIES ((sizeof(descriptor_list))/(sizeof(struct descriptor_list_struct)) )


#endif

