#pragma once

#include <string>
#include <al.h>
#include <ogg/ogg.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

class COggStream
{
public:
	COggStream();
	~COggStream();

	void open(std::string path); // obtain a handle to the file
	void release();         // release the file handle
	bool playback();        // play the Ogg stream
	bool playing();         // check if the source is playing
	bool update();          // update the stream if necessary

protected:
	bool stream(ALuint buffer);   // reloads a buffer
	void empty();                 // empties the queue
	void check();                 // checks OpenAL error state
	std::string errorString(int code); // stringify an error code

private:
	FILE*           oggFile;       // file handle
	OggVorbis_File  oggStream;     // stream handle
	vorbis_info*    vorbisInfo;    // some formatting data
	vorbis_comment* vorbisComment; // user comments

	ALuint *buffers; // front and back buffers
	ALuint source;     // audio source
	ALenum format;     // internal format
};
