//Generated from WEBsources/configDone.html
#ifndef WEBconfigDone_html_H
#define WEBconfigDone_html_H

unsigned char configDone_html[] = {0x1f,0x8b,0x08,0x00,0x4d,0xc6,0x18,0x67,0x02,0xff,0x5d,0x91,0xbb,0x6e,0xc3,0x30,0x0c,0x45,0x77,0x7f,0x05,0xab,0xb9,0x89,0x9a,0x5d,0xf2,0x50,0x27,0x40,0x87,0x06,0xed,0xe0,0xa5,0xa3,0x22,0x31,0x11,0x11,0x59,0x32,0x2c,0xd6,0x89,0xff,0xbe,0x7e,0x15,0x7d,0x70,0xba,0x22,0xaf,0x0e,0x78,0x41,0xf5,0xb0,0x7f,0xab,0xea,0x8f,0xf7,0x03,0xbc,0xd4,0xc7,0xd7,0xb2,0x50,0x9e,0x9b,0x50,0x16,0x30,0xd6,0xa8,0xd1,0xb8,0x45,0xab,0x06,0xd9,0x40,0x34,0x0d,0x6a,0xd1,0x13,0xde,0xda,0xd4,0xb1,0x00,0x9b,0x22,0x63,0x64,0x2d,0x6e,0xe4,0xd8,0x6b,0x87,0x3d,0x59,0xdc,0xcc,0x8f,0x47,0xa0,0x48,0x4c,0x26,0x6c,0xb2,0x35,0x01,0xf5,0x6e,0xfb,0x24,0x40,0xae,0xb0,0x40,0xf1,0x0a,0x1d,0x06,0x2d,0x32,0x0f,0x01,0xb3,0x47,0x1c,0x69,0x3c,0xb4,0x23,0x9d,0xf1,0xce,0xd2,0xe6,0x2c,0xc0,0x77,0x78,0xd6,0xa2,0xa2,0x88,0x47,0xc3,0x1d,0xdd,0xb7,0x73,0xb7,0x41,0x47,0x46,0x0b,0x13,0xc2,0xcc,0x53,0x72,0x59,0xb2,0x50,0xa7,0xe4,0x86,0x95,0xef,0x77,0x8b,0x98,0xaa,0x4a,0xf1,0x4c,0x97,0xcf,0xce,0x30,0xa5,0x08,0xfb,0x14,0x71,0xf1,0xc8,0x6f,0x93,0x72,0xd4,0x03,0x39,0x2d,0x26,0xc0,0x73,0x48,0xf6,0x2a,0x7e,0x7e,0xcf,0x43,0x1b,0x4c,0xce,0x5a,0xd8,0x99,0x74,0xfa,0xe7,0x98,0x5d,0x6d,0x09,0xb5,0x47,0xc8,0x43,0x66,0x6c,0x80,0xf2,0x18,0x2e,0xb3,0xe9,0x98,0xe2,0xe5,0xaf,0x51,0xb6,0xbf,0xd8,0x72,0x84,0xaf,0x3b,0x2c,0x52,0xc9,0x25,0xc4,0x94,0x6a,0x3a,0xc3,0x17,0x85,0x6a,0x5e,0x5b,0x9d,0x01,0x00,0x00};
unsigned int configDone_html_len =  269 ;



#define DEF_HANDLE_configDone_html  server->on("/configDone.html", HTTP_GET, std::bind (&WebInterface::handleFile, this, std::placeholders::_1,"text/html;charset=UTF-8",configDone_html,configDone_html_len));
#endif
