licenses(["restricted"])

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "plugin_lib",
    srcs = glob([
        "*.cc",
    ]),
    hdrs = glob([
        "*.h",
    ]),
    deps = [
        "//tensorflow/compiler/jit:xla_device",
        "//tensorflow/compiler/jit:xla_jit_headers_lib",
        "//tensorflow/compiler/tf2xla:xla_compiler",
        "//tensorflow/compiler/xla:xla_headers_lib",
        "//tensorflow/compiler/xla/service",
        "//third_party/eigen3",
        "@local_config_cuda//cuda:cuda_headers",
        "@protobuf//:protobuf_headers",
        "//tensorflow/compiler/xla:logging", # jtxiao
    ],
)

filegroup(
    name = "all_files",
    srcs = glob(
        ["**/*"],
        exclude = [
            "**/METADATA",
            "**/OWNERS",
        ],
    ),
)
