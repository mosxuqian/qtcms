#include "StdAfx.h"

static short ALawDecompressTable[] =
{
    (short)0xEA80, (short)0xEB80, (short)0xE880, (short)0xE980, (short)0xEE80, (short)0xEF80, (short)0xEC80, (short)0xED80,
    (short)0xE280, (short)0xE380, (short)0xE080, (short)0xE180, (short)0xE680, (short)0xE780, (short)0xE480, (short)0xE580,
    (short)0xF540, (short)0xF5C0, (short)0xF440, (short)0xF4C0, (short)0xF740, (short)0xF7C0, (short)0xF640, (short)0xF6C0,
    (short)0xF140, (short)0xF1C0, (short)0xF040, (short)0xF0C0, (short)0xF340, (short)0xF3C0, (short)0xF240, (short)0xF2C0,
    (short)0xAA00, (short)0xAE00, (short)0xA200, (short)0xA600, (short)0xBA00, (short)0xBE00, (short)0xB200, (short)0xB600,
    (short)0x8A00, (short)0x8E00, (short)0x8200, (short)0x8600, (short)0x9A00, (short)0x9E00, (short)0x9200, (short)0x9600,
    (short)0xD500, (short)0xD700, (short)0xD100, (short)0xD300, (short)0xDD00, (short)0xDF00, (short)0xD900, (short)0xDB00,
    (short)0xC500, (short)0xC700, (short)0xC100, (short)0xC300, (short)0xCD00, (short)0xCF00, (short)0xC900, (short)0xCB00,
    (short)0xFEA8, (short)0xFEB8, (short)0xFE88, (short)0xFE98, (short)0xFEE8, (short)0xFEF8, (short)0xFEC8, (short)0xFED8,
    (short)0xFE28, (short)0xFE38, (short)0xFE08, (short)0xFE18, (short)0xFE68, (short)0xFE78, (short)0xFE48, (short)0xFE58,
    (short)0xFFA8, (short)0xFFB8, (short)0xFF88, (short)0xFF98, (short)0xFFE8, (short)0xFFF8, (short)0xFFC8, (short)0xFFD8,
    (short)0xFF28, (short)0xFF38, (short)0xFF08, (short)0xFF18, (short)0xFF68, (short)0xFF78, (short)0xFF48, (short)0xFF58,
    (short)0xFAA0, (short)0xFAE0, (short)0xFA20, (short)0xFA60, (short)0xFBA0, (short)0xFBE0, (short)0xFB20, (short)0xFB60,
    (short)0xF8A0, (short)0xF8E0, (short)0xF820, (short)0xF860, (short)0xF9A0, (short)0xF9E0, (short)0xF920, (short)0xF960,
    (short)0xFD50, (short)0xFD70, (short)0xFD10, (short)0xFD30, (short)0xFDD0, (short)0xFDF0, (short)0xFD90, (short)0xFDB0,
    (short)0xFC50, (short)0xFC70, (short)0xFC10, (short)0xFC30, (short)0xFCD0, (short)0xFCF0, (short)0xFC90, (short)0xFCB0,
    (short)0x1580, (short)0x1480, (short)0x1780, (short)0x1680, (short)0x1180, (short)0x1080, (short)0x1380, (short)0x1280,
    (short)0x1D80, (short)0x1C80, (short)0x1F80, (short)0x1E80, (short)0x1980, (short)0x1880, (short)0x1B80, (short)0x1A80,
    (short)0x0AC0, (short)0x0A40, (short)0x0BC0, (short)0x0B40, (short)0x08C0, (short)0x0840, (short)0x09C0, (short)0x0940,
    (short)0x0EC0, (short)0x0E40, (short)0x0FC0, (short)0x0F40, (short)0x0CC0, (short)0x0C40, (short)0x0DC0, (short)0x0D40,
    (short)0x5600, (short)0x5200, (short)0x5E00, (short)0x5A00, (short)0x4600, (short)0x4200, (short)0x4E00, (short)0x4A00,
    (short)0x7600, (short)0x7200, (short)0x7E00, (short)0x7A00, (short)0x6600, (short)0x6200, (short)0x6E00, (short)0x6A00,
    (short)0x2B00, (short)0x2900, (short)0x2F00, (short)0x2D00, (short)0x2300, (short)0x2100, (short)0x2700, (short)0x2500,
    (short)0x3B00, (short)0x3900, (short)0x3F00, (short)0x3D00, (short)0x3300, (short)0x3100, (short)0x3700, (short)0x3500,
    (short)0x0158, (short)0x0148, (short)0x0178, (short)0x0168, (short)0x0118, (short)0x0108, (short)0x0138, (short)0x0128,
    (short)0x01D8, (short)0x01C8, (short)0x01F8, (short)0x01E8, (short)0x0198, (short)0x0188, (short)0x01B8, (short)0x01A8,
    (short)0x0058, (short)0x0048, (short)0x0078, (short)0x0068, (short)0x0018, (short)0x0008, (short)0x0038, (short)0x0028,
    (short)0x00D8, (short)0x00C8, (short)0x00F8, (short)0x00E8, (short)0x0098, (short)0x0088, (short)0x00B8, (short)0x00A8,
    (short)0x0560, (short)0x0520, (short)0x05E0, (short)0x05A0, (short)0x0460, (short)0x0420, (short)0x04E0, (short)0x04A0,
    (short)0x0760, (short)0x0720, (short)0x07E0, (short)0x07A0, (short)0x0660, (short)0x0620, (short)0x06E0, (short)0x06A0,
    (short)0x02B0, (short)0x0290, (short)0x02F0, (short)0x02D0, (short)0x0230, (short)0x0210, (short)0x0270, (short)0x0250,
    (short)0x03B0, (short)0x0390, (short)0x03F0, (short)0x03D0, (short)0x0330, (short)0x0310, (short)0x0370, (short)0x0350,
};

short AlawToLinearSample(unsigned char sample)
{
    return ALawDecompressTable[sample];
}

int g711a_decode(short amp[], const unsigned char g711a_data[], int g711a_bytes)
{
	int i;
	int samples;
	unsigned char code;
	int sl;
	
	for (samples = i = 0;;)
	{
		if (i >= g711a_bytes)
			break;
		code = g711a_data[i++];
		
		sl = AlawToLinearSample(code);
		
		amp[samples++] = (short) sl;
	}
	return samples*2;
}


