#include "stdio.h"
#include "math.h"
#include "string.h"

#define PI 3.14159
#define X_MAX 128
#define Y_MAX 64
#define Y_MULTI 54
#define Y_UPMOVE 64

void ShowWave_AndTran(float *gyh)
{
    float waveDate[X_MAX];
    for (int i = 0; i < X_MAX; ++i)
    {
        waveDate[i] = Y_MULTI * sin(PI * i / 64.0f);
        for (int j = 0; j < 4; ++j)
        {
            waveDate[i] += Y_MULTI * sin(PI * i * (j + 1) / 64.0f) * gyh[j];
        }
        waveDate[i] += Y_UPMOVE;
        printf("%f\n", waveDate[i]);
    }
}
int main(void)
{
    float gyh[4] = {0, 0.25, 0, 0};
    ShowWave_AndTran(gyh);
    return 0;
}
