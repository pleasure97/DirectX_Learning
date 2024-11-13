#include "soundclass.h"

SoundClass::SoundClass()
{
	m_secondaryBuffer = 0; 
}

SoundClass::SoundClass(const SoundClass&)
{
}

SoundClass::~SoundClass()
{
}

bool SoundClass::LoadTrack(IDirectSound8* DirectSound, char* filename, long volume)
{
	bool result; 

	// Load the wave file for the sound.
	result = LoadStereoWaveFile(DirectSound, filename, volume);
	if (!result)
	{
		return false; 
	}

	return true; 
}

void SoundClass::ReleaseTrack()
{
	// Release the wave file buffers.
	ReleaseWaveFile(); 

	return; 
}

bool SoundClass::PlayTrack()
{
	HRESULT result;

	// Set positon at the beginning of the sound buffer.
	result = m_secondaryBuffer->SetCurrentPosition(0); 
	if (FAILED(result))
	{
		return false;
	}
	
	// If looping is on, then play the contents of the secondary buffer in a loop, otherwise, just play it once.
	result = m_secondaryBuffer->Play(0, 0, 0); 
	if (FAILED(result))
	{
		return false;
	}

	return true; 
}

bool SoundClass::StopTrack()
{
	HRESULT result;

	// Stop the sound from playing.
	result = m_secondaryBuffer->Stop(); 
	if (FAILED(result))
	{
		return false;
	}

	return true; 
}

