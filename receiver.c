#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h> //fcntl
#include <sys/time.h>

#define SIZE (1024*1024)
#define PORT 5000
#define BUF_SIZE 100
#define  SA  struct  sockaddr


int main(int argc,char *argv[])
{

    int size_recv , total_size= 0;
    struct timeval begin , now;
    double timediff;
    int timeout = 2;

	/*Criando o arquivo onde o arquivo recebido sera guardado */
    FILE *fp = fopen("recebi2.txt", "ab");
    if(NULL == fp)
    {
        printf("Erro ao criar o arquivo");
        return 1;
    }

    /* Criando um socket primeiro */
    int sockfd = 0;
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0))< 0)
    {
        printf("\n Erro : Nao foi possivel criar o socket \n");
        return 1;
    }

    /* Inicializando a estrutura de dados sockaddr_in */
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT); // port
    //para usar um ip qualquer use inet_addr("10.10.10.10"); ao invés de htonl(INADDR_ANY)
    //serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);

    /* Fazendo a conexao com o sender*/
    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0)
    {
        printf("\n Erro : Conexao com o sender falhou \n");
        return 1;
    }

    /* Recebe os dados em parte do tamanho 100*/
    int bytesReceived = 0;
    char buff[BUF_SIZE];

    //make socket non blocking
    fcntl(sockfd, F_SETFL, O_NONBLOCK);
     
    //pega o tempo inicial
    gettimeofday(&begin , NULL);
     
    while(1)
    {
        gettimeofday(&now , NULL);
         
        //tempo passado em segundos
        timediff = (now.tv_sec - begin.tv_sec) + 1e-6 * (now.tv_usec - begin.tv_usec);
         
        //Se recebeu algum dado, mas temos um timeout depois, deve parar
        if( total_size > 0 && timediff > timeout )
        {
            break;
        }
         
        //Se nao recebemos nenhum dado, espera um pouco mais, o dobro do timeout
        else if( timediff > timeout*2)
        {
            break;
        }
        
        int tam_sender = sizeof(struct sockaddr_in);
        char buff[BUF_SIZE];
        memset(buff, 0, BUF_SIZE); //Limpa a variavel
        while((size_recv =  recvfrom(sockfd, buff, BUF_SIZE, 0, (struct sockaddr*) &serv_addr, &tam_sender) ) > 0)
        {
            total_size += size_recv;
            printf("Bytes recebido %d\n",total_size);
            fwrite(buff, 1, size_recv, fp);

            //reseta tempo incial
            gettimeofday(&begin , NULL);
        }

        if(bytesReceived < 0)
        {
            printf("\n Erro na leitura \n");
        } 
    }

    return 0;
}
