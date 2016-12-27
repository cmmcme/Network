#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUF_SIZE 1024
void error_handling(char *);

int main(int argc, char *argv[])
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    struct timeval timeout; //timeout 제한 시간
    fd_set reads, cpy_reads;    //readsfds, copy readsfds
    
    socklen_t adr_sz;
    int fd_max, str_len, fd_num, i;
    char buf[BUF_SIZE];
    if(argc != 2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));
    
    if(bind(serv_sock,(struct sockaddr *)&serv_adr,sizeof(serv_adr)) == -1)
        error_handling("bind() error!");
    if(listen(serv_sock, 5) == -1)
        error_handling("listen() error!");
    
    FD_ZERO(&reads);    // reads 배열을 0으로 초기화 시킨다.
    FD_SET(serv_sock,&reads);  //serv_sock 번째에 대응되는 필드의 bit값을 1로 설정한다.
    fd_max = serv_sock; // ???
    
    while(1)
    {
        cpy_reads = reads; // select 함수를 실행하면
        
        // while문 밖에 선언하면 시간이 계속 줄어든 채로 timeout 시간이 설정 되기 때문에 while문 안에 선언한다.
        timeout.tv_sec = 5;
        timeout.tv_usec = 5000;
        
        /*
         select(관리하는 파일의 갯수, readfds, writefds, exceptfds, timeout)
        timeout 시간동안 변경이 없다면 0을 반환, NULL로 설정하면 데이터가 있을 때 까지 무한 대기
         select 함수는 비트의 값이 1인 필드의 갯수를 반환한다.
         */
        if((fd_num = select(fd_max+1, &cpy_reads, 0, 0, &timeout)) == -1)
            break;
        if(fd_num == 0) // timeout 시간동안 변경이 없을 시
            continue;
        
        // 관찰할 파일 디스크립터의 번호만큼 반복한다..!
        for(i=0; i<fd_max+1; i++)
        {
            // 이것둥
            if(FD_ISSET(i,&cpy_reads))  //i번째 비트의 변화가 있을 때
            {
                // 서버 소켓의 상태 변화가 맞으면 연결요청에 대해 수락한다.
                if(i == serv_sock)
                {
                    adr_sz = sizeof(clnt_adr);
                    clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_adr, &adr_sz);
                    
                    // 연결된 파일 디스크립터(clnt_sock)의 정보를 등록한다
                    FD_SET(clnt_sock,&reads);
                    
                    // 생성된 client 소켓이 max 보다 크면 검색할 max 범위를 clnt_sock 으로 바꿔준다
                    if(fd_max < clnt_sock)
                        fd_max = clnt_sock;
                    
                    printf("connected client : %d \n", clnt_sock);
                }
                
                // 수신할 데이터가 있는 경우..!
                else
                {
                    // client가 연결된 후 buf에 있는 message의 길이를 반환 받음
                    str_len = read(i, buf, BUF_SIZE);
                    
                    
                     // read의 반환값이 0인 경우 -> 보낼 메세지가 없는 것 -> 연결 종료
                    if(str_len == 0)
                    {
                        //해당 bit를 0으로 초기화 시킨 후 client를 닫음
                        FD_CLR(i, &reads);
                        close(i);
                        printf("closed client : %d \n", i);
                    }
                    else
                        write(i,buf,str_len); // 0이 아닌 경우 해당 길이만큼 write 해줌
                }
            }
        }
    }
    close(serv_sock);
    return 0;
}
void error_handling(char *buf)
{
    fputs(buf, stderr);
    fputc('\n',stderr);
    exit(1);
}
