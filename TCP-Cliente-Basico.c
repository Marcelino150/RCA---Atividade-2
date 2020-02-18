#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio_ext.h>

/*
 * Cliente TCP
 */

struct mensagem{

	int validade;
	char usuario[20];
	char mensagem[80];

};

int main(int argc, char **argv)
{
    unsigned short port;       
    char sendbuf[512];              
    char recvbuf[512];              
    struct hostent *hostnm;    
    struct sockaddr_in server; 
	  struct mensagem msg;
    int s, op, n = 0, full;                     

    /*
     * O primeiro argumento (argv[1]) e o hostname do servidor.
     * O segundo argumento (argv[2]) e a porta do servidor.
     */
    if (argc != 3)
    {
        fprintf(stderr, "Use: %s hostname porta\n", argv[0]);
        exit(1);
    }

    /*
     * Obtendo o endereco IP do servidor
     */
    hostnm = gethostbyname(argv[1]);
    if (hostnm == (struct hostent *) 0)
    {
        fprintf(stderr, "Gethostbyname failed\n");
        exit(2);
    }
    port = (unsigned short) atoi(argv[2]);

    /*
     * Define o endereco IP e a porta do servidor
     */
    server.sin_family      = AF_INET;
    server.sin_port        = htons(port);
    server.sin_addr.s_addr = *((unsigned long *)hostnm->h_addr);

    /*
     * Cria um socket TCP (stream)
     */
    if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket()");
        exit(3);
    }

    /* Estabelece conexao com o servidor */
    if (connect(s, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("Connect()");
        exit(4);
    }

    do{

       printf("Opcoes:\n 1 - Cadastrar mensagem\n 2 - Ler mensagens\n 3 - Apagar mensagens\n 4 - Sair da Aplicacao\n > ");

				scanf("%d",&op);

				if(op >= 1 && op <= 4){
				  /* Envia a mensagem no buffer de envio para o servidor */
				  if (send(s, &op, sizeof(int), 0) < 0)
				  {
				      perror("Send()");
				      exit(5);
				  }

					system("clear");
				}

     		switch(op){

				 case 1:

					printf("Usuario: ");
					__fpurge(stdin);
					gets(msg.usuario);

					printf("Mensagem: ");
					__fpurge(stdin);
					gets(msg.mensagem);

					msg.validade = 1;

				  if (send(s, &msg, sizeof(msg), 0) < 0){
		          perror("Send()");
		          exit(5);
		      }

					if (recv(s, &full, sizeof(full), 0) == -1){
							perror("Recv()");
							exit(6);
					}

					if(full)
						printf("Memoria Cheia! A mensagem nao foi gravada.\n");

					printf("\nPressione qualquer tecla...\n");
					__fpurge(stdin);
					getchar();
					break;

				 case 2:
		
						if (recv(s, &n, sizeof(n), 0) == -1){
								perror("Recv()");
								exit(6);
						}

						printf("Mensagens cadastradas: %d\n",n);

						for(int i = 0; i < n; i++){
						
								if (recv(s, &msg, sizeof(msg), 0) == -1){
										perror("Recv()");
										exit(6);
								}

								printf("Usuario: %s		Mensagem: %s\n",msg.usuario,msg.mensagem);
						}

						printf("\nPressione qualquer tecla...\n");
						__fpurge(stdin);
						getchar();
					break;

				 case 3:

					printf("Usuario: ");
					__fpurge(stdin);
					gets(sendbuf);

					if (send(s, sendbuf, sizeof(sendbuf), 0) < 0){
							perror("Send()");
							exit(7);
					}

					if (recv(s, &n, sizeof(n), 0) == -1){
							perror("Recv()");
							exit(6);
				  }

					printf("Mensagens apagadas: %d\n",n);

					for(int i = 0; i < n; i++){

							if (recv(s, &msg, sizeof(msg), 0) == -1){
								perror("Recv()");
								exit(6);
				  		}

							printf("Usuario: %s		Mensagem: %s\n",msg.usuario,msg.mensagem);				
						
					}

					printf("\nPressione qualquer tecla...\n");
					__fpurge(stdin);
					getchar();

					break;
       }

			 system("clear");

			 n = 0;
    }while(op != 4);
	
    /* Fecha o socket */
    close(s);

    printf("Cliente terminou com sucesso.\n");
    exit(0);

}


