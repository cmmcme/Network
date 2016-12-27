#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
void error_handling(char *message);

int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock;
	char message[BUF_SIZE];
	int str_len, i;
	
    /*
     struct sock_in
     {
     sa_family_t sin_family; // Address Family = sin_addr + sin_port
     uint16_t    sin_port; // 16bit TCP PORT Number
     struct in_adr sin_addr; // 32bit IP Address
     char sin_zero[8];   // No Used
     }
     */
	struct sockaddr_in serv_adr;
	struct sockaddr_in clnt_adr;

	socklen_t clnt_adr_sz;  // clnt_adr struct size
	
	if(argc!=2) {
		printf("Usage : %s <port>\n", argv[0]); // Error input
		exit(1);
	}
	
	serv_sock=socket(PF_INET, SOCK_STREAM, 0); // Create server_socket
	if(serv_sock==-1)
		error_handling("socket() error");
	
	memset(&serv_adr, 0, sizeof(serv_adr)); // init serv_adr
    
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY); // Server IP_address
	serv_adr.sin_port=htons(atoi(argv[1])); //Host to Network server Port number

    
    // 서버 소켓에 입력받은 서버 IP주소와 Port번호 할당.
	if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1) // bind(서버소켓, 서버 구조체, 서버 구조체의 크기)
		error_handling("bind() error");
	
    // 5개의 연결대기큐를 만들어 연결요청 가능 상태로 만든다.
	if(listen(serv_sock, 5)==-1) // Listen(서버 소켓, 큐의 크기)
		error_handling("listen() error");
	
  
	clnt_adr_sz=sizeof(clnt_adr);   // 클라이언트 구조체의 SIZE를 반환받는다.
    char *str_ptr;  // 클라이언트의 IP주소를 받기 위해 선언한 String 이다.
    

	for(i=0; i<5; i++)
	{
        // 클라이언트의 파일 디스크립터 값을 받는다.
		clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz); // accept(서버소켓, 클라이언트 구조체, 클라이언트 구조체의 크기)
        
		if(clnt_sock==-1)
			error_handling("accept() error");
        
		else
        {
            // 32bit 정수 형태의 clnt_IP_Addr을 문자열 형태의 IP주소로 변환한다.
            str_ptr = inet_ntoa(clnt_adr.sin_addr);
            // ntohs(network바이트 순서에서 host바이트 순서로 변환한다.)
            // s가 short를 의미하므로 PORT번호 변환에 사용된다.
            printf("Connected client %s %d\n", str_ptr, ntohs(clnt_adr.sin_port));
            printf("Connected client %d \n", i+1);
        }
	
        // clnt가 전송한 message를 읽어오는 함수이다. FIN이 오면 str_len == 0이기 때문에 클라이언트 소켓을 닫는다.
		while((str_len=read(clnt_sock, message, BUF_SIZE))!=0)
			write(clnt_sock, message, str_len); //str_len이 0이 아니라면 전송받은 message를 buffer에 write하는 함수이다.

		close(clnt_sock);
	}

	close(serv_sock);   // 5개의 클라이언트 소켓을 모두 전송 받으면 serv_sock을 닫는다.
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
