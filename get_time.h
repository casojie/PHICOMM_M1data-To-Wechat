#include <stdio.h>
#include <time.h>
struct TIME_cj
{
    int time_int;
    char time_char[25];
};

void get_time(struct TIME_cj * buf_time)
{
    time_t time_now;
    struct tm *curr_time = NULL;
    time(&time_now);
    buf_time->time_int=time_now;
    curr_time = localtime(&time_now);
    sprintf(buf_time->time_char,"%02d:%02d:%02d",curr_time->tm_hour,curr_time->tm_min,curr_time->tm_sec);
}