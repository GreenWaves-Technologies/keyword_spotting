/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */
#include "wavIO.h"

#define WAV_HEADER_LEN	44

static int ReadWAVHeader(char *FileName, header_struct *HeaderInfo)
{
	// TODO use a struct for header
	switch_fs_t fs;
	__FS_INIT(fs);
	switch_file_t File = __OPEN_READ(fs, FileName);

	if (!File) {
		printf("Unable to open file %s\n", FileName);
		return 1;
	}

	unsigned int Err = 0;
	unsigned char *Header = (unsigned char *) __ALLOC_L2(WAV_HEADER_LEN);
	Err |= (Header == 0);
	if ((__READ(File, Header, WAV_HEADER_LEN) != WAV_HEADER_LEN) || Err) return 1;

	HeaderInfo->FileSize      = Header[4]  | (Header[5]<<8)  | (Header[6]<<16)  |	(Header[7]<<24);
	HeaderInfo->format_type   = Header[20] | (Header[21]<<8);
	HeaderInfo->NumChannels   = Header[22] | (Header[23]<<8);
	HeaderInfo->SampleRate    = Header[24] | (Header[25]<<8) | (Header[26]<<16) |	(Header[27]<<24);
	HeaderInfo->byterate      = Header[32] | (Header[33]<<8);
	HeaderInfo->BitsPerSample = Header[34] | (Header[35]<<8);
	HeaderInfo->DataSize      = Header[40] | (Header[41]<<8) | (Header[42]<<16) |	(Header[43]<<24);
	__FREE_L2(Header, WAV_HEADER_LEN);
	__CLOSE(File);
	__FS_DEINIT(fs);
	return Err;
}

static int ReadWavShort(switch_file_t File, short int* OutBuf, unsigned int NumSamples, unsigned int Channels)
{
	unsigned char *data_buf = (unsigned char *) __ALLOC_L2(Channels*sizeof(short int));
	int BytesPerSample = Channels * sizeof(short int);
	if (data_buf==NULL) {
		printf("Error allocating\n");
		return 1;
	}
	int i, ch;
	for (i=0; i<NumSamples; i++){
		int len = __READ(File, data_buf, BytesPerSample);
		if (len != BytesPerSample) return 1;
		int data_in_channel;
		int offset = 0;
		for (ch=0; ch<Channels; ch++){
			data_in_channel = data_buf[offset] | (data_buf[offset+1] << 8);
			OutBuf[i*Channels + ch] = (short int) data_in_channel;
			offset += 2; //Bytes in each channel
		}
	}
	return 0;
}

static int ReadWavChar(switch_file_t File, char* OutBuf, unsigned int NumSamples, unsigned int Channels)
{
	return 1;
}

int ReadWavFromFile(char *FileName, void* OutBuf, unsigned int BufSize, header_struct *HeaderInfo) 
{
	if (ReadWAVHeader(FileName, HeaderInfo)) return 1;
	switch_file_t File = (switch_file_t) 0;
	switch_fs_t fs;
	__FS_INIT(fs);
	File = __OPEN_READ(fs, FileName);
	if (File == 0) {
		printf("Failed to open file, %s\n", FileName); goto Fail;
	}

	int NumSamples = HeaderInfo->DataSize * 8 / (HeaderInfo->NumChannels * HeaderInfo->BitsPerSample);
	int SizeOfEachSample = (HeaderInfo->NumChannels * HeaderInfo->BitsPerSample) / 8;

	__SEEK(File, WAV_HEADER_LEN);

	int SamplesShort;
	if (HeaderInfo->BitsPerSample == 16) SamplesShort = 1;
	else if (HeaderInfo->BitsPerSample == 8) SamplesShort = 0;
	else return 1;

	if (BufSize < HeaderInfo->DataSize){
		printf("Buffer Size too small: %d required, %d given", HeaderInfo->DataSize, BufSize);
		return 1;
	}

	int res;
	if (HeaderInfo->BitsPerSample == 16)
		res = ReadWavShort(File, (short int *) OutBuf, NumSamples, HeaderInfo->NumChannels);
	else if (HeaderInfo->BitsPerSample == 8)
		res = ReadWavChar(File, (char *) OutBuf, NumSamples, HeaderInfo->NumChannels);
	else goto Fail;

	if (res) {
		printf("Input ended unexpectedly or bad format, %s\n", FileName); goto Fail;
	}
	__CLOSE(File);
	__FS_DEINIT(fs);
	PRINTF("\n\nFile: %s, FileSize: %d, NumChannels: %d, SampleRate: %d, BitsPerSample: %d, DataSize: %d, NumSamples: %d\n", \
		    FileName, HeaderInfo->DataSize, HeaderInfo->NumChannels, HeaderInfo->SampleRate, HeaderInfo->BitsPerSample, HeaderInfo->DataSize, NumSamples);

	return 0;
Fail:
	__CLOSE(File);
	__FS_DEINIT(fs);
	printf("Failed to load file %s from flash\n", FileName);
	return 1;

}
