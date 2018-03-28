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

#define SIZE (1024*1024)
#define PORT 5000
#define BUF_SIZE 100


int main(int argc,char *argv[])
{
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);

    printf("Socket estabelecido com sucesso\n");

    struct sockaddr_in serv_addr;
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT);

    int slen = sizeof(serv_addr);

    int lastTime = 1;

    bind(listenfd, (struct sockaddr*)&serv_addr,sizeof(serv_addr));

    if(listen(listenfd, 10) == -1)
    {
        printf("Falha ao escutar\n");
        return -1;
    }

    for (;;)
    {
        int connfd = accept(listenfd, (struct sockaddr*)NULL ,NULL);

        /* Abre o arquivo que qeuremos transferir */
        FILE *fp = fopen(argv[1],"rb");
        if(fp==NULL)
        {
            printf("Erro ao abrir o arquivo que queremos enviar");
            return 1;
        }

        /* Le os dados do arquivo e começa a enviar*/
        for (;;)
        {
            /* Primeiro le o arquivo em partes de tamanho 100 bytes */
            unsigned char buff[BUF_SIZE]={0};
            int nread = fread(buff,1,BUF_SIZE,fp);
            printf("Bytes lidos %d \n", nread);

            /* Se leitura esta sendo bem sucedida entao envia o arquivo */
            if(nread > 0)
            {
                printf("Enviando \n");
                sendto(connfd, buff, nread, 0, (struct sockaddr*)&serv_addr, slen);
            }

            /*
             * Se acontecer algum problema com a leitura ou
             * houve algum erro ou chegamos ao final do arquivo
             */
            if (nread < BUF_SIZE)
            {
                if (feof(fp))
                    printf("Fim do arquivo\n");
                if (ferror(fp))
                    printf("Erro na leitura do arquivo\n");
                break;
            }
        }
        close(connfd);
        sleep(1);
    }

    return 0;
}