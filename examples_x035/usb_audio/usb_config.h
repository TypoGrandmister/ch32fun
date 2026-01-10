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



#define Audio_ch 0x02
#define Audio_IN 1
#define audio_sample_rate 0x80,0x3E,0x00 //8Ksmp/s

#ifndef _USB_CONFIG_H
#define _USB_CONFIG_H

#include "funconfig.h"
#include "ch32fun.h"

//genric USB defs
/*limits are 1-8*/
#ifndef Audio_ch
#define Audio_ch 1
#endif
#if (Audio_ch>8)
#define Audio_ch 8
#endif
#if (Audio_ch==0)
#define Audio_ch 1
#endif

#ifndef audio_encode_type
/* 1->PCM, 2->8bit,3->float*/
#define audio_encode_type 0x01
#endif

#ifndef audio_sample_rate
#define audio_sample_rate 0x00,0x80,0x3E//8000
#endif

#ifndef audio_res
#define audio_res 0x10
#endif

#ifndef Audio_IN
#define Audio_IN 0
#else
#if Audio_IN>0
#define Audio_IN 1
#endif
#endif

#ifndef Audio_OUT
#define Audio_OUT 0
#else 
#if (Audio_OUT>0)
#define Audio_OUT 1
#endif
#endif

#define FUSB_CONFIG_EPS       (Audio_IN+Audio_OUT+1) // Include EP0 in this count
#define FUSB_SUPPORTS_SLEEP   0
#define FUSB_CURSED_TURBO_DMA 0 // Hacky, but seems fine, shaves 2.5us off filling 64-byte buffers.
#define FUSB_IO_PROFILE       1
#define FUSB_USE_HPE          FUNCONF_ENABLE_HPE
#define FUSB_5V_OPERATION     FUNCONF_5V_OPERATION

#define STR_MANUFACTURER u"CNLohr"
#define STR_PRODUCT      u"ch32fun ch32x035 test"
#define STR_SERIAL       u"CUSTOMDEVICE000"

#include "usb_defines.h"

/* terminal out Descriptor (ID, INPUT)*/
#define USB_Audio_OUT_Terminal_Desc(id,sink,source)\
	0x09		/*len*/						,\
	0x24		/*CS_INTERFACE*/			,\
	0x03		/*out code*/				,\
	id			/*id of terminal*/			,\
	0x01,sink	/*0x03->speaker,0x01->USB*/	,\
	0			/*associated terminal*/		,\
	source		/*input terminal*/			,\
	0x00		/*string index*/	

/* terminal out Descriptor 
source 1=USB,source 2=mic,
*/
#define USB_Audio_IN_Terminal_Desc(id,source,ch_count,ch_mask)\
	0x0C			/*len*/						,\
	0x24			/*CS_INTERFACE*/			,\
	0x02			/*IN code*/					,\
	id				/*id of terminal*/			,\
	0x01,source		/*0x02->mic,0x01->USB*/		,\
	0				/*associated terminal*/		,\
	ch_count		/*channel count*/			,\
	ch_mask,0x00	/*channel bit mask*/		,\
	0x00			/*string index*/			,\
	0x00			/*string index*/


//Taken from http://www.usbmadesimple.co.uk/ums_ms_desc_dev.htm
static const uint8_t device_descriptor[] = {
	0x12,				//Length
	1,  				//Type (Device)
	0x00, 0x02,			//Spec
	0x01,				//Device Class
	0x0,				//Device Subclass
	0x0,				//Device Protocol  (000 = use config descriptor)
	64,					//Max packet size for EP0 (This has to be 8 because of the USB Low-Speed Standard)
	0x09, 0x12,			//ID Vendor
	0x35, 0xd0,			//ID Product
	0x03, 0x00,			//ID Rev
	1,					//Manufacturer string
	2,					//Product string
	3,					//Serial string
	1,					//Max number of configurations
};


static const uint8_t channel_mask[]={0,0x3,0x7,0x0F,0x1F,0x3F,0x5F,0x7F,0xFF};


