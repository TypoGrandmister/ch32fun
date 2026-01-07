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

//genric USB defs

#define FUSB_CONFIG_EPS       1 // Include EP0 in this count
#define FUSB_SUPPORTS_SLEEP   0
#define FUSB_CURSED_TURBO_DMA 0 // Hacky, but seems fine, shaves 2.5us off filling 64-byte buffers.
#define FUSB_IO_PROFILE       1
#define FUSB_USE_HPE          FUNCONF_ENABLE_HPE
#define FUSB_5V_OPERATION     FUNCONF_5V_OPERATION

#define STR_MANUFACTURER u"CNLohr"
#define STR_PRODUCT      u"ch32fun ch32x035 test"
#define STR_SERIAL       u"CUSTOMDEVICE000"

#include "usb_defines.h"





#ifndef Audio_OUT_ch
#define Audio_OUT_ch 0
#endif

#ifndef Audio_IN_ch
#define Audio_IN_ch 0
#endif

/* terminal out Descriptor (ID, INPUT)*/
#define USB_Audio_OUT_Terminal_Desc(id,source)\
	0x09		/*len*/						,\
	0x24		/*CS_INTERFACE*/			,\
	0x03		/*out code*/				,\
	id			/*id of terminal*/			,\
	0x01,0x03	/*0x0301->speaker*/			,\
	0			/*associated terminal*/		,\
	source		/*input terminal*/			,\
	0x00		/*string index*/	

/* terminal out Descriptor 
source 1=USB,source 2=mic,
*/
#define USB_Audio_IN_Terminal_Desc(id,source,ch_count,ch_mask)\
	0x0C		/*len*/						,\
	0x24		/*CS_INTERFACE*/			,\
	source		/*out code*/				,\
	id			/*id of terminal*/			,\
	0x01,source	/*terminal type*/			,\
	0			/*associated terminal*/		,\
	ch_count	/*channel count*/			,\
	ch_mask		/*channel bit mask*/		,\
	0x00		/*string index*/			,\
	0x00		/*string index*/

/*max is 8*/
static uint8_t channel_mask(int count){
	int mask=0;
	if (count>7){
		count=7;
	}
	for (int i=0;i<count;i++){
		mask=mask|(1<<i);
	}
	return(mask)
}
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
    0x00,                                                   // intrface number                              2
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
    0x01,0x00,                                              // audio device class spec                      3,4                 
    0x19,0x00,                                              // Total number of bytes returned               5,6                 
    (Audio_IN_ch>0)+(Audio_OUT_ch>0)*2,
#if(Audio_IN_ch>0)
	0x01,
	0x02,
#endif
#if(Audio_OUT_ch>0)
	0x03,
	0x04,
#endif
#if(Audio_IN_ch>0)
	0x01,
	0x02,
#endif
#if(Audio_OUT_ch>0)
		USB_Audio_OUT_Terminal_Desc(0x1,0x2),
#endif

	USB_Audio_IN_Terminal_Desc(0x2,0x1,0x2,0x3),
};


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

