#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
/*
 * Servidor TCP
 */

struct mensagem{

	int validade; 
	char usuario[20];
	char mensagem[80];

};

int main(int argc, char **argv)
{
    unsigned short port;       
    //char sendbuf[12];              
    char recvbuf[512];  
		//char *sendbuf;            
    struct sockaddr_in client; 
    struct sockaddr_in server; 
    int s;                     /* Socket para aceitar conexoes       */
    int ns;                    /* Socket conectado ao cliente        */
    int namelen;
    int op, n = 0, nmsgs = 0, full = 0;
		struct mensagem msgs[5];
		struct mensagem recvmsg;               

    /*
     * O primeiro argumento (argv[1]) e a porta
     * onde o servidor aguardara por conexoes
     */
    if (argc != 2)
    {
        fprintf(stderr, "Use: %s porta\n", argv[0]);
        exit(1);
    }

    port = (unsigned short) atoi(argv[1]);

    /*
     * Cria um socket TCP (stream) para aguardar conexoes
     */
    if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket()");
        exit(2);
    }

   /*
    * Define a qual endereco IP e porta o servidor estara ligado.
    * IP = INADDDR_ANY -> faz com que o servidor se ligue em todos
    * os enderecos IP
    */
    server.sin_family = AF_INET;   
    server.sin_port   = htons(port);       
    server.sin_addr.s_addr = INADDR_ANY;

    /*
     * Liga o servidor a porta definida anteriormente.
     */
    if (bind(s, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
       perror("Bind()");
       exit(3);
    }

    /*
     * Prepara o socket para aguardar por conexoes e
     * cria uma fila de conexoes pendentes.
     */
    if (listen(s, 1) != 0)
    {
        perror("Listen()");
        exit(4);
    }

    /*
     * Aceita uma conexao e cria um novo socket atraves do qual
     * ocorrera a comunicacao com o cliente.
     */
    namelen = sizeof(client);
    if ((ns = accept(s, (struct sockaddr *)&client, (socklen_t *)&namelen)) == -1)
    {
        perror("Accept()");
        exit(5);
    }

		for(int i = 0; i < 5; i++)
				msgs[i].validade = 0;


		do{

				/* Recebe uma mensagem do cliente atraves do novo socket conectado */
				if (recv(ns, &op, sizeof(op), 0) == -1)
				{
				    perror("Recv()");
				    exit(6);
				}

				switch(op){

					case 1:

						if (recv(ns, &recvmsg, sizeof(recvmsg), 0) == -1){
								perror("Recv()");
								exit(6);
						}

						if(nmsgs == 5)
								full = 1;
						else
								full = 0;

						if (send(ns, &full, sizeof(full), 0) < 0){
				        perror("Send()");
				        exit(5);
				    }

						for(int i = 0; i < 5; i++)
							if(msgs[i].validade == 0){
								msgs[i] = recvmsg;
								nmsgs++;
								break;
						  }

						 break;

					case 2:					 

						 if (send(ns, &nmsgs, sizeof(nmsgs), 0) < 0){
									perror("Send()");
									exit(7);
						 }						 
						
						 for(int i = 0; i < 5; i++)
								if(msgs[i].validade != 0)
									if (send(ns, &msgs[i], sizeof(msgs[i]), 0) < 0){
										perror("Send()");
										exit(7);
							 		}

						 break;

					case 3:	
				
								if (recv(ns, recvbuf, sizeof(recvbuf), 0) == -1){
										perror("Recv()");
										exit(6);
								}

								for(int i = 0; i < 5; i++)
										if(strcmp(msgs[i].usuario,recvbuf) == 0){
											n++;
										}	

								if (send(ns, &n, sizeof(n), 0) < 0){
									perror("Send()");
									exit(7);
						 		}

							  for(int i = 0; i < 5; i++){
									if(strcmp(msgs[i].usuario,recvbuf) == 0){
										if (send(ns, &msgs[i], sizeof(msgs[i]), 0) < 0){
											perror("Send()");
											exit(7);
								 		}
										msgs[i].validade = 0;
										nmsgs-- ;
									}
								}

								n = 0;

						 break;
				
					case 4:

						if ((ns = accept(s, (struct sockaddr *)&client, (socklen_t *)&namelen)) == -1)
  					{
        				perror("Accept()");
        				exit(5);
    				}

						break;

				}

		}while(1);
    /* Fecha o socket conectado ao cliente */
    close(ns);

    /* Fecha o socket aguardando por conexões */
    close(s);

    printf("Servidor terminou com sucesso.\n");
    exit(0);
}


