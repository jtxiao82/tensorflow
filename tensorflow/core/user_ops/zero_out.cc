#include "tensorflow/core/framework/op.h"
#include "tensorflow/core/framework/op_kernel.h"
#include "tensorflow/core/framework/shape_inference.h"


#include "tensorflow/interface_to_systemc/logging.h"
#include <iostream>
#include <vector>

using namespace tensorflow;

REGISTER_OP("ZeroOut")
    .Input("x: int32")
    .Input("y: int32")
    .Output("zeroed: int32")
    .SetShapeFn([](::tensorflow::shape_inference::InferenceContext* c) {
      c->set_output(0, c->input(0));
      return Status::OK();
    });

class ZeroOutOp : public OpKernel {
 public:
  explicit ZeroOutOp(OpKernelConstruction* context) : OpKernel(context) {}

  Logging* log;

  void Compute(OpKernelContext* context) override {
    // Grab the input tensor
    const Tensor& input_tensor = context->input(0);
    auto input = input_tensor.flat<int32>();

    // Grab the input tensor1
    const Tensor& input_tensor1 = context->input(1);
    auto input1 = input_tensor1.flat<int32>();

    std::vector<float> param; 
    std::vector<float> param1; 

    for(int i = 0; i< input.size(); i++) {
      //std::cout << input(i) << std::endl;
      param.push_back(input(i));
    }
    log->IPC_Client(param);

    for(int i = 0; i< input1.size(); i++) {
      //std::cout << input1(i) << std::endl;
      param1.push_back(input1(i));
    }
    log->IPC_Client(param1);

    std::vector<double> result;
    result = log->IPC_Server();

    // Create an output tensor
    Tensor* output_tensor = NULL;
    OP_REQUIRES_OK(context, context->allocate_output(0, input_tensor.shape(),
                                                     &output_tensor));
    auto output = output_tensor->flat<int32>();

    // Set all but the first element of the output tensor to 0.
    const int N = result.size();
    for (int i = 0; i < N; i++) {
      output(i) = result[i];
    }
  }
};

REGISTER_KERNEL_BUILDER(Name("ZeroOut").Device(DEVICE_CPU), ZeroOutOp);

