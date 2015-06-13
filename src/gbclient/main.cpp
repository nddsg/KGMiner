//
// Created by Baoxu Shi on 6/13/15.
//

#include "socket_client.h"

int main(int argc, const char *argv[]) {

  socket_client client;

  client.start("/tmp/gbserver");

  return 0;
}