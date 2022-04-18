#include <netdb.h>
#include <iostream>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

const size_t MESSAGE_MAX_SIZE = 100;

int main(int argc, char **argv)
{
    
    if (argc != 3)
    {
        //en el caso de poner mas parametros de la cuenta
        std::cerr << "Nº de parámetros incorrectos\n Formato: .\ejercicio_4 <direccion> <puerto>\n ";
        return -1;
    }

    struct addrinfo hints;
    struct addrinfo *res;

    memset((void *)&hints, 0, sizeof(struct addrinfo));

    hints.ai_flags = AI_PASSIVE;     
    hints.ai_family = AF_INET;       
    hints.ai_socktype = SOCK_STREAM;

    int recieve = getaddrinfo(argv[1], argv[2], &hints, &res);
    int sd = socket(res->ai_family, res->ai_socktype, 0);

    //Gestion de errores

    if (recieve != 0)
    {
        std::cerr << "Error getaddrinfo: " << gai_strerror(recieve) << std::endl;
        return -1;
    }

    if (sd == -1)
    {
        std::cerr << "Error en la creación de [socket]\n";
        return -1;
    }

      if (bind(sd, res->ai_addr, res->ai_addrlen) == -1)
    {
        std::cerr << "Error en la llamada a [bind]\n";
        return -1;
    }

    //solo una conexion
    if(listen(sd, 1) == -1){
        std::cerr << "Fallo en el [listen]\n";
        return -1;
    }

    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    struct sockaddr client;
    socklen_t clientLen = sizeof(struct sockaddr);
    int client_sd = accept(sd, (struct sockaddr *)&client, &clientLen);


    
   

    if(client_sd == -1){
        std::cerr << "No se ha aceptado la conexion TCP con el cliente [accept]\n";
    } 

    getnameinfo((struct sockaddr *)&client, clientLen, host, NI_MAXHOST, serv,
                 NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
    std::cout<<"Conexión desde:"<<host<<serv<<'\n';

    bool serverActive = true;
    while (serverActive)
    {
        char buffer[MESSAGE_MAX_SIZE];

        ssize_t bytesRecieved = recv(client_sd, (void *) buffer, (MESSAGE_MAX_SIZE -1 )* sizeof(char), 0);

        if( bytesRecieved <= 0 ){
            std::cout << "Conexion terminada\n";
            serverActive = false;
            continue;
        }
        
        send(client_sd, (void *) buffer, bytesRecieved, 0);
    }

    close(client_sd);
    close(sd);

    return 0;
}