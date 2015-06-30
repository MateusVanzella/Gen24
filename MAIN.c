//CM-Messenger
//Cristiano José dos Santos
//Mateus Ribeiro Vanzella

//Compila com gcc MAIN.c -lpthread -o MAIN

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>



#define MAX_CMN_LEN 100
#define MAX_MENSAGENS 30
#define MAX_CONEXOES 20



typedef struct contato
{
    char ip_port[15];
    char nome[30];
    struct sockaddr_in destino;
    int socket;


}contato;

typedef struct pacote
{
    char endereco[15];
    char msn[1024];
} pacote;

typedef struct handler
{
    int socket_cliente;
    char* ip_cliente;
} handler;

//typedef struct Contato contato;



int socket_desc, client_sock, sk, i, true = 1,n;
char send_data [1024] , recv_data[1024];
struct sockaddr_in server, client;
int sin_size, numcontatos =0, Nmensagem = 0;
char mensagens[30][1024];
pacote entrada[30];



void *gerenciador_conexao(void *socket_desc)
{
    handler hand=*(handler*)socket_desc;

    int sk = hand.socket_cliente;
    char endereco[15];
    int SIZE;
    char *mensagem, mensagem_cliente[1024];


    while( (SIZE = recv(sk , mensagem_cliente , 2000 , 0)) > 0)
    {

        mensagem_cliente[SIZE] = '\0';

		Nmensagem++;
		printf("Nova mensagem chegou! \n\n");
        printf("%s\n\n",mensagem_cliente);
	
	}

    return 0;
}

void* gerenciador_recepcao(void* id)
{
    listen(socket_desc , 20);
    sk = sizeof(struct sockaddr_in);
    pthread_t thread_server;
    handler arg;

    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&sk)) )
    {

        arg.socket_cliente = client_sock;
		arg.ip_cliente = inet_ntoa((struct in_addr)client.sin_addr);

        if( pthread_create(&thread_server , NULL ,  gerenciador_conexao, (void*)&client_sock) < 0)
        {
            perror("could not create thread");
            return NULL;
        }
 
        puts("\nUm usuario foi conectado a voce!\n");
        
    }

    if (client_sock < 0)
    {
        perror("Falha de aceitação");
        return NULL;
    }
}


void Open_Server()
{
    //Criação do socket servidor
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc < 0)
    {
        printf("Falha ao criar o socket");
        exit(0);
    }

   // puts("socket criado");
    //Prepara estrutura do socket
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 40000 );

    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error MSN
        perror("Erro: Falha de bind!");
        return;
    }
    //Fim da criação do socket
    //Criação da thread que ficará a espera de conexões
    pthread_t thread_conexoes;

    if( pthread_create(&thread_conexoes , NULL ,  gerenciador_recepcao , (void*)(intptr_t)socket_desc) < 0)
    {
        perror("Nao foi possivel criar a conexao!");
    }
}

