#include "common.h"

int main(void)
{
  int sock;
  struct sockaddr_in server_address;
  socklen_t server_length;
  char server_message[MESSAGE_LENGTH];
  char my_message[MESSAGE_LENGTH];
  char port_message[PORT_MESSAGE_LENGTH];
  int port;

  sock = CreateSocket();

  server_address.sin_family = AF_INET;
  server_address.sin_port = 10000;
  server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

  sprintf(my_message, "Hello!");
  server_length = sizeof(struct sockaddr_in);
  if (connect(sock, (struct sockaddr *) &server_address, server_length) < 0) {
    perror("connect error");
    close(sock);
    exit(1);
  }
  /*  Get new port. */
  if (recv(sock, port_message, MESSAGE_LENGTH, 0) < 0) {
    perror("recv error");
    close(sock);
    exit(1);
  }
  memcpy(&port, port_message, sizeof(int));
  server_address.sin_port = port;
  close(sock);
  sock = CreateSocket();
  if (connect(sock, (struct sockaddr *) &server_address, server_length) < 0) {
    perror("connect2 error");
    close(sock);
    exit(1);
  }

  if (recv(sock, server_message, MESSAGE_LENGTH, 0) < 0) {
    perror("recv error");
    close(sock);
    exit(1);
  }
  printf("%s\n", server_message);
  if (send(sock, my_message, MESSAGE_LENGTH, 0) < 0) {
    perror("send error");
    close(sock);
    exit(1);
  }

  close(sock);

  return 0;
}
