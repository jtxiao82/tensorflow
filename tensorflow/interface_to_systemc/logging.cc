#include "logging.h"

#include <iostream>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


Logging::Logging() {
}

Logging::~Logging() = default;

void Logging::DumpParam(const xla::Literal& literal) {

  std::cout << "Extraction input parameters" << std::endl;
  double param;
  for(int i=0; i<literal.f32s_size(); i++) {
    std::cout << *(literal.f32s().data() + i) << std::endl;
    param = *(literal.f32s().data() + i);
  }
  IPC_Client(param);
}


void Logging::DumpConst(const xla::Literal& literal) {

  std::cout << "Extraction input constant" << std::endl;
  double constant;
  for(int i=0; i<literal.f32s_size(); i++) {
    std::cout << *(literal.f32s().data() + i) << std::endl;
    constant = *(literal.f32s().data() + i);
  }

}

void Logging::DumpOp(xla::BinaryOperation binop) {
  std::cout << "Binary Operator" << std::endl;
  std::cout << binop << std::endl;
}

void Logging::IPC_Client(double param) {
  int client_to_server;
  char *myfifo = "/tmp/client_to_server_fifo";

  int server_to_client;
  char *myfifo2 = "/tmp/server_to_client_fifo";

  char str[BUFSIZ];

  sprintf(str, "%2.3f", param);

  /* write str to the FIFO */
  client_to_server = open(myfifo, O_WRONLY);
  server_to_client = open(myfifo2, O_RDONLY);
  write(client_to_server, str, sizeof(str));

  perror("Write:"); //Very crude error check

  read(server_to_client,str,sizeof(str));

  perror("Read:"); // Very crude error check

  std::cout << "...received from the server: " << str << std::endl;

  // ISSUE!!
  //
  // Dont close the FIFO. It will make transfer without lock
  // Think a general way to control the Transfer flow.

  //close(client_to_server);
  //close(server_to_client);
}

void Logging::DumpPtx(const char* ptx) {

  //std::cout << ptx << std::endl;
}

void Logging::DumpShape(std::string str) {

  std::cout << str << std::endl;
}

void Logging::Monitor(std::string str) {
  
  std::cout << str << std::endl;
}

