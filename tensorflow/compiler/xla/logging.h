#ifndef INTERFACE_TO_SYSTEMC_LOGGING_H
#define INTERFACE_TO_SYSTEMC_LOGGING_H


#include <iostream>
#include <vector>
#include <string>

#include "tensorflow/compiler/xla/xla.pb.h"
#include "tensorflow/compiler/xla/xla_data.pb.h"
#include "tensorflow/compiler/xla/literal_util.h"

// *************************************************
// Provide a interface to log tensorflow information
// *************************************************
class Logging {

  public:
    explicit Logging();
    virtual ~Logging();

    // *************************
    // Send - IPC Client Wrapper
    // *************************
    void SendParam(const xla::Literal& literal, xla::int64 count);
  
    // ****************************
    // Receive - IPC Server Wrapper
    // ****************************
    std::vector<float> RecvResult(const xla::Literal* literal);

    // *************************************
    // IPC Named Pipe
    //   Client - Pass image and kernel
    //   Server - Receive output feature map 
    // *************************************
    int IPC_Client(std::vector<float> param);
    std::vector<float> IPC_Server();

    // ****************************
    // IPC Share Memory Umimplement
    // ****************************
    int IPC_Shm_Client(std::vector<float> param);
    std::vector<float> IPC_Shm_Server();

};

#endif
