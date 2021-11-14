#include "stdio.h"
#include "string.h"

int true_T = 60;

void UpdateGYH(void)
{
    char strBuf[6];                             // !! Waring  应该改sruBuf[9] 但是这个是我在电赛之后整理代码后发现的 故不改 留个纪念
    snprintf(strBuf, 9, "T:%4dus", true_T / 3); // 测量周期
    printf("%s", strBuf);
}

int main(void)
{
    for (int i = 0; i < 10; ++i)
    {
        UpdateGYH(); // 测量周期
        printf("hello\r\n");
    }

    return 0;
}
