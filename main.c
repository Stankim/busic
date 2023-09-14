

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <math.h>
#include <string.h>

/*
The header of a wav file Based on:
https://ccrma.stanford.edu/courses/422/projects/WaveFormat/
*/
typedef struct wavfile_header_s
{
    char    ChunkID[4];     /*  4   */
    int32_t ChunkSize;      /*  4   */
    char    Format[4];      /*  4   */

    char    Subchunk1ID[4]; /*  4   */
    int32_t Subchunk1Size;  /*  4   */
    int16_t AudioFormat;    /*  2   */
    int16_t NumChannels;    /*  2   */
    int32_t SampleRate;     /*  4   */
    int32_t ByteRate;       /*  4   */
    int16_t BlockAlign;     /*  2   */
    int16_t BitsPerSample;  /*  2   */

    char    Subchunk2ID[4];
    int32_t Subchunk2Size;
} wavfile_header_t;

/*Standard values for CD-quality audio*/
#define SUBCHUNK1SIZE   (16)
#define AUDIO_FORMAT    (1) /*For PCM*/
#define NUM_CHANNELS    (2)
#define SAMPLE_RATE     (44100)

#define BITS_PER_SAMPLE (16)

#define BYTE_RATE       (SAMPLE_RATE * NUM_CHANNELS * BITS_PER_SAMPLE / 8)
#define BLOCK_ALIGN     (NUM_CHANNELS * BITS_PER_SAMPLE / 8)


#define MIN_PIVOT_FREQ (20)

/*Return 0 on success and -1 on failure*/
int write_PCM16_stereo_header(FILE *file_p, int32_t SampleRate, int32_t FrameCount)
{
    int ret;
    
    wavfile_header_t wav_header;
    int32_t subchunk2_size;
    int32_t chunk_size;
    
    size_t write_count;
    
    subchunk2_size  = FrameCount * NUM_CHANNELS * BITS_PER_SAMPLE / 8;
    chunk_size      = 4 + (8 + SUBCHUNK1SIZE) + (8 + subchunk2_size);
    
    wav_header.ChunkID[0] = 'R';
    wav_header.ChunkID[1] = 'I';
    wav_header.ChunkID[2] = 'F';
    wav_header.ChunkID[3] = 'F';
    
    wav_header.ChunkSize = chunk_size;
    
    wav_header.Format[0] = 'W';
    wav_header.Format[1] = 'A';
    wav_header.Format[2] = 'V';
    wav_header.Format[3] = 'E';
    
    wav_header.Subchunk1ID[0] = 'f';
    wav_header.Subchunk1ID[1] = 'm';
    wav_header.Subchunk1ID[2] = 't';
    wav_header.Subchunk1ID[3] = ' ';
    
    wav_header.Subchunk1Size = SUBCHUNK1SIZE;
    wav_header.AudioFormat = AUDIO_FORMAT;
    wav_header.NumChannels = NUM_CHANNELS;
    wav_header.SampleRate = SampleRate;
    wav_header.ByteRate = BYTE_RATE;
    wav_header.BlockAlign = BLOCK_ALIGN;
    wav_header.BitsPerSample = BITS_PER_SAMPLE;
    
    wav_header.Subchunk2ID[0] = 'd';
    wav_header.Subchunk2ID[1] = 'a';
    wav_header.Subchunk2ID[2] = 't';
    wav_header.Subchunk2ID[3] = 'a';
    wav_header.Subchunk2Size = subchunk2_size;

    write_count = fwrite(   &wav_header,
        sizeof(wavfile_header_t), 1,
        file_p);

    ret = (1 != write_count)? -1 : 0;
    return ret;
}

/*Data structure to hold a single frame with two channels*/



    #define DEFAULT_AMPL (0.0);
    #define DEFAULT_FREQ (0.0);


/*Generate two saw-tooth signals at two frequencies and amplitudes*/
int generate_audio_data( double freqs[][NUM_CHANNELS],
    double ampls[][NUM_CHANNELS],
    int length,
    int32_t SampleRate,
    int32_t end,
    int begin,
    int16_t  *buffer_p)
{
    double SampleRate_d = (double)SampleRate;
    if(SampleRate_d == 0){
        printf("The sample rate cannot be zero.\n");
        exit(0);
    }
    double SamplePeriod = 1.0 / SampleRate_d;
    
    double Periods[length][NUM_CHANNELS];
    double Slopes[length][NUM_CHANNELS];
    
    for(int i=0; i<length;i++){
        for(int l=0; l<NUM_CHANNELS;l++){
            if( freqs[i][l]*2 >= SampleRate_d){
                return -1;
            }
            if(freqs[i][l] == 0) Periods[i][l] = 0;
            else
            Periods[i][l] = 1.0 / freqs[i][l];
        }
    }
   // printf("done with loading periods\n");
    double phases[NUM_CHANNELS];
    for (int l = 0; l < NUM_CHANNELS; l++)	
    {
        phases[l]=0.0;
    }

    for(int k = begin,rounds_per_line = NUM_CHANNELS * length, lines= (end-begin)/rounds_per_line;k < end;)
    {
        for(int i=0; i<length;i++){
            for(int l=0; l<NUM_CHANNELS && k < end;l++, k++){
                //printf("%f\n", (double)(1- ((1.0/lines) * (k/rounds_per_line)) ));
                double amp = ampls[i][l] * (double)(1 - (double)(k-begin) /(double)(end - begin));

                Slopes[i][l]  = amp / Periods[i][l];

                phases[l] += SamplePeriod;
                if(phases[l] > Periods[i][l]){
                    phases[l] -= Periods[i][l];
                }

               // printf("writting %d\t%f\t%f\t%d\n", k, freqs[i][l], amp, (int16_t)(phases[l] * Slopes[i][l]));
                buffer_p[k]    = (int16_t)(phases[l] * Slopes[i][l]);

            }
            
        }
       // printf("\n");
    }
    return 0;
}


