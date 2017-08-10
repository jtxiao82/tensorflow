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

namespace patch
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}
Logging::Logging() {
}

Logging::~Logging() = default;

void Logging::DumpParam(const xla::Literal& literal) {

  std::cout << "Extraction input parameters" << std::endl;
  std::vector<float> param;
  for(int i=0; i<literal.f32s_size(); i++) {
    std::cout << *(literal.f32s().data() + i) << std::endl;
     
    param.push_back(*(literal.f32s().data() + i));
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

void Logging::IPC_Client(std::vector<float> param) {
  // IPC Client here
  // Need to transfer every parameters to Server
  // parameter array to buffer test
  char my_fifo_name [128];
  char buf2 [4096];
  int id = 6813;
  sprintf (my_fifo_name, "/tmp/add_client_fifo%d", id);
  if (mkfifo (my_fifo_name, 0664) == -1)
    perror ("mkfifo");
  int fd_server, fd, bytes_read;
  char v[32];
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
  // send message to server

  // Bug Need to be fixed
  // Sometimes the server will be blocking when write command is sent.
  // The reason may be the open() call every time
  // Sol:
  // The open() need to move to constructor to be intialization
  if ((fd_server = open (SERVER_FIFO, O_WRONLY)) == -1) {
      perror ("open: server fifo");
  }

  if (write (fd_server, buf2, strlen (buf2)) != strlen (buf2)) {
      perror ("write");
  }

  if (close (fd_server) == -1) {
      perror ("close");
  }    
  // read the answer
  if ((fd = open (my_fifo_name, O_RDONLY)) == -1)
     perror ("open");
  memset (buf2, '\0', sizeof (buf2));
  if ((bytes_read = read (fd, buf2, sizeof (buf2))) == -1)
      perror ("read");

  if (bytes_read > 0) {
      //printf ("Answer: %s\n", buf2);
  }

  if (close (fd) == -1) {
      perror ("close");
  }
  unlink(my_fifo_name);
}

std::vector<double> Logging::IPC_Server() {
  int fd, fd_client, bytes_read, i;
  char buf [4096];
  char *return_fifo;
  char *numbers [MAX_NUMBERS];

  if ((mkfifo (SERVER_FIFO, 0664) == -1) && (errno != EEXIST)) {
    perror ("mkfifo");
    exit (1);
  }

  if ((fd = open (SERVER_FIFO, O_RDONLY)) == -1)
    perror ("open");

  while (1) {
    // get a message
    memset (buf, '\0', sizeof (buf));
    /*if ((bytes_read = read (fd, buf, sizeof (buf))) == -1)
      perror ("read");
    if (bytes_read == 0)
      continue;*/

    bytes_read = read (fd, buf, sizeof (buf));

    if (bytes_read > 0) {
      return_fifo = strtok (buf, ", \n");
      i = 0;
      numbers [i] = strtok (NULL, ", \n");

      std::vector<double> param;

      while (numbers [i] != NULL && i < MAX_NUMBERS) {
        //printf ("Number%d: %s\n", i, numbers[i]);
        param.push_back(atof(numbers[i]));
        numbers [++i] = strtok (NULL, ", \n");
      }
      std::cout << param.size() <<std::endl;

      /* Send the result */
      if ((fd_client = open (SERVER_FIFO, O_WRONLY)) == -1) {
          perror ("open: client fifo");
          continue;
      }

      if (error)
          sprintf (buf, "Error in input.\n");
      else
          sprintf (buf, "Sum = %.8g\n", buf);

      if (write (fd_client, buf, strlen (buf)) != strlen (buf))
          perror ("write");

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

