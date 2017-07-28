# Description:
# jtxiao's lib for TensorFlow.

licenses(["notice"])  # Apache 2.0

# -----------------------------------------------------------------------------
# Public targets

cc_library(
    name = "logging",
    srcs = ["logging.cc"],
    hdrs = ["logging.h"],
    deps = [
        "//tensorflow/compiler/xla:xla_data_proto",    
        "//tensorflow/compiler/xla:xla_proto",    
    ],
    visibility = ["//visibility:public"],
)

# -----------------------------------------------------------------------------
# Google-internal targets.

filegroup(
    name = "hdrs_files",
    srcs = glob(
        ["**/*.h"],
        exclude = [],
    ),
    visibility = ["//tensorflow:__subpackages__"],
)

filegroup(
    name = "srcs_files",
    srcs = glob(
        ["**/*.cc"],
        exclude = [],
    ),
    visibility = ["//tensorflow:__subpackages__"],
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
    visibility = ["//tensorflow:__subpackages__"],
)
