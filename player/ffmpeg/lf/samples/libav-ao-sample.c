/**
 * Program copyright blinking blip, 2011
 * Downloaded from 
 * http://blinkingblip.wordpress.com/2011/10/08/decoding-and-playing-an-audio-stream-using-libavcodec-libavformat-and-libao/
 */

#include <stdio.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <ao/ao.h>
 
void die(const char* message) {
    fprintf(stderr, "%s\n", message);
    exit(1);
}
 
int main(int argc, char* argv[]) {
    if (argc < 2) {
        die("Usage: play file");
    }
 
    const char* input_filename = argv[1];
 
    // This call is necessarily done once in your app to initialize
    // libavformat to register all the muxers, demuxers and protocols.
    av_register_all();
 
    // A media container
    AVFormatContext* container = 0;
 
    if (avformat_open_input(&container, input_filename, NULL, NULL) < 0) {
        die("Could not open file");
    }
 
    if (av_find_stream_info(container) < 0) {
        die("Could not find file info");
    }
 
    int stream_id = -1;
 
    // To find the first audio stream. This process may not be necessary
    // if you can gurarantee that the container contains only the desired
    // audio stream
    int i;
    for (i = 0; i < container->nb_streams; i++) {
        if (container->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
            stream_id = i;
            break;
        }
    }
 
    if (stream_id == -1) {
        die("Could not find an audio stream");
    }

    /* 
    // Extract some metadata
    AVDictionary* metadata = container->metadata;
 
    const char* artist = av_dict_get(metadata, "artist", NULL, 0)->value;
    const char* title = av_dict_get(metadata, "title", NULL, 0)->value;
 
    fprintf(stdout, "Playing: %s - %s\n", artist, title);
    */

    // Find the apropriate codec and open it
    AVCodecContext* codec_context = container->streams[stream_id]->codec;
 
    AVCodec* codec = avcodec_find_decoder(codec_context->codec_id);
 
    if (!avcodec_open(codec_context, codec) < 0) {
        die("Could not find open the needed codec");
    }
 
    // To initalize libao for playback
    ao_initialize();
 
    int driver = ao_default_driver_id();
 
    // The format of the decoded PCM samples
    ao_sample_format sample_format;
    sample_format.bits = 16;
    sample_format.channels = 2;
    sample_format.rate = 44100;
    sample_format.byte_format = AO_FMT_NATIVE;
    sample_format.matrix = 0;
 
    ao_device* device = ao_open_live(driver, &sample_format, NULL);
 
    AVPacket packet;
    int buffer_size = AVCODEC_MAX_AUDIO_FRAME_SIZE;
    int8_t buffer[AVCODEC_MAX_AUDIO_FRAME_SIZE];
 
    while (1) {
 
        buffer_size = AVCODEC_MAX_AUDIO_FRAME_SIZE;
 
        // Read one packet into `packet`
        if (av_read_frame(container, &packet) < 0) {
            break;  // End of stream. Done decoding.
        }
 
        // Decodes from `packet` into the buffer
        if (avcodec_decode_audio3(codec_context, (int16_t*)buffer, &buffer_size, &packet) < 1) {
            break;  // Error in decoding
        }
 
        // Send the buffer contents to the audio device
        ao_play(device, (char*)buffer, buffer_size);
    }
 
    av_close_input_file(container);
 
    ao_shutdown();
 
    fprintf(stdout, "Done playing. Exiting...");
 
    return 0;
}