int index_of_note(char *note){

    int length = 0;
    for(length=0; note[length];length++);
        if(length< 2 || length > 3)
            return -1;
        if(note[length-1] < '0' || note[length-1]> '8')
            return -1;
        int y = note[0] - 'C';
        if(y<-2 || y>4)
            return -1;
        if(y<0)
            y=13+2*y;
        else if(y<3)
            y= 2*y;
        else
            y=2*y-1;
        if(length==2)
            y=12*(note[1] - '0') + y;
        else{
            if(note[1] != '#' || note[0] == 'B' || note[0] == 'E')
                return -1;
            y= 12*(note[2] - '0') + (y+1);
        }
        return y;
    }

    double getNoteFreq(char *arg, int counter){
        int note_index = index_of_note(arg);
        if(note_index == -1){
            printf("Format error: The note %s at index %d is invalid.\n",arg, counter);
            return -1;
        }
        return MIN_PIVOT_FREQ * pow(2, note_index/12.0);
    }

    int token_onset(char *str, int i){
        return  ((i > 0 && (str[i-1] == ':' || str[i-1] == ';') && str[i] >='A' && str[i] <= 'G')
         || str[i]==':' 
         || str[i] ==';'
         || (((str[i]==32||str[i]==10||str[i]==':'|| str[i]==';'||str[i]==9||i==0)
            && (str[i+1]!='\0' && str[i+1]!=32&&str[i+1]!=10&&str[i+1]!=':'&&str[i+1]!=';'&&str[i+1]!=9&&str[i+1]!=13)) 
         ));

    }
    int getNumberOfTokens(char *str){
        int j=0;
        for(int i=0;str[i+1]!='\0';token_onset(str, i) ?j++,i++:i++);
            return j;
    }

    char** getArgsFromStr(char *str, int n_arguments){
        char **arguments= malloc(sizeof(char*)*(n_arguments));
        for(int i=0,k=0;str[i]!='\0';i++){

            if(token_onset(str, i)){
                if(str[i] == ':'){
                    arguments[k] =":";
                }
                else if(str[i] == ';'){
                    arguments[k] =";";
                }
                else{
                    int j;
                    if((i != 0 || str[i] == 32 || str[i] == 9) && !(str[i] >='A' && str[i] <= 'G')) i++;
                    for(j=0;str[i]!='\0' && str[i]!=32 && str[i]!=10 && str[i]!=':' && str[i]!=';' && str[i]!=13 && str[i]!=9 ;i++,j++);
                        arguments[k] = malloc(j+1);
                    arguments[k][j] ='\0';
                    for(int l=0;l<j;l++){
                        arguments[k][l] = str[i-j+l];
                    }
                    if(i != 0 ||str[i] == 32 || str[i] == 9 || (str[i] >='A' && str[i] <= 'G')) i--;
                }
                k++;
            }
        }
        return arguments;

    }



    char* getFileContents(char *filename){
        char * buffer = 0;
        long length;
        FILE * f = fopen (filename, "rb");

        if (f)
        {
            fseek (f, 0, SEEK_END);
            length = ftell (f);
            fseek (f, 0, SEEK_SET);
            buffer = malloc (length+1);
            buffer[length] = '\0';
            if (buffer)
            {
                fread (buffer, 1, length, f);
            }
            fclose (f);
        }
        return buffer;
    }

    char * name_output_file(char *arg, int length){
        char* output_file= malloc(length+5);
        output_file[length+2] = '\0';
        output_file[length+1] = 'v';
        output_file[length] = 'a';
        output_file[length-1] = 'w';
        output_file[length-2] = '.';
        for(int i=0;i<length-2;output_file[i]=arg[i], i++);
            return output_file;
    }

    int main(int n_argz, char **argz)
    {
    

        if(n_argz==1){
            printf("Error: The input provided is not adequate.\n");
            return 0;
        }
        char **arguments;
        int n_arguments;
        char *output_file = "audio.wav";
        if(n_argz == 2){
            int length = strlen(argz[1]);
            if(argz[1][length-2] == '.' && argz[1][length-1] == 'v')
            {
                output_file = name_output_file(argz	[1], length);
                char* buffer = getFileContents(argz[1]);
                if (buffer){
                 n_arguments=getNumberOfTokens(buffer);
                 arguments = getArgsFromStr(buffer, n_arguments);
             }
             else{
                printf("The file does not exist.\n");
            }
        }
        else{
            printf("The second argument is not a .v file name.\n");
            return 0;
        }
        if(n_arguments == 0){
            printf("The file is empty.\n");
            return 0;
        }
    }
    else{
        n_arguments = n_argz -1;
        if(n_arguments == 0){
            printf("No arguments passed in.\n");
            return 0;
        }
        arguments = &argz[1];
    }

    double total_duration=0;
    for(int i=0,line_size=0;i<=n_arguments;i++){

        if((i == n_arguments  || arguments[i][0] == ';') && line_size != 0){

            total_duration += strtod(arguments[i-1], NULL);

            for(int j=i-line_size,channels=1;j<i-1;j++){


                if(arguments[j][0]==':'){
                    channels++;
                }
                if(channels > NUM_CHANNELS){
                    printf("The number of channels in line exceeds the allocated amount.\n");
                    exit(0);
                }
            }

            line_size=0;
            continue;
        }

        line_size++;
    }



    int ret;
    FILE* file_p;

    int32_t FrameCount = total_duration * SAMPLE_RATE;
    
    int16_t  *buffer_p = NULL;
    
    size_t written;
    

    /*Open the wav file*/
    file_p = fopen(output_file, "wb");
    if(NULL == file_p)
    {
        perror("fopen failed in main");
        ret = -1;
        goto error0;
    }
    
    /*Allocate the data buffer*/
    int channel_size = NUM_CHANNELS * sizeof(int16_t);
    buffer_p = (int16_t *)malloc(channel_size * FrameCount);
    if(NULL == buffer_p)
    {
        perror("fopen failed in main");
        ret = -1;
        goto error1;        
    }
    int progress=0;
    for(int i=0, line_size=0,size_of_cur_channel=0,max_size_of_channel_within_line=0;i<=n_arguments;i++){


        if(i == n_arguments || arguments[i][0] == ':' || arguments[i][0]==';'){
            if(size_of_cur_channel > max_size_of_channel_within_line)
                max_size_of_channel_within_line = size_of_cur_channel;

            size_of_cur_channel=0;
        }else if(arguments[i][0] >='A' && arguments[i][0] <= 'G'){
            size_of_cur_channel++;
        }



        if((i == n_arguments || arguments[i][0] == ';')&& line_size!=0){

            double duration = NUM_CHANNELS * strtod(arguments[i-1], NULL);
            double freqs[max_size_of_channel_within_line][NUM_CHANNELS];
            double ampls[max_size_of_channel_within_line][NUM_CHANNELS];
            for(int x= 0;x<max_size_of_channel_within_line;x++){
                for(int y=0;y<NUM_CHANNELS;y++){
                    freqs[x][y] = DEFAULT_FREQ;
                    ampls[x][y] =  (double)SHRT_MAX * DEFAULT_AMPL;
                }

            }

            for(int j= i - line_size,channel=0, channel_freqs_counter=-1;j<i-1;j++){

                if(arguments[j][0]==':'){
                    channel++;
                    channel_freqs_counter=-1;
                }
                else if(arguments[j][0] >='A' && arguments[i][0] <= 'G'){
                    channel_freqs_counter++;
                    freqs[channel_freqs_counter][channel] = getNoteFreq(arguments[j], j);
                    if(freqs[channel_freqs_counter][channel] == -1) return 0;
                }
                else{
                    ampls[channel_freqs_counter][channel] = strtod(arguments[j], NULL) * (double)SHRT_MAX;
                }

            }


            

            int begin = progress;
            int framecount_to_write = round(duration* SAMPLE_RATE);
            int end = progress + framecount_to_write ;

            if(i<line_size){
                begin = progress + 1;
            }
    /*Fill the buffer*/

            ret = generate_audio_data(freqs,
                ampls,
                max_size_of_channel_within_line,
                SAMPLE_RATE,
                end,
                begin,
                buffer_p);
            progress += framecount_to_write;
            max_size_of_channel_within_line=0;
            line_size=0;
            continue;
        }
        line_size++;
    }

//printf("done generating\n");

    if(ret < 0)
    {
        fprintf(stderr, "generating audio failed in main\n");
        ret = -1;
        goto error2;
    }

    /*Write the wav file header*/
    ret = write_PCM16_stereo_header(file_p,
        SAMPLE_RATE,
        FrameCount);
//printf("done writting header\n");
    if(ret < 0)
    {
        perror("write_PCM16_stereo_header failed in main");
        ret = -1;
        goto error2;
    }

    written = fwrite(buffer_p, channel_size, FrameCount, file_p);
    if(written < FrameCount)
    {
        perror("write_PCM16wav_data failed in main");
        ret = -1;
        goto error2;
    }
    /*Free and close everything*/

    error2:
    free(buffer_p);
    error1:
    fclose(file_p);
    error0:
    free(arguments);
    return ret;    
}
