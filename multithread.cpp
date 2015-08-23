#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#define NUMS 5

struct th_data
{

    int len;
    int start;
    int *vec;
};

void *summation(void*arg)
{
    th_data*data =  (th_data*)arg;
    int len = data->len;
    int start = data->start;
    int *sum = new int;
    *sum = 0;
    for(int i=0, j=start; i<len; i++, j++)
    {
       *sum = *sum + data->vec[j];
    }
    printf("the summation of this thread is: %d, with start id = %d\n", *sum, data->vec[start]);
    //pthread_exit((void*)sum);
    return (void*)sum;
};


int main()
{

    int numdata = 10000;
    int gvalue = 0;
    int *vec = (int*)malloc(numdata*sizeof(int));    
    for(int i=0; i<numdata; i++)
    {
       vec[i] = i+1;
       gvalue += vec[i];
    }
    th_data TD[NUMS];
    pthread_t th[NUMS];
    int len = (numdata-1)/NUMS + 1;
    int index =0;
    for(int i=0; i<NUMS; i++)
    {
        TD[i].start = index;
        if(index+len>numdata)
        {
            len = numdata-index;
        }
        TD[i].len = len;
        TD[i].vec = vec;
        pthread_create(&th[i], NULL, summation, (void*)&TD[i]);
        index = index + len;
    }
    int totalsum = 0;
    int *value;
    for(int i=0; i<NUMS; i++)
    {
       
       pthread_join(th[i], (void**)&value);
       totalsum = totalsum + *value;
       free(value);
    } 

    printf("the final summation is: %d, the sequential value is %d\n", totalsum, gvalue);
    free(vec);
}
