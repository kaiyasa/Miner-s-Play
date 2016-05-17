
#include <stdio.h>

#include <alsa/asoundlib.h>


/* Handle for the PCM device */ 
snd_pcm_t *pcm_handle;          

/* Playback stream */
snd_pcm_stream_t stream = SND_PCM_STREAM_PLAYBACK;

/* Periodsize (bytes) */
snd_pcm_uframes_t periodsize = 8192;

/* Number of periods */
int periods = 2;

int num_frames = periodsize >> 2;
  

/*
   This structure contains information about
   the hardware and can be used to specify the
   configuration to be used for the PCM stream.
*/ 
snd_pcm_hw_params_t *hwparams;
  

/*
   Name of the PCM device, like plughw:0,0
   The first number is the number of the soundcard,
   the second number is the number of the device.
*/
char *pcm_name;
  
int init() {
    int err;

    /* Init pcm_name. Of course, later you
       will make this configurable ;-)     */
    pcm_name = strdup("default");
  

    /* Allocate the snd_pcm_hw_params_t structure on the stack. */
    snd_pcm_hw_params_alloca(&hwparams);
  

    /* Open PCM. The last parameter of this function is the mode.
       If this is set to 0, the standard mode is used. Possible
       other values are SND_PCM_NONBLOCK and SND_PCM_ASYNC.
       If SND_PCM_NONBLOCK is used, read / write access to the
       PCM device will return immediately. If SND_PCM_ASYNC is
       specified, SIGIO will be emitted whenever a period has
       been completely processed by the soundcard.
    */
    if (snd_pcm_open(&pcm_handle, pcm_name, stream, 0) < 0) {
      fprintf(stderr, "Error opening PCM device %s\n", pcm_name);
      return(-1);
    }

    /* Init hwparams with full configuration space */
    if (snd_pcm_hw_params_any(pcm_handle, hwparams) < 0) {
      fprintf(stderr, "Can not configure this PCM device.\n");
      return(-1);
    }

    unsigned int rate = 44100; /* Sample rate */
    unsigned int exact_rate;   /* Sample rate returned by */
                      /* snd_pcm_hw_params_set_rate_near */ 
    int dir;          /* exact_rate == rate --> dir = 0 */
                      /* exact_rate < rate  --> dir = -1 */
                      /* exact_rate > rate  --> dir = 1 */

  
    /* Set access type. This can be either
       SND_PCM_ACCESS_RW_INTERLEAVED or
       SND_PCM_ACCESS_RW_NONINTERLEAVED.
       There are also access types for MMAPed
       access, but this is beyond the scope
       of this introduction.
    */
    if (snd_pcm_hw_params_set_access(pcm_handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED) < 0) {
      fprintf(stderr, "Error setting access.\n");
      return(-1);
    }
  
    /* Set sample format */
    if (snd_pcm_hw_params_set_format(pcm_handle, hwparams, SND_PCM_FORMAT_S16_LE) < 0) {
      fprintf(stderr, "Error setting format.\n");
      return(-1);
    }

    /* Set sample rate. If the exact rate is not supported
       by the hardware, use nearest possible rate.
    */ 
    exact_rate = rate;
    if (snd_pcm_hw_params_set_rate_near(pcm_handle, hwparams, &exact_rate, NULL) < 0) {
      fprintf(stderr, "Error setting rate.\n");
      return(-1);
    }
    if (rate != exact_rate) {
      fprintf(stderr, "The rate %d Hz is not supported by your hardware.\n\n==> Using %d Hz instead.\n", rate, exact_rate);
    }

    /* Set number of channels */
    if (snd_pcm_hw_params_set_channels(pcm_handle, hwparams, 2) < 0) {
      fprintf(stderr, "Error setting channels.\n");
      return(-1);
    }

#if 0
    /* Set number of periods. Periods used to be called fragments. */ 
    if ((err = snd_pcm_hw_params_set_periods(pcm_handle, hwparams, periods, 0)) < 0) {
      fprintf(stderr, "Error setting periods -- %s.\n", snd_strerror(err));
      return(-1);
    }
#endif
  

    /* Set buffer size (in frames). The resulting latency is given by
       latency = periodsize * periods / (rate * bytes_per_frame)
    */
    if (snd_pcm_hw_params_set_buffer_size(pcm_handle, hwparams, (periodsize * periods)>>2) < 0) {
      fprintf(stderr, "Error setting buffersize.\n");
      return(-1);
    }
  
  
    /* Apply HW parameter settings to
       PCM device and prepare device
    */
    if (snd_pcm_hw_params(pcm_handle, hwparams) < 0) {
      fprintf(stderr, "Error setting HW params.\n");
      return(-1);
    }

    if (snd_pcm_prepare(pcm_handle) < 0) {
        fprintf(stderr, "Error preparing device\n");
      return(-1);
    }

    return 0;
}
  
void noise() {
    unsigned char *data;
    int            pcmreturn, l1, l2;
    short          s1, s2;
    int            frames;

    data = (unsigned char *)malloc(periodsize);
    frames = periodsize >> 2;
    for(l1 = 0; l1 < 100; l1++) {
        for(l2 = 0; l2 < num_frames; l2++) {
            s1 = (l2 % 128) * 100 - 5000;  
            s2 = (l2 % 256) * 100 - 5000;  
            data[4*l2] = (unsigned char)s1;
            data[4*l2+1] = s1 >> 8;
            data[4*l2+2] = (unsigned char)s2;
            data[4*l2+3] = s2 >> 8;
        }

        while ((pcmreturn = snd_pcm_writei(pcm_handle, data, frames)) < 0) {
            snd_pcm_prepare(pcm_handle);
            fprintf(stderr, "<<<<<<<<<<<<<<< Buffer Underrun >>>>>>>>>>>>>>>\n");
        }
    }
}
  
int main(int argc, char **argv) {
    if (init() == 0)
        noise();
}


