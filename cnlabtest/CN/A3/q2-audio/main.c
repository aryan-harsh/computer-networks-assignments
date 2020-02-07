#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <portaudio.h>
#include <sndfile.h>

#define FRAMES_PER_BUFFER 1024

typedef struct
{
    uint16_t formatType;
    uint8_t numberOfChannels;
    uint32_t sampleRate;
    size_t size;
    float *recordedSamples;
} AudioData;

typedef struct
{
    float *snippet;
    size_t size;
} AudioSnippet;

AudioData initAudioData(uint32_t sampleRate, uint16_t channels, int type)
{
    AudioData data;
    data.formatType = type;
    data.numberOfChannels = channels;
    data.sampleRate = sampleRate;
    data.size = 0;
    data.recordedSamples = NULL;
    return data;
}

float avg(float *data, size_t length)
{
    float sum = 0;
    for (size_t i = 0; i < length; i++)
    {
        sum += fabs(*(data + i));
    }
    return (float) sum / length;
}

long storeFLAC(AudioData *data, const char *fileName)
{

    uint8_t err = SF_ERR_NO_ERROR;
    SF_INFO sfinfo =
    {
        .channels = data->numberOfChannels,
        .samplerate = data->sampleRate,
        .format = SF_FORMAT_FLAC | SF_FORMAT_PCM_16
    };

    SNDFILE *outfile = sf_open(fileName, SFM_WRITE, &sfinfo);
    if (!outfile) return -1;

    // Write the entire buffer to the file
    long wr = sf_writef_float(outfile, data->recordedSamples, data->size / sizeof(float));
    err = data->size - wr;

    // Force write to disk and close file
    sf_write_sync(outfile);
    sf_close(outfile);
    puts("Wrote to file!!!!");
    return err;
}

int main(void)
{
    PaError err = paNoError;
    if((err = Pa_Initialize())) goto done;
    const PaDeviceInfo *info = Pa_GetDeviceInfo(Pa_GetDefaultInputDevice());
    AudioData data = initAudioData(44100, info->maxInputChannels, paFloat32);
    AudioSnippet sampleBlock =
    {
        .snippet = NULL,
        .size = FRAMES_PER_BUFFER * sizeof(float) * data.numberOfChannels
    };
    PaStream *stream = NULL;
    sampleBlock.snippet = malloc(sampleBlock.size);
    time_t talking = 0;
    time_t silence = 0;
    PaStreamParameters inputParameters =
    {
        .device = Pa_GetDefaultInputDevice(),
        .channelCount = data.numberOfChannels,
        .sampleFormat = data.formatType,
        .suggestedLatency = info->defaultHighInputLatency,
        .hostApiSpecificStreamInfo = NULL
    };

    if((err = Pa_OpenStream(&stream, &inputParameters, NULL, data.sampleRate, FRAMES_PER_BUFFER, paClipOff, NULL, NULL))) goto done;
    if((err = Pa_StartStream(stream))) goto done;
    for(int i = 0;;)
    {
        err = Pa_ReadStream(stream, sampleBlock.snippet, FRAMES_PER_BUFFER);
        if (err) goto done;
        else if(avg(sampleBlock.snippet, FRAMES_PER_BUFFER) > 0.000550 && i < 1000) // talking
        {
            printf("You're talking! %d\n", i);
            i++;
            
            time(&talking);
            data.recordedSamples = realloc(data.recordedSamples, sampleBlock.size * i);
            data.size = sampleBlock.size * i;
            if (data.recordedSamples) memcpy((char*)data.recordedSamples + ((i - 1) * sampleBlock.size), sampleBlock.snippet, sampleBlock.size);
            else
            {
                free(data.recordedSamples);
                data.recordedSamples = NULL;
                data.size = 0;
            }
        }
        else //silence
        {
            double test = difftime(time(&silence), talking);
            if (test >= 1.5 && test <= 10)
            {
                char buffer[100];
                snprintf(buffer, 100, "file:%d.flac", i);
                storeFLAC(&data, buffer);
                talking = 0;
                free(data.recordedSamples);
                data.recordedSamples = NULL;
                data.size = 0;
            }
        }
    }

done:
    free(sampleBlock.snippet);
    Pa_Terminate();
    return err;
}