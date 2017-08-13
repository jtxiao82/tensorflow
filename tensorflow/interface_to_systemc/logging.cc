#include "logging.h"

#include <iostream>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <error.h>
#include <string>
#include <sstream>
#include <stdlib.h>

#define SERVER_FIFO "/tmp/addition_fifo_server"
#define MAX_NUMBERS 500

// patch namespace is used to convert input to string
namespace patch
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}

// Logging constructor
Logging::Logging() {
}

// Logging deconstructor
Logging::~Logging() = default;

void Logging::DumpParam(const xla::Literal& literal) {

  std::cout << "Extraction input parameters" << std::endl;
  // Dump parameters in XLA backend "Computation Builder"
  std::vector<float> param;
  for(int i=0; i<literal.f32s_size(); i++) {
    std::cout << *(literal.f32s().data() + i) << std::endl;
     
    param.push_back(*(literal.f32s().data() + i));
  }
  // Use IPC PIPE to send parameters to systemc proccess
  IPC_Client(param);
}


void Logging::DumpConst(const xla::Literal& literal) {

  std::cout << "Extraction input constant" << std::endl;

  // Dump parameters in XLA backend "Computation Builder"
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

void Logging::IPC_Client(std::vector<float> param) {
  // IPC Client here
  // Need to transfer every parameters to Server
  // parameter array to buffer test

  // Feed char to my_fifo_name
  char my_fifo_name [128];
  
  // Use buf2 to store parameters and send to another proccess
  // The char array size will demonate the communication throughput
  char buf2 [4096];

  // Suppose allocate fifo at location "/tmp/add_client_fifo6813"
  int id = 6813;
  sprintf (my_fifo_name, "/tmp/add_client_fifo%d", id);

  // Create fifo (a new file at local machine)
  if (mkfifo (my_fifo_name, 0664) == -1)
    perror ("mkfifo");

  int fd_server, fd, bytes_read;

  // Use v[] to store parameters and string cat to buf2
  // The char array size will demonate the communication throughput
  char v[32];

  // Send parameters to char buffers
  double integer = param[0];
  sprintf(v, "%2.3f", integer);

  char const *pchar;
  for(int i=1; i<param.size(); i++) {
    strcat (v," ");
    std::string s = patch::to_string(param[i]);
    pchar = s.c_str();
    strcat (v,pchar);
  }
  strcpy (buf2, my_fifo_name);
  strcat (buf2, " ");
  strcat (buf2, v);

  // Open Server fifo file
  if ((fd_server = open (SERVER_FIFO, O_WRONLY)) == -1) {
      perror ("open: server fifo");
  }

  // Use write command to send buf2 to another proccess
  if (write (fd_server, buf2, strlen (buf2)) != strlen (buf2)) {
      perror ("write");
  }

  // Close the Serveri fifo file
  if (close (fd_server) == -1) {
      perror ("close");
  }    

  // Open Server fifo file
  if ((fd = open (my_fifo_name, O_RDONLY)) == -1)
     perror ("open");

  // Should it be removed ?
  memset (buf2, '\0', sizeof (buf2));

  // Use read command to read the datas from another proccess
  // This function call "read()" will blocking this proccess until the datas are be read !
  if ((bytes_read = read (fd, buf2, sizeof (buf2))) == -1)
      perror ("read");

  // Open Server fifo file
  if (close (fd) == -1) {
      perror ("close");
  }
 
  // Remove the server fifo file at local 
  unlink(my_fifo_name);
}

std::vector<double> Logging::IPC_Server() {
  int fd, fd_client, bytes_read, i;

  // Use buf to store parameters and receive from another proccess
  char buf [4096];

  char *return_fifo;
  char *numbers [MAX_NUMBERS];

  // Create fifo (a new file at local machine)
  if ((mkfifo (SERVER_FIFO, 0664) == -1) && (errno != EEXIST)) {
    perror ("mkfifo");
    exit (1);
  }

  // Open Server fifo file
  if ((fd = open (SERVER_FIFO, O_RDONLY)) == -1)
    perror ("open");

  while (1) {
    // get a message
    memset (buf, '\0', sizeof (buf));

    // Use read command to receive datas from another proccess and store in buf
    bytes_read = read (fd, buf, sizeof (buf));

    if (bytes_read > 0) {
      return_fifo = strtok (buf, ", \n");
      i = 0;
      numbers [i] = strtok (NULL, ", \n");

      std::vector<double> param;

      while (numbers [i] != NULL && i < MAX_NUMBERS) {
        //printf ("Number%d: %s\n", i, numbers[i]);

        // Push recevie datas at a vector
        param.push_back(atof(numbers[i]));
        numbers [++i] = strtok (NULL, ", \n");
      }
 
      // Debug for observe receive data size
      std::cout << param.size() <<std::endl;

      // Open Server fifo file
      if ((fd_client = open (SERVER_FIFO, O_WRONLY)) == -1) {
          perror ("open: client fifo");
          continue;
      }

      // Use write command to notify another proccess
      // Represent the it has receive successfully
      if (write (fd_client, buf, strlen (buf)) != strlen (buf))
          perror ("write");
   
      // Close server fifo file
      if (close (fd_client) == -1)
          perror ("close");   

      return param;

    }
  }
}



void Logging::DumpPtx(const char* ptx) {

  std::cout << ptx << std::endl;
}

void Logging::DumpShape(std::string str) {

  std::cout << str << std::endl;
}

void Logging::Monitor(std::string str) {
  
  //std::cout << str << std::endl;
}

