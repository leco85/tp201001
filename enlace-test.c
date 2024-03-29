/*
 * Arquivo contendo uma aplicacao para teste das funcoes da camada de enlace
 *
 * Componentes: Felipe Almeida Tavares
 *              Leandro Lima Monteiro Coelho
 *		Rafael de Oliveira Costa
 */

#include "fisica.h"
#include "enlace.h"
#include "comutador.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <unistd.h>

int indicacao_frame;

void exibir_menu() {
	printf("Selecione uma Função\n");
	printf("'a': L_Activate_Request\n");
	printf("'i': L_Data_Indication\n");
	printf("'d': L_Data_Request\n");
	printf("'r': L_Data_Receive\n");
	printf("'x': L_Deactivate_Request\n");
	printf("'s': Sair\n");
}

void menu(char option){

	unsigned char mac;
	char *switch_addr;
	char *message_to_send;
	char *buffer;
	char *temp;
	int switch_port;
	int mac_temp;
	int not_leave=0;

	unsigned char mac_source;
	char frame_recv[200];
	int max_frame;

	switch(option){
  		case 'a':      
			do {
				printf("Digite o MAC do host\n");
				scanf("%d",&mac_temp);
				if (mac_temp < 1 || mac_temp > 255)
					printf("MAC inválido. Escolha um entre 0 e 255\n");
			} while (mac_temp < 1 || mac_temp > 255);
			mac=(unsigned char)mac_temp;

			printf("Digite a porta do comutador\n");
			scanf("%d",&switch_port);
			getchar(); // a proxima entrada eh por fgets
			
			switch_addr  = (char*) malloc (15 * sizeof(char));
			buffer  = (char*) malloc (15 * sizeof(char));
			do{
				printf("Digite o IP do comutador\n");
				fgets(switch_addr, 15, stdin);
				switch_addr[strlen(switch_addr)-1] = '\0'; //remover o \n consumido
				strcpy (buffer,switch_addr);
				temp = strtok (buffer,".");
				while (temp != NULL)
				{
					if(atoi(temp)>255 || atoi(temp)<0){
						not_leave=1;
						printf("--Failed endereco invalido\n");
						break;
					}
		 			temp = strtok (NULL, ".");
					not_leave=0;
				}	
			}while(not_leave);
			free(buffer);

			if(!L_Activate_Request(mac, switch_port,switch_addr)){
					 printf("--Failed L_Activate_Request\n");
					 break;
			}

			printf("--Sucess L_Activate_Request\n");
			exibir_menu();
			break;

		case 'i':
			indicacao_frame = L_Data_Indication();
			if(!indicacao_frame){
				printf("nao ");
			}        
			printf("existe um quadro recebido na camada de enlace\n");
			exibir_menu();
			break;

		case 'd':
			do {
				printf("Digite o MAC destino\n");
				scanf("%d",&mac_temp);
				if (mac_temp < 1 || mac_temp > 255)
					printf("MAC inválido. Escolha um entre 0 e 255\n");
			} while (mac_temp < 1 || mac_temp > 255);
			mac=(unsigned char)mac_temp;
			getchar(); // a proxima entrada eh por fgets

			message_to_send  = (char*) malloc (PAYLOAD_SIZE * sizeof(char));
			printf("Digite a mensagem a ser enviada\n");
			fgets(message_to_send, PAYLOAD_SIZE, stdin);
			message_to_send[strlen(message_to_send)-1] = '\0';

			L_Data_Request(mac,message_to_send,strlen(message_to_send));

			printf("--Sucess L_Data_Request\n");	
			exibir_menu();
			break;
		case 'r':
			if (indicacao_frame) {
				printf("Qual o tamanho maximo dos dados do frame?\n");
				scanf("%d", &max_frame);

				memset(&frame_recv, 0, sizeof(frame_recv));
				printf("recuperando..\n");
				if (L_Data_Receive(&mac_source, frame_recv, max_frame) >= 0) {
				printf("A mensagem recebida do mac %d foi: %s\n", mac_source, frame_recv); 
				}
				else {
					printf("A mensagem recebida é maior que o tamanho maximo de dados do frame\n"); 
				}
			}
			else {
				printf("Verifique antes de tentar receber!\n");
			}
			exibir_menu();
			break;
	
		case 'x':
			L_Deactivate_Request();            
			printf("--Sucess L_Deactivate_Request\n");
			exibir_menu();
			
		case 's':            
			exit(1);
	}
	
}

int kbhit()
{
    struct timeval tv = { 0L, 0L };
    fd_set fds;
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv);
}

int getch()
{
    int r;
    unsigned char c;

    if ((r = read(0, &c, sizeof(c))) < 0) {
        return r;
    } else {
        return c;
    }
}

int main(){
	char ipreal[15];
	printf("Qual o IP real desta maquina?\n");
	fgets(ipreal, 15, stdin);
	ipreal[strlen(ipreal)-1] = '\0';

	enlace_definirIPreal(ipreal);
	fisica_definirIPreal(ipreal);

	exibir_menu();
	while(1){
		while (!kbhit()) {
			L_MainLoop();
		}
		menu(getch());
	}

   return 0;
}