/* Configuration Descriptor Set */
static const uint8_t config_descriptor[ ] =
{
	    /* Interface config*/
    0x09,													// bLength
    0x02,													// bDescriptorType
    57+Audio_IN*34+Audio_OUT*34, 0x00,		// wTotalLength
    0x02,													// bNumInterfaces (stream&control)
    0x03,													// bConfigurationValue
    0x01,													// iConfiguration
    0x80,													// bmAttributes: Bus Power
    0x32,													// MaxPower: 100mA


	/* switch to Interface 0 audio class */
    0x09,													// bLength
    0x04,													// bDescriptorType
    0x00,													// intrface number
    0x00,													// bAlternateSetting
	(Audio_IN+Audio_OUT),													// Num Endpoints
    0x01,													// Interface Class
    0x01,													// sub class
    0x00,													// interface Protocol
    0x00,													// string index


	/* Audio Header Descriptor */
	0x08+Audio_IN+Audio_OUT,						// bLength
	0x24,														// CS_INTERFACE
	0x01,														// header subtype
	0x00,0x01,													// audio device class spec
	39+Audio_IN*29+Audio_OUT*29,0x00,				// Total number of bytes returned
	Audio_IN+Audio_OUT,						// terminal count
	#if Audio_IN 										//specify terminal names
		0x01,
	#endif
	#if Audio_OUT
		0x02,	
	#endif


#if Audio_IN
	USB_Audio_IN_Terminal_Desc(0x1,0x2,Audio_ch,channel_mask[Audio_ch-1]),
	USB_Audio_OUT_Terminal_Desc(0x3,01,0x1),
	/*endpoint settings*/
	0x09,			//len
	0x05,			//end point desc			
	0x81,			//out on endpoint 1
	0x00,			//asyc
	0x40,0x00,			//max packet size
	0x01,0x00,0x00,			//spec

#endif


#if Audio_OUT
	USB_Audio_IN_Terminal_Desc(0x2,0x1,Audio_ch,channel_mask[Audio_ch-1]),
	USB_Audio_OUT_Terminal_Desc(0x4,0x03,0x2),
	/*endpoint settings*/
	0x09,					//len
	0x05,					//end point desc			
	0x02,					//in on endpoint 2	
	0x00,					//asyc
	0x40,0x00,		//max packet size
	0x0A,0x00					//interval, 10mS is reccomnded by spec 
	0x01,0x00,					//spec

#endif


/*switch to stream intraface */
0x09,                                                   // bLength                                      0
0x04,                                                   // bDescriptorType                              1
0x01,                                                   // intrface number                              2
0x00,                                                   // bAlternateSetting                            3
0x00,                                                   // Num Endpoints    	 						4
0x01,                                                   // Interface Class 						        5
0x02,                                                   // sub class                                    6
0x00,                                                   // interface Protocol					        7
0x00,


0x7,					//len
0x24,					//CS intraface
0x01,					//header
0x02,					//terminal
0x01,					//line delay
0x01,0x00,				//format, 1->PCM, 2->8bit,3->float


0x7,					//len
0x24,					//CS intraface
0x01,					//header
0x02,					//terminal
0x01,					//line delay
audio_encode_type,0x00,				

/*type I Format Type Descriptor*/
0x0B, 	//len
0x24,									//CS_INTERFACE
0x02, 									//format
0x01,									//type I
Audio_ch,							//channel count
#if (audio_encode_type==1)
4-((Audio_ch%(audio_res/8)))*(audio_res/8),
#else
2,
#endif
audio_res,
1,										//single supported frequncy
audio_sample_rate,


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

	//{0x00002100, config_descriptor + 18, 9 }, // Not sure why, this seems to be useful for Windows + Android.

	{0x00000300, (const uint8_t *)&string0, 4},
	{0x04090301, (const uint8_t *)&string1, sizeof(STR_MANUFACTURER)},
	{0x04090302, (const uint8_t *)&string2, sizeof(STR_PRODUCT)},	
	{0x04090303, (const uint8_t *)&string3, sizeof(STR_SERIAL)}
};
#define DESCRIPTOR_LIST_ENTRIES ((sizeof(descriptor_list))/(sizeof(struct descriptor_list_struct)) )


#endif

