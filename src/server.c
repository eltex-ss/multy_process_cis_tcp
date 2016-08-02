#include "common.h"

struct sockaddr_in InitializeAddress(int port_num)
{
  struct sockaddr_in address;
  
  address.sin_family = AF_INET;
  address.sin_port = port_num;
  address.sin_addr.s_addr = INADDR_ANY;
  return address;
}

void BindSocket(int sock, struct sockaddr_in *address)
{
  if (bind(sock, (struct sockaddr *) address,
           sizeof(struct sockaddr_in)) == -1) {
    perror("bind error");
    close(sock);
    exit(1);
  }
}

int main(void)
{
  int server_socket;
  int client_socket;
  struct sockaddr_in server_address;
  struct sockaddr_in client_address;
  socklen_t client_length;
  char client_message[MESSAGE_LENGTH];
  char my_message[MESSAGE_LENGTH];
  char port_message[PORT_MESSAGE_LENGTH];
  int port_num = 10000;

  server_socket = CreateSocket();
  server_address = InitializeAddress(port_num);
  BindSocket(server_socket, &server_address);

  if (listen(server_socket, 10) == -1) {
    perror("listen error");
    close(server_socket);
    exit(1);
  }

  client_length = sizeof(struct sockaddr_in);
  sprintf(my_message, "Hi!");
  while (1) {
    if ((client_socket = accept(server_socket, (struct sockaddr *)
            &client_address, &client_length)) < 0) {
      perror("accept error");
      close(server_socket);
      exit(1);
    }

    ++port_num;
    memcpy(port_message, &port_num, sizeof(int));
    if (send(client_socket, port_message, PORT_MESSAGE_LENGTH, 0) < 0) {
      perror("send error");
      close(client_socket);
      close(server_socket);
      exit(1);
    }
    if (fork() == 0) {
      struct sockaddr_in fork_address;
      int fork_client_socket,
          fork_server_socket;

      fork_server_socket = CreateSocket();
      fork_address = InitializeAddress(port_num);
      BindSocket(fork_server_socket, &fork_address);
      if (listen(fork_server_socket, 1) < 0) {
        perror("listen error");
        close(fork_server_socket);
        exit(1);
      }
      if ((fork_client_socket =
           accept(fork_server_socket, (struct sockaddr *) &client_address,
                  &client_length)) < 0) {
        perror("accept error");
        close(fork_server_socket);
        exit(1);
      }

      printf("New port: %d\n", port_num);
      if (send(fork_client_socket, my_message, MESSAGE_LENGTH, 0) < 0) {
        perror("send error");
        close(fork_client_socket);
        close(fork_server_socket);
        exit(1);
      }
      if (recv(fork_client_socket, client_message, MESSAGE_LENGTH, 0) < 0) {
        perror("recvfrom error");
        close(fork_client_socket);
        close(fork_server_socket);
        exit(1);
      }
      printf("%s\n", client_message);
      close(fork_client_socket);
      close(fork_server_socket);
      exit(0);
    }
    else {
      close(client_socket);
    }
  }

  close(server_socket);

  return 0;
}