bool SoundClass::LoadStereoWaveFile(IDirectSound8* DirectSound, char* filename, long volume)
{
	FILE* filePtr; 
	RiffWaveHeaderType riffWaveFileHeader; 
	SubChunkHeaderType subChunkHeader;
	FmtType fmtData; 
	WAVEFORMATEX waveFormat; 
	DSBUFFERDESC bufferDesc; 
	HRESULT result;
	IDirectSoundBuffer* tempBuffer;
	unsigned char* waveData; 
	unsigned char* bufferPtr; 
	unsigned long long count;
	unsigned long dataSize; 
	unsigned long bufferSize; 
	long seekSize; 
	int error;
	bool foundFormat; 
	bool foundData; 

	// Open the wave file for reading in binary.
	error = fopen_s(&filePtr, filename, "rb"); 
	if (error != 0)
	{
		return false;
	}

	// Read in the riff wave header.
	count = fread(&riffWaveFileHeader, sizeof(riffWaveFileHeader), 1, filePtr); 
	if (count != 1)
	{
		return false; 
	}

	// Check that the chunk ID is in the RIFF format.
	if ((riffWaveFileHeader.chunkId[0] != 'R') || (riffWaveFileHeader.chunkId[1] != 'I') || (riffWaveFileHeader.chunkId[2] != 'F') || (riffWaveFileHeader.chunkId[3] != 'F'))
	{
		return false;
	}

	// Check that the file format is the WAVE format.
	if ((riffWaveFileHeader.format[0] != 'W') || (riffWaveFileHeader.format[1] != 'A') || (riffWaveFileHeader.format[2] != 'V') || (riffWaveFileHeader.format[3] != 'E'))
	{
		return false;
	}

	// Read in the sub chunk headers until you find the format chunk.
	foundFormat = false;
	while (foundFormat == false)
	{
		// Read in the sub chunk header.
		count = fread(&subChunkHeader, sizeof(subChunkHeader), 1, filePtr); 
		if (count != 1)
		{
			return false;
		}

		// Determine if it is the fmt header. 
		// If not, then move to the end of the chunk and read in the next one. 
		if ((subChunkHeader.subChunkId[0] == 'f') && (subChunkHeader.subChunkId[1] == 'm') && (subChunkHeader.subChunkId[2] == 't') && (subChunkHeader.subChunkId[3] == ' '))
		{
			foundFormat = true;
		}
		else
		{
			fseek(filePtr, subChunkHeader.subChunkSize, SEEK_CUR); 
		}
	}

	// Read in the format data.
	count = fread(&fmtData, sizeof(fmtData), 1, filePtr); 
	if (count != 1)
	{
		return false; 
	}

	// Check that the audio format is WAVE_FORMAT_PCM.
	if (fmtData.audioFormat != WAVE_FORMAT_PCM)
	{
		return false;
	}

	// Check that the wave file was recorded in stereo format.
	if (fmtData.numChannels != 2)
	{
		return false;
	}

	// Check that the wave file was recorded at a sampler rate of 44.1KHZ.
	if (fmtData.sampleRate != 44100)
	{
		return false;
	}

	// Ensure that the wave file was recorded in 16 bit format.
	if (fmtData.bitsPerSample != 16)
	{
		return false;
	}

	// Seek up to the next sub chunk.
	seekSize = subChunkHeader.subChunkSize - 16; 
	fseek(filePtr, seekSize, SEEK_CUR); 

	// Read in the sub chunk headers until you find the data chunk.
	foundData = false;
	while (foundData == false)
	{
		// Read the sub chunk header.
		count = fread(&subChunkHeader, sizeof(subChunkHeader), 1, filePtr); 
		if (count != 1)
		{
			return false; 
		}

		// Determine if it is the data header. 
		// If not, then move to the end of the chunk and read in the next one.
		if ((subChunkHeader.subChunkId[0] == 'd') && (subChunkHeader.subChunkId[1] == 'a') && (subChunkHeader.subChunkId[2] == 't') && (subChunkHeader.subChunkId[3] == 'a'))
		{
			foundData = true;
		}
		else
		{
			fseek(filePtr, subChunkHeader.subChunkSize, SEEK_CUR);
		}
	}

	// Store the size of the data chunk.
	dataSize = subChunkHeader.subChunkSize; 

	// Set the wave format of secondary buffer that this wave file will be loaded onto.
	waveFormat.wFormatTag = WAVE_FORMAT_PCM; 
	waveFormat.nSamplesPerSec = fmtData.sampleRate;
	waveFormat.wBitsPerSample = fmtData.bitsPerSample; 
	waveFormat.nChannels = fmtData.numChannels; 
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels; 
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign; 
	waveFormat.cbSize = 0;

	// Set the buffer description of the secondary sound buffer that the wave file will be loaded onto.
	bufferDesc.dwSize = sizeof(DSBUFFERDESC); 
	bufferDesc.dwBufferBytes = dataSize; 
	bufferDesc.dwReserved = 0; 
	bufferDesc.lpwfxFormat = &waveFormat; 
	bufferDesc.guid3DAlgorithm = GUID_NULL;
	bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME;

	// Create a temporary sound buffer with the specific buffer settings.
	result = DirectSound->CreateSoundBuffer(&bufferDesc, &tempBuffer, NULL); 
	if (FAILED(result))
	{
		return false;
	}

	// Test the buffer format against the direct sound 8 interface and create the secondary buffer.
	result = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&m_secondaryBuffer); 
	if (FAILED(result))
	{
		return false;
	}

	// Release the temporary buffer.
	tempBuffer->Release();
	tempBuffer = 0; 

	// Create a temporary buffer to hold the wave file data.
	waveData = new unsigned char[dataSize]; 

	// Read in the wave file data into the newly created buffer.
	count = fread(waveData, 1, dataSize, filePtr); 
	if (count != dataSize)
	{
		return false;
	}

	// Close the file once done reading.
	error = fclose(filePtr); 
	if (error != 0)
	{
		return false;
	}

	// Lock the secondary buffer to write wave data into it.
	result = m_secondaryBuffer->Lock(0, dataSize, (void**)&bufferPtr, (DWORD*)&bufferSize, NULL, 0, 0);
	if (FAILED(result))
	{
		return false;
	}

	// Copy the wave data into the buffer.
	memcpy(bufferPtr, waveData, dataSize);

	// Unlock the secondary buffer after the data has been written to it.
	result = m_secondaryBuffer->Unlock((void*)bufferPtr, bufferSize, NULL, 0);
	if (FAILED(result))
	{
		return false;
	}

	// Release the wave data since it was copied into the secondary buffer.
	delete[] waveData;
	waveData = 0;

	// Set volume of the buffer.
	result = m_secondaryBuffer->SetVolume(volume);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void SoundClass::ReleaseWaveFile()
{
	// Release the secondary sound buffer.
	if (m_secondaryBuffer)
	{
		m_secondaryBuffer->Release(); 
		m_secondaryBuffer = 0; 
	}

	return;
}