#include "logging.h"

// **************
// IPC Named Pipe
// **************
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

// ****************
// IPC Share Memory
// ****************
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>

#define SERVER_FIFO "/tmp/addition_fifo_server"
#define MAX_NUMBERS 5000

// **************************************************
// patch namespace is used to convert input to string
// **************************************************
namespace patch
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}

// *******************
// Logging constructor
// *******************
Logging::Logging() {
}

// *********************
// Logging deconstructor
// *********************
Logging::~Logging() = default;

void Logging::SendParam(const xla::Literal& literal, xla::int64 count) {

  std::vector<float> param_ipc;
  // ***************
  // Config Transfer
  // ***************
  for(int i = 0; i < 4; i++)
    param_ipc.push_back(literal.shape().dimensions(i));

  // *****************
  // Raw Data Transfer
  // *****************

  switch(literal.shape().element_type()) {
    case xla::U32: 
      {
        for(int i = 0; i < literal.u32s_size() ; i++)
          param_ipc.push_back(literal.u32s(i));
      }
      break;
    case xla::S32:
      {
        for(int i = 0; i < literal.s32s_size() ; i++)
          param_ipc.push_back(literal.s32s(i));
      }
      break;
    case xla::F32:
      {
        for(int i = 0; i < literal.f32s_size() ; i++)
          param_ipc.push_back(literal.f32s(i));
        
      }
      break;
    default:
      break;
  }
  // Response time
  sleep(0.1);

  int Transfer = IPC_Client(param_ipc);
}

std::vector<float> Logging::RecvResult(const xla::Literal* literal) {

  // Return Result
  std::vector<float> output_ipc;
  output_ipc = IPC_Server();
  return output_ipc;
}

int Logging::IPC_Client(std::vector<float> param) {
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
  if (mkfifo (my_fifo_name, 0664) == -1) {}
    //perror ("mkfifo");

  int fd_server, fd, bytes_read;

  // Use v[] to store parameters and string cat to buf2
  // The char array size will demonate the communication throughput
  char v[4096];

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

  // Print send write command
  std::cout << "Send Param" << std::endl;

  // Open Server fifo file
  if ((fd_server = open (SERVER_FIFO, O_RDWR)) == -1) {
      //perror ("open: server fifo");
  }

  // Use write command to send buf2 to another proccess
  if (write (fd_server, buf2, strlen (buf2)) != strlen (buf2)) {
      //perror ("write");
  }

  // Close the Server fifo file
  if (close (fd_server) == -1) {
      //perror ("close");
  }   

  return 0;
}


std::vector<float> Logging::IPC_Server() {
  int fd, fd_client, bytes_read, i;

  // Use buf to store parameters and receive from another proccess
  char buf [4096];

  char *return_fifo;
  char *numbers [MAX_NUMBERS];

  // Create fifo (a new file at local machine)
  if ((mkfifo (SERVER_FIFO, 0664) == -1) && (errno != EEXIST)) {
    //perror ("mkfifo");
    exit (1);
  }

  // Open Server fifo file
  if ((fd = open (SERVER_FIFO, O_RDWR)) == -1) {} // Sometime will be blocked by read(). SoL: SERVER_FIFO should set "O_RDWR". otherwise this will be block by open()
    //perror ("open");
  sleep(0.01);

  while (1) {
    // get a message
    memset (buf, '\0', sizeof (buf));

    // Print send read command
    std::cout << "Receive Output" << std::endl;

    // Use read command to receive datas from another proccess and store in buf
    bytes_read = read (fd, buf, sizeof (buf));

    if (bytes_read > 0) {
      return_fifo = strtok (buf, ", \n");
      i = 0;
      numbers [i] = strtok (NULL, ", \n");

      std::vector<float> param;

      while (numbers [i] != NULL && i < MAX_NUMBERS) {
        //printf ("Number%d: %s\n", i, numbers[i]);

        // Push recevie datas at a vector
        param.push_back(atof(numbers[i]));
        numbers [++i] = strtok (NULL, ", \n");
      }

      unlink(SERVER_FIFO);
      return param;

    }
  }
}

int Logging::IPC_Shm_Client(std::vector<float> param) {

  char c;
  int shmid;
  key_t key;
  int *shm, *s;

  /*
   * We'll name our shared memory segment
   * "5678".
   */
  key = 5678;

  /*
   * Create the segment.
   */
  if ((shmid = shmget(key, 27, IPC_CREAT | 0666)) < 0) {
      perror("shmget");
      exit(1);
  }

  /*
   * Now we attach the segment to our data space.
   */
  if (( shm = (int *)shmat(shmid, NULL, 0)) == (int *) -1) {
      perror("shmat");
      exit(1);
  }

  /*
   * Now put some things into the memory for the
   * other process to read.
   */
  s = shm;

  for(int i = 0; i < param.size(); i++) {
      *s++ = param[i];
       //std::cout << param[i] << std::endl; 
  }
  *s = NULL;

  /*
   * Finally, we wait until the other process 
   * changes the first character of our memory
   * to '*', indicating that it has read what 
   * we put there.
   */
  while (*shm != '*') {
    sleep(0.1);
  }
 
  return 0;
}


std::vector<float> Logging::IPC_Shm_Server() {
  int shmid;
  key_t key;
  int *shm, *s;

  /*
   * We need to get the segment named
   * "5678", created by the server.
   */
  key = 5679;

  /*
   * Locate the segment.
   */
  if ((shmid = shmget(key, 28, 0666)) < 0) {
      perror("shmget");
      exit(1);
  }

  /*
   * Now we attach the segment to our data space.
   */
  if ((shm = (int*)shmat(shmid, NULL, 0)) == (int *) -1) {
      perror("shmat");
      exit(1);
  }

  /*
   * Now read what the server put in the memory.
   */
  std::vector<float> param;
  for (s = shm; *s != NULL; s++) {
    //int val = s[0] - '0'; // Receive char. Convert char to int
    //int val = s[0]; // Receive int
    float val = s[0]; // Receive int
    param.push_back(val);
    //putchar(*s);
  }
  //putchar('\n');


  /*
   * Finally, change the first character of the 
   * segment to '*', indicating we have read 
   * the segment.
   */
  *shm = '*';
  return param;

}