int main()
{

    int i,j,p,choice,count=0, loop=1,limit,result, aux1,k,l,m,o,npontos = 0;
    int frag[4];
    contato contat[30];
    contato aux[1];
    char aux2[15],aux4[15];
    char aux3[2] = {'.','\0'};
    char nome[30],str[30];
    char msn[1024], contact[30];

    int res, valopt,ncontatos;
    long arg = 0;;
    struct timeval tv;
    fd_set myset;
    socklen_t lon;

    for(i=0; i<30; i++)
		for(j=0;j<1024;j++)
        entrada[i].msn[j] = 0;
		

	//Cria socket
    Open_Server();

    while(loop)
    {
        printf("*************************MENU******************************\n");
        printf("1-Listar contatos ( %d ):\n", count);
        printf("2-Adicionar contato:\n");
        printf("3-Enviar mensagem a um contato:\n");
        printf("4-Enviar mensagem a um grupo de contatos:\n");
        printf("5-Excluir um contato:\n");
        //printf("6-Ler mensagens(%d):\n", Nmensagem);
        printf("6-Sair:\n\n");
        scanf("%d",&choice);

        while(choice<1 || choice>7)
        {
            printf("Nao ha essa opçao (apenas as apçoes se 1 - 7 são validas!!) ");
            fflush(stdin);
            scanf("%d", &n);
        }


        switch(choice)
        {

        case 1:
            if(count != 0)
            {
                printf("***********************Contatos*****************************\n");
                printf("\n ");

                for(j=0; j<count; j++)
                {
                    printf("Nome :%s      IP:%s\n", contat[j].nome,contat[j].ip_port);
                }
            }
            else
                printf("\n\n\nNão ha nenhum contato na lista\n\n\n");
            break;

        case 2:


            printf("Nome do contato(30 caracteres):\n");
            scanf("%s", str);
            str[29] = '\0';
			npontos=0;
            strcpy(contat[count].nome,str);

            printf("IP adress e numero da porta(XXX.XXX.XXX.XXX):\n");
            printf("**nao esqueça de colocar os pontos**\n");
            scanf("%s", aux2);
            strcpy(aux4,aux2);

            // verificação do tamanho do ip
            if(strlen(aux2) > 15 || strlen(aux2) < 7)
            {
                printf("IP invalido:numeros de caracter menor que 15\n");
                break;
            }
            else
            {

                // verificação da pontação do ip
                for(l=0 ; l<strlen(aux2) ; l++)
                {
                    if(aux2[l] == aux3[0])
                    {
                       npontos++;
                    }
                }

                if(npontos != 3)
                {
                    printf("IP invalido: Pontuaçao incorreta\n");
                    break;
                }
            }

            strcpy(contat[count].ip_port,aux4);
            strcpy(contat[count].nome,str);

            contat[count].socket = socket(AF_INET , SOCK_STREAM , 0);

            if (contat[count].socket == -1)
            {
                printf("Could not create socket");
                break;
            }

            contat[count].destino.sin_addr.s_addr = inet_addr(contat[count].ip_port);
            contat[count].destino.sin_family = AF_INET;
            contat[count].destino.sin_port = htons(40000);

            // codigo implementado com auxilio de http://developerweb.net/viewtopic.php?id=3196
            arg |= O_NONBLOCK;

            if(fcntl(contat[count].socket, F_SETFL, arg) < 0)
            {
                perror("Falha na conexao");
                break;
            }
            res =connect(contat[count].socket, (struct sockaddr *)&(contat[count].destino), sizeof(contat[count].destino));

            if(res < 0)
            {
                if(errno == EINPROGRESS)
                {
                    tv.tv_sec = 7;	//timeout de 7 segundos
                    tv.tv_usec = 0;
                    FD_ZERO(&myset);
                    FD_SET(contat[count].socket, &myset);
                    if(select(contat[count].socket+1,NULL,&myset,NULL,&tv))
                    {
                        lon = sizeof(int);
                        getsockopt(contat[count].socket, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon);

                        if(valopt)
                        {
                            printf("Falha: usuario sem conexao\n");
                            //Diminui vetor em caso de erro
                            break;
                        }
                    }
                    else
                    {
                        printf("Tempo  esgotado\n");
                        break;
                    }
                }
                else
                {
                    perror("Erro");
                    break;
                }
            }

            // Voltando a ser bloqueante
            if( (arg = fcntl(contat[count].socket, F_GETFL, NULL)) < 0)
            {
                perror("Erro ");
                break;
            }

            arg &= (~O_NONBLOCK);

            if( fcntl(contat[count].socket, F_SETFL, arg) < 0)
            {
                perror("Erro ");
                break;
            }

            count++;
            break;

        case 3:

            if(count == 0)
            {
                printf("Nao a contatos na sua lista!!");
                break;
            }

			printf("Nome do contato que deseja mandar a mensagem (30 caracteres):\n");
            scanf("%s",contact);

			for(p=0;p<count;p++)
			{

				if(strcmp(contat[p].nome, contact) == 0 )
				break;
			}
			if (p==count)
			{
				printf("contato não encontrado, mensagem não enviada\n");
				break;
			}

			printf("Digite a mensagem que deseja enviar: \n");
			getchar();
			fgets(msn,1024,stdin);

			if( send(contat[p].socket , msn , strlen(msn) , 0) < 0)
			{
            perror("Falha no envio");
            break;
			}
			
			//timeout de 3 seg 
			tv.tv_sec  = 3;  
			tv.tv_usec = 0;
			setsockopt(contat[i].socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
			
			break;

        case 4:
            if(count == 0)
            {
                printf("Nao a contatos na sua lista!!");
                break;
            }
			printf("digite quantos contatos deseja adicionar: \n");
			scanf("%d",&ncontatos);
			
			printf("Digite a mensagem que deseja enviar: \n");
			getchar();
			fgets(msn,1024,stdin);
			for(i=0;i<ncontatos;i++){
 
               	printf("Nome do %dº contato que deseja mandar a mensagem e tecle enter (30 caracteres) :\n", i+1);
				scanf("%s",contact);

				for(p=0;p<count;p++)
				{

					if(strcmp(contat[p].nome, contact) == 0 )
					break;
				}
				if (p==count)
				{
					printf("contato não encontrado, mensagem não enviada\n");
					break;
				}

				

				if( send(contat[p].socket , msn , strlen(msn) , 0) < 0)
				{
				perror("Falha no envio");
				break;
				}
			
				//timeout de 3 seg 
				tv.tv_sec  = 3;  
				tv.tv_usec = 0;
				setsockopt(contat[i].socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
            }
            break;

        case 5:
            printf("Nome do contato que se deseja deletar:\n");
            scanf("%s",nome);

            for(i=0 ; i<count ; i++)
            {
                if(contat[i].nome != nome)
                {
                    printf("Contato nao existente!!");
                    break;
                }
            }



            limit = strlen(nome);

            for(j=0; j<count; j++)
            {

                result = strncmp(nome,contat[j].nome,limit);

                if(result == 0)
                {
                    aux[0] = contat[j];
                    contat[j] = contat[count-1];
                    contat[count-1] = aux[0];
                    count--;
                    printf("Contato %s deletado:\n",contat[count].nome);
					close(contat[count].socket);
					
                }
            }
			
            break;

      /*  case 6:

            //Só printa a matriz onde é guardada as mensagnes
            printf("*****************Minhas mensagens********************\n");
            for(i=0 ; i<Nmensagem +1 ; i++)
            {
                if(entrada[i].msn[1] == 0)
                {
                    //printf("*****************Fim das mensagens***********************\n");
                    break;
                }

				printf("%s", entrada[i].msn);
				/*
                for(j=0 ; j<1024 ; j++)
                {

                    printf("%s", entrada[i].msn[j]);

                    if(entrada[i].msn[j+1] == aux3[1])
                    {
                        j = 1023;
                    }
                }*/
			/*
                printf("\n");

            }
            printf("*****************Fim das mensagens***********************\n");

            break;*/

        case 6:
            printf("\nBye Bye ^^\n");
			close(socket_desc);
			for(i=0;i<count;i++)
				{
				close(contat[i].socket);
				}
            loop =0;
            break;

        default:
            printf("Opcao invalida:\n");
        }
    }

    return 0;
}
