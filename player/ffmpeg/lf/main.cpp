

#include <cstdio>

extern "C" {
#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavformat/avformat.h>
}

using namespace std;


void show(const char *fname) {
    AVFormatContext *fmt_ctx = NULL;

    /* open input file, and allocate format context */
    if (avformat_open_input(&fmt_ctx, fname, NULL, NULL) < 0) {
        fprintf(stderr, "ERROR: cannot open media file: '%s'\n", fname);
        exit(1);
    }

    /* dump input information to stderr */
    av_dump_format(fmt_ctx, 0, fname, 0);

    int             audio_stream_idx = -1;
    AVCodecContext *audio_dec_ctx    = NULL;
    AVStream       *audio_stream     = NULL;

    if (open_codec_context(&audio_stream_idx, fmt_ctx, AVMEDIA_TYPE_AUDIO) >= 0) {
        audio_stream = fmt_ctx->streams[audio_stream_idx];
        audio_dec_ctx = audio_stream->codec;
    }

}

int main(int argc, char **argv) {
    /* register all formats and codecs */
    av_register_all();

    if (argc > 1)
        for(auto c = 1; c < argc; ++c)

            show(argv[c]);
    return 0;
}
