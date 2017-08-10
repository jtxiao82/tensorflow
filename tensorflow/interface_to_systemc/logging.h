#ifndef INTERFACE_TO_SYSTEMC_LOGGING_H
#define INTERFACE_TO_SYSTEMC_LOGGING_H


#include <iostream>
#include <vector>
#include <string>

#include "tensorflow/compiler/xla/xla.pb.h"
#include "tensorflow/compiler/xla/xla_data.pb.h"


// Provide a interface to log tensorflow information
class Logging {

  public:
    explicit Logging();
    virtual ~Logging();

    // Log all parameters
    void DumpParam(const xla::Literal& literal);
  
    // Log all constants
    void DumpConst(const xla::Literal& literal);

    // Log Ops
    void DumpOp(xla::BinaryOperation binop);

    // IPC Client. Pass the parameter to Chisel hardware wrapper by systemc.
    void IPC_Client(std::vector<float> param);

    std::vector<double> IPC_Server();

    // Log Ptx assembly code
    void DumpPtx(const char* ptx);
 
    // Log Computation shape
    void DumpShape(std::string str);

    // Monitor CUDA Get Kernel
    void Monitor(std::string str);

};






#endif
