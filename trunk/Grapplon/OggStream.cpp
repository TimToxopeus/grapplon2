#include "OggStream.h"
#include <stdio.h>

#include "LogManager.h"

#include "GameSettings.h" 

COggStream::COggStream()
{
	oggFile = NULL;
	vorbisInfo = NULL;
	vorbisComment = NULL;
	buffers = new ALuint[SETS->BUFFERS];
}

COggStream::~COggStream()
{
	release();
	delete buffers;
}

void COggStream::open(std::string path)
{
	int result;
	FILE *pFile = fopen(path.c_str(), "rb");
	if(!pFile)
		throw std::string("Could not open Ogg file.");

	result = ov_open(pFile, &oggStream, NULL, 0);
	if( result < 0 )
	{
		fclose(pFile);
		throw std::string("Could not open Ogg stream. ") + errorString(result);
	}
	oggFile = pFile;

	vorbisInfo = ov_info(&oggStream, -1);
	vorbisComment = ov_comment(&oggStream, -1);

	if(vorbisInfo->channels == 1)
		format = AL_FORMAT_MONO16;
	else
		format = AL_FORMAT_STEREO16;

	alGenBuffers(SETS->BUFFERS, buffers);
	check();
	alGenSources(1, &source);
	check();

	alSource3f(source, AL_POSITION,        0.0, 0.0, 0.0);
	alSource3f(source, AL_VELOCITY,        0.0, 0.0, 0.0);
	alSource3f(source, AL_DIRECTION,       0.0, 0.0, 0.0);
	alSourcef (source, AL_ROLLOFF_FACTOR,  0.0          );
	alSourcei (source, AL_SOURCE_RELATIVE, AL_TRUE      );
	//alSourcei (source, AL_LOOPING, AL_TRUE );
}

void COggStream::release()
{
	alSourceStop(source);
	empty();

	alDeleteSources(1, &source);
	check();

	alDeleteBuffers(SETS->BUFFERS, buffers);
	check();

	ov_clear(&oggStream);
}

bool COggStream::playback()
{
	if(playing())
		return true;

	for ( int i = 0; i<SETS->BUFFERS; i++ )
		if(!stream(buffers[i]))
			return false;

	alSourceQueueBuffers(source, SETS->BUFFERS, buffers);
	alSourcePlay(source);

	return true;
}

bool COggStream::playing()
{
    ALenum state;
    alGetSourcei(source, AL_SOURCE_STATE, &state);

	return (state == AL_PLAYING);
}

bool COggStream::update()
{
	int processed;
	bool active = true;

	alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);
	while(processed--)
	{
		ALuint buffer = 0;
		alSourceUnqueueBuffers(source, 1, &buffer);
		check();

		active = stream(buffer);
		check();

		alSourceQueueBuffers(source, 1, &buffer);
		check();
	}

	return active;
}

bool COggStream::stream(ALuint buffer)
{
	char *data = new char[SETS->BUFFER_SIZE];
	int  size = 0;
	int  section;
	int  result;

	while(size < SETS->BUFFER_SIZE)
	{
		result = ov_read(&oggStream, data + size, SETS->BUFFER_SIZE - size, 0, 2, 1, & section);

		if(result > 0)
			size += result;
		else if(result < 0)
			throw errorString(result);
		else
			break;
	}

	if(size == 0||result == 0)
	{
		ov_raw_seek(&oggStream, 0);
		return false;
	}

	alBufferData(buffer, format, data, size, vorbisInfo->rate);
	check();

	delete data;

	return true;
}

void COggStream::empty()
{
	int queued;
	alGetSourcei(source, AL_BUFFERS_QUEUED, &queued);

	while(queued--)
	{
		ALuint buffer;
		alSourceUnqueueBuffers(source, 1, &buffer);
		check();
	}
}

void COggStream::check()
{
	int error = alGetError();
	if(error != AL_NO_ERROR)
	{
		CLogManager::Instance()->LogMessage( "OpenAL produced an error: " + errorString( error ) );
		//throw std::string( "OpenAL produced an error: " + errorString( error ) );
	}
}

std::string COggStream::errorString(int code)
{
	switch(code)
	{
	case OV_EREAD:
		return std::string("Read from media.");
	case OV_ENOTVORBIS:
		return std::string("Not Vorbis data.");
	case OV_EVERSION:
		return std::string("Vorbis version mismatch.");
	case OV_EBADHEADER:
		return std::string("Invalid Vorbis header.");
	case OV_EFAULT:
		return std::string("Internal logic fault (bug or heap/stack corruption.");
	case AL_INVALID_VALUE:
		return std::string("Open AL error.");
	default:
		return std::string("Unknown Ogg error.");
	}
}
