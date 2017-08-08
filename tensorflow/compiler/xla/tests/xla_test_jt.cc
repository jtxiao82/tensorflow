#include <vector>
#include <iostream>

// IPC Test
/*#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>*/
// End

#include "tensorflow/compiler/xla/client/computation_builder.h"
#include "tensorflow/compiler/xla/client/computation.h"
#include "tensorflow/compiler/xla/client/local_client.h"
#include "tensorflow/compiler/xla/client/client.h"
#include "tensorflow/compiler/xla/client/client_library.h"
#include "tensorflow/compiler/xla/client/global_data.h"
#include "tensorflow/compiler/xla/legacy_flags/cpu_compiler_flags.h"
#include "tensorflow/compiler/xla/tests/client_library_test_base.h"
#include "tensorflow/compiler/xla/tests/literal_test_util.h"
#include "tensorflow/compiler/xla/tests/test_macros.h"
#include "tensorflow/core/platform/test.h"


int main(int argc, char** argv) {
  std::vector<tensorflow::Flag> flag_list;
  xla::legacy_flags::AppendCpuCompilerFlags(&flag_list);
  xla::string usage = tensorflow::Flags::Usage(argv[0], flag_list);
  const bool parse_result = tensorflow::Flags::Parse(&argc, argv, flag_list);
  if (!parse_result) {
    LOG(ERROR) << "\n" << usage;
    return 2;
  }
  testing::InitGoogleTest(&argc, argv);
  if (argc > 1) {
    LOG(ERROR) << "Unknown argument " << argv[1] << "\n" << usage;
    return 2;
  }
  // Modified
  // Open ../service/cpu/sample_harness.cc to compare codes
  // Open BUILD file to modified

  // Backup  
  // Go to tensorflow official website 
  // Find version "r1.0" 
  // "The following codes are at the same dir(tensorflow/tensorflow/compier/xla/test)"
  // Find file axpy_simple_test.cc
  // Find file BUILD


  xla::Client* client(xla::ClientLibrary::LocalClientOrDie());
  // Transfer parameter
  std::unique_ptr<xla::Literal> alpha_literal =
      xla::LiteralUtil::CreateR1<float>({3.1415926535f});
  std::unique_ptr<xla::GlobalData> alpha_data =
      client->TransferToServer(*alpha_literal).ConsumeValueOrDie();

  std::unique_ptr<xla::Literal> x_literal =
      xla::LiteralUtil::CreateR1<float>({-1.0, 1.0, 2.0, -2.0, -3.0, 3.0, 4.0, -4.0, -5.0, 5.0});
  std::unique_ptr<xla::GlobalData> x_data =
      client->TransferToServer(*x_literal).ConsumeValueOrDie();

  // Build computation
  
  xla::ComputationBuilder builder_(client, "ax");
  auto p0 = builder_.Parameter(0, alpha_literal->shape(), "param0");
  auto p1 = builder_.Parameter(1, x_literal->shape(), "param1");
  auto Ax = builder_.Mul(p1, p0);

  xla::StatusOr<xla::Computation> computation_status = builder_.Build();
  xla::Computation computation = computation_status.ConsumeValueOrDie();

  // Execute and transfer result of computation.
  //std::cout << "Execute and transfer" << std::endl;
  xla::ExecutionProfile profile;
  xla::StatusOr<std::unique_ptr<xla::Literal>> result =
      client->ExecuteAndTransfer(
          computation,
          /*arguments=*/{alpha_data.get(), x_data.get()},
          /*execution_options=*/nullptr,
          /*execution_profile=*/&profile);
  std::unique_ptr<xla::Literal> actual = result.ConsumeValueOrDie();

  LOG(INFO) << tensorflow::strings::Printf("computation took %lldns",
                                           profile.compute_time_ns());
  LOG(INFO) << xla::LiteralUtil::ToString(*actual);
  
  std::cout << xla::LiteralUtil::ToString(*actual) << std::endl;

  return 0;
}
