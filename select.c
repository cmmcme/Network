#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>

#define BUF_SIZE 30
int main(int argc, char *argv[])
{
    fd_set reads, temps;
    int result, str_len;
    char buf[BUF_SIZE];
    struct timeval timeout;
    
    FD_ZERO(&reads);
    FD_SET(0,&reads);
    
    /*  만약 타임아웃이 여기 선언되어 있다면 와일문을 돌면서 timeout에 있는 초의 값이 점점 줄어든다
     timeout.tv_sec = 5;
     timeout.tv_usec = 0;
    */
    while(1)
    {
        temps = reads;
      
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
        
        result = select(1, &temps, 0, 0, &timeout); // 매칭된 갯수가 반환된다능
        if(result == -1)
        {
            puts("select() error!");
            break;
        }
        else if(result == 0)    //timeout 이 되면 select의 return 값이 0이다.
        {
            puts("time out!");
        }
        else
        {
            if(FD_ISSET(0,&temps))  // 0번째 파일 디스크립터가 바꼇나요?
            {
                str_len = read(0,buf,BUF_SIZE);
                buf[str_len] = 0;
                printf("message from consol : %s",buf);
            }
        }
    }
    
    
    
    
    
    return 0;
}
