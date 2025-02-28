# GPU executor library for data-parallel kernel launches and cross-platform
# HPC-library APIs.
#
# Throughout this file, all targets are built with the standard crosstool and
# do not link against restricted binary blobs.

load("//xla:xla.bzl", "xla_cc_test")
load("//xla/stream_executor:build_defs.bzl", "stream_executor_friends", "stream_executor_internal")
load("@tsl//tsl:tsl.bzl", "set_external_visibility", "transitive_hdrs")
load("@tsl//tsl:tsl.default.bzl", "filegroup")
load("@tsl//tsl/platform:build_config.bzl", "tf_proto_library")
load("@tsl//tsl/platform:build_config_root.bzl", "if_static")
load("@tsl//tsl/platform:rules_cc.bzl", "cc_library")

package(
    # copybara:uncomment default_applicable_licenses = ["//tensorflow:license"],
    default_visibility = set_external_visibility([":friends"]),
    licenses = ["notice"],
)

# StreamExecutor clients that can depend on targets defined in stream_executor package.
package_group(
    name = "friends",
    packages = stream_executor_friends(),
)

# StreamExecutor platform-dependent implementations. We restrict visibility of all internal
# implementation interfaces to internal users (everything in `stream_executor::internal` namespace).
package_group(
    name = "internal",
    packages = stream_executor_internal(),
)

#===--------------------------------------------------------------------------------------------===#
# StreamExecutor public API
#===--------------------------------------------------------------------------------------------===#

# We bundle headers into filegroups for internal use only (we re-export the same set of headers
# from multiple targets), and all external clients should depend on one of the public `cc_library`
# targets that have dependencies required for compiling headers (e.g. absl dependencies). These
# filegroup roughly correspond to "StreamExecutor components" that are available to the clients.

# These are the headers that constitute StreamExecutor public API. Clients should not depend on
# this filegroup directly, but instead depend on a `stream_executor_headers` target if they need
# a header-only dependency (this is a very rare exception when we are building dynamic librarires
# for open source projects, e.g. Tensorflow, internally at Google we almost always link statically),
# or usually on a `sream_executor` target that will also link implementation.
filegroup(
    name = "stream_executor_public_headers",
    srcs = [
        "allocator_stats.h",
        "command_buffer.h",
        "data_type.h",
        "device_description.h",
        "device_id_utils.h",
        "device_memory.h",
        "device_memory_allocator.h",
        "device_options.h",
        "event.h",
        "executor_cache.h",
        "kernel.h",
        "kernel_cache_config.h",
        "kernel_spec.h",
        "launch_dim.h",
        "module_spec.h",
        "numeric_options.h",
        "platform.h",
        "plugin.h",
        "plugin_registry.h",
        "stream.h",
        "stream_executor.h",
        "stream_executor_internal.h",
        "stream_executor_pimpl.h",
        "temporary_device_memory.h",
        "temporary_memory_manager.h",
        "trace_listener.h",
    ],
    visibility = ["//visibility:private"],
)

# These are the headers for default StreamExecutor plugins.
filegroup(
    name = "stream_executor_plugin_headers",
    srcs = [
        "blas.h",
        "dnn.h",
        "fft.h",
    ],
    visibility = ["//visibility:private"],
)

#===--------------------------------------------------------------------------------------------===#
# StreamExecutor public libraries
#===--------------------------------------------------------------------------------------------===#

# Some of the StreamExecutor libraries that do not depend on StreamExecutor itself (Stream, Kernel,
# Event, etc.) exported as standalone libraries (these libraries should not depend on
# `stream_executor` and `stream_executor_headers` targets). This is mostly a historical artifact of
# an era when StreamExecutor was a part of Tensorflow.

# TODO(ezhulenev): Consider merging some (all?) of these libraries into StreamExecutor target, e.g.
# does it really make sense to have a separate `device_memory` library which is not usable without
# StreamExecutor.

tf_proto_library(
    name = "device_description_proto",
    srcs = ["device_description.proto"],
    cc_api_version = 2,
    make_default_target_header_only = True,
    protodeps = ["//xla:autotune_results_proto"],
)

cc_library(
    name = "device_description",
    srcs = ["device_description.cc"],
    hdrs = ["device_description.h"],
    deps = [
        ":device_description_proto_cc",
        ":launch_dim",
        "//xla/stream_executor/platform",
        "@com_google_absl//absl/algorithm:container",
        "@com_google_absl//absl/strings",
        "@tsl//tsl/lib/math:math_util",
        "@tsl//tsl/platform:statusor",
    ],
)

cc_library(
    name = "device_memory",
    hdrs = ["device_memory.h"],
    deps = ["//xla/stream_executor/platform"],
)

cc_library(
    name = "device_options",
    hdrs = ["device_options.h"],
    deps = [
        "//xla/stream_executor/platform",
        "@com_google_absl//absl/log:check",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "device_id_utils",
    hdrs = ["device_id_utils.h"],
    deps = [
        ":platform",
        ":stream_executor",
        "@tsl//tsl/framework:device_id_impl",
    ],
)

cc_library(
    name = "host_or_device_scalar",
    hdrs = ["host_or_device_scalar.h"],
    deps = [":device_memory"],
)

cc_library(
    name = "launch_dim",
    hdrs = ["launch_dim.h"],
    deps = ["@com_google_absl//absl/strings"],
)

cc_library(
    name = "numeric_options",
    hdrs = ["numeric_options.h"],
)

cc_library(
    name = "platform",
    srcs = ["platform.cc"],
    hdrs = ["platform.h"],
    deps = [
        ":device_description",
        ":device_options",
        "//xla/stream_executor/platform",
        "@com_google_absl//absl/log:check",
        "@com_google_absl//absl/status",
        "@tsl//tsl/platform:status",
        "@tsl//tsl/platform:statusor",
    ],
)

cc_library(
    name = "plugin",
    hdrs = ["plugin.h"],
)

#===--------------------------------------------------------------------------------------------===#
# StreamExecutor platform-dependent interfaces
#===--------------------------------------------------------------------------------------------===#

# Only platform-dependent StreamExecutor implementations (e.g. StreamExecutor for GPUs) and targets
# defined by StreamExecutor itself (e.g. `event`, `kernel`, etc.) can depend on internal
# interfaces (interfaces that define platform-specific API).
#
# External clients of StreamExecutor should depend on `stream_executor` target (links StreamExecutor
# implementation in static build configuration), or a header only `stream_executor_headers`.

cc_library(
    name = "stream_executor_internal",
    hdrs = ["stream_executor_internal.h"],
    visibility = [":internal"],
    deps = [
        ":stream_executor_headers",
        "//xla/stream_executor/platform",
        "@com_google_absl//absl/functional:any_invocable",
        "@com_google_absl//absl/status",
        "@tsl//tsl/platform:status",
        "@tsl//tsl/platform:statusor",
    ],
)

#===--------------------------------------------------------------------------------------------===#
# Exporting headers for Tensorflow
#===--------------------------------------------------------------------------------------------===#

# Tensorflow device memory allocators are aliases for StreamExecutor allocators, we export
# headers for Tensorflow to build shared libraries in OSS.

filegroup(
    name = "device_mem_allocator_headers",
    srcs = [
        "device_host_allocator.h",
        "device_mem_allocator.h",
    ],
    visibility = ["//tensorflow/core:__pkg__"],
)

#===--------------------------------------------------------------------------------------------===#
# StreamExecutor implementation
#===--------------------------------------------------------------------------------------------===#

# Targets that implement StreamExecutor APIs are private, and should not be used outside of
# `stream_executor` package. Clients should depend on `stream_executor` (headers and
# implementation) or `stream_executor_headers` (only headers, if there is a reason not to link
# implementation) if they want to use StreamExecutor.

cc_library(
    name = "command_buffer",
    srcs = ["command_buffer.cc"],
    hdrs = ["command_buffer.h"],
    visibility = ["//visibility:private"],
    deps = [
        ":stream_executor_internal",
        "//xla/stream_executor/platform",
        "@com_google_absl//absl/status:statusor",
        "@tsl//tsl/platform:statusor",
    ],
)

cc_library(
    name = "event",
    srcs = ["event.cc"],
    hdrs = ["event.h"],
    visibility = ["//visibility:private"],
    deps = [
        ":stream_executor_headers",
        ":stream_executor_internal",
        "//xla/stream_executor/platform",
        "@tsl//tsl/platform:status",
    ],
)

cc_library(
    name = "executor_cache",
    srcs = ["executor_cache.cc"],
    hdrs = ["executor_cache.h"],
    visibility = ["//visibility:private"],
    deps = [
        ":platform",
        ":stream_executor_headers",
        "//xla/stream_executor/platform",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/container:node_hash_map",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/log:check",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/synchronization",
        "@tsl//tsl/platform:statusor",
    ],
)

cc_library(
    name = "kernel_spec",
    srcs = ["kernel_spec.cc"],
    hdrs = ["kernel_spec.h"],
    visibility = ["//visibility:private"],
    deps = [
        "//xla/stream_executor/platform",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/log:check",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/synchronization",
        "@tsl//tsl/platform:logging",
    ],
)

cc_library(
    name = "kernel",
    srcs = ["kernel.cc"],
    hdrs = ["kernel.h"],
    visibility = ["//visibility:private"],
    deps = [
        ":device_memory",
        ":platform",
        ":stream_executor_headers",
        "//xla/stream_executor/platform",
        "@com_google_absl//absl/log:check",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/types:span",
        "@tsl//tsl/platform:errors",
        "@tsl//tsl/platform:logging",
        "@tsl//tsl/platform:platform_port",
        "@tsl//tsl/platform:status",
        "@tsl//tsl/platform:statusor",
    ],
)

#===--------------------------------------------------------------------------------------------===#

# The stream_executor_headers target does not prescribe an implementation.
cc_library(
    name = "stream_executor_headers",
    textual_hdrs = [
        "allocator_stats.h",
        "blas.h",
        "command_buffer.h",
        "device_description.h",
        "device_memory.h",
        "device_memory_allocator.h",
        "device_options.h",
        "device_id_utils.h",
        "dnn.h",
        "event.h",
        "executor_cache.h",
        "fft.h",
        "kernel.h",
        "kernel_cache_config.h",
        "kernel_spec.h",
        "data_type.h",
        "launch_dim.h",
        "numeric_options.h",
        "module_spec.h",
        "multi_platform_manager.h",
        "platform.h",
        "plugin.h",
        "plugin_registry.h",
        "stream.h",
        "stream_executor.h",
        "stream_executor_internal.h",
        "stream_executor_pimpl.h",
        "temporary_device_memory.h",
        "temporary_memory_manager.h",
        "trace_listener.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":allocator_stats",
        ":device_description_proto_cc",
        ":dnn_proto_cc",
        ":host_or_device_scalar",
        "//xla/stream_executor/platform",
        "@com_google_absl//absl/algorithm:container",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/container:node_hash_map",
        "@com_google_absl//absl/functional:any_invocable",
        "@com_google_absl//absl/log:check",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/types:optional",
        "@com_google_absl//absl/types:span",
        "@tsl//tsl/framework:device_id",
        "@tsl//tsl/framework:device_type",
        "@tsl//tsl/platform:env",
        "@tsl//tsl/platform:errors",
        "@tsl//tsl/platform:float8",
        "@tsl//tsl/platform:logging",
        "@tsl//tsl/platform:status",
        "@tsl//tsl/platform:statusor",
        "@tsl//tsl/platform:types",
    ],
)

transitive_hdrs(
    name = "stream_executor_install_hdrs",
    deps = [":stream_executor_headers"],
)

cc_library(
    name = "allocator_stats",
    srcs = ["allocator_stats.cc"],
    hdrs = ["allocator_stats.h"],
    deps = [
        "//xla/stream_executor/platform",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/types:optional",
    ],
)

tf_proto_library(
    name = "dnn_proto",
    srcs = ["dnn.proto"],
    make_default_target_header_only = True,
    protodeps = ["@tsl//tsl/protobuf:dnn_proto"],
    exports = ["@tsl//tsl/protobuf:dnn_proto"],
)

cc_library(
    name = "fft",
    hdrs = ["fft.h"],
    deps = [
        "//xla/stream_executor/platform",
    ],
)

cc_library(
    name = "device_mem_allocator",
    hdrs = [
        "device_host_allocator.h",
        "device_mem_allocator.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":stream_executor",
        "@tsl//tsl/framework:allocator",
        "@tsl//tsl/framework:device_id",
        "@tsl//tsl/platform:macros",
        "@tsl//tsl/profiler/lib:traceme",
    ],
)

cc_library(
    name = "multi_platform_manager",
    srcs = ["multi_platform_manager.cc"],
    hdrs = ["multi_platform_manager.h"],
    deps = [
        ":platform",
        ":stream_executor_headers",
        "//xla/stream_executor/platform",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/synchronization",
        "@tsl//tsl/platform:errors",
        "@tsl//tsl/platform:status",
        "@tsl//tsl/platform:statusor",
    ],
)

cc_library(
    name = "stream_executor_pimpl_header",
    hdrs = [
        "device_description.h",
        "kernel.h",
        "kernel_cache_config.h",
        "stream_executor_pimpl.h",
    ],
    visibility = ["//visibility:private"],
    deps = [
        ":device_description_proto_cc",
        ":platform",
        ":stream_executor_headers",
        ":stream_executor_internal",
        "//xla/stream_executor/platform",
        "@com_google_absl//absl/algorithm:container",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/functional:any_invocable",
        "@com_google_absl//absl/log:check",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/types:optional",
        "@com_google_absl//absl/types:span",
        "@tsl//tsl/platform:env",
        "@tsl//tsl/platform:logging",
        "@tsl//tsl/platform:status",
        "@tsl//tsl/platform:statusor",
        "@tsl//tsl/protobuf:dnn_proto_cc",
    ],
)

# It implements :stream_executor_pimpl_header
cc_library(
    name = "stream_executor_pimpl",
    srcs = [
        "stream.cc",
        "stream_executor_pimpl.cc",
    ],
    hdrs = ["stream_executor_pimpl.h"],
    visibility = ["//visibility:private"],
    deps = [
        ":blas",
        ":command_buffer",
        ":device_memory",
        ":dnn",
        ":event",
        ":executor_cache",
        ":fft",
        ":host_or_device_scalar",
        ":kernel",
        ":kernel_spec",
        ":launch_dim",
        ":platform",
        ":plugin_registry",
        ":stream_executor_headers",
        ":stream_executor_internal",
        ":temporary_device_memory",
        ":temporary_memory_manager",
        "//xla/stream_executor/platform",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/functional:any_invocable",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/types:optional",
        "@com_google_absl//absl/types:span",
        "@eigen_archive//:eigen3",
        "@tsl//tsl/platform:env",
        "@tsl//tsl/platform:errors",
        "@tsl//tsl/platform:logging",
        "@tsl//tsl/platform:stacktrace",
        "@tsl//tsl/platform:status",
        "@tsl//tsl/platform:statusor",
        "@tsl//tsl/protobuf:dnn_proto_cc",
        "@tsl//tsl/util:env_var",
    ],
)

cc_library(
    name = "blas",
    srcs = ["blas.cc"],
    hdrs = ["blas.h"],
    deps = [
        ":stream_executor_headers",
        "//xla/stream_executor/platform",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/types:span",
        "@tsl//tsl/platform:statusor",
        "@tsl//tsl/protobuf:dnn_proto_cc",
    ],
)

cc_library(
    name = "plugin_registry",
    srcs = ["plugin_registry.cc"],
    hdrs = ["plugin_registry.h"],
    deps = [
        ":blas",
        ":dnn",
        ":fft",
        ":multi_platform_manager",
        ":platform",
        ":stream_executor_headers",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/synchronization",
        "@tsl//tsl/platform:errors",
        "@tsl//tsl/platform:status",
        "@tsl//tsl/platform:statusor",
    ],
)

cc_library(
    name = "dnn",
    srcs = ["dnn.cc"],
    hdrs = ["dnn.h"],
    deps = [
        ":device_description_proto_cc",
        ":device_memory",
        ":dnn_proto_cc",
        ":numeric_options",
        ":stream_executor_headers",
        "//xla/stream_executor/platform",
        "@com_google_absl//absl/algorithm:container",
        "@com_google_absl//absl/container:btree",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/types:optional",
        "@com_google_absl//absl/types:span",
        "@tsl//tsl/lib/strings:proto_serialization",
        "@tsl//tsl/platform:logging",
        "@tsl//tsl/platform:status",
        "@tsl//tsl/platform:statusor",
        "@tsl//tsl/protobuf:dnn_proto_cc",
    ] + if_static(["@com_google_protobuf//:protobuf"]),
)

cc_library(
    name = "temporary_memory_manager",
    srcs = ["temporary_memory_manager.cc"],
    hdrs = ["temporary_memory_manager.h"],
    deps = [
        ":device_memory",
        ":stream_executor_headers",
        ":temporary_device_memory",
        "//xla/stream_executor/platform",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/synchronization",
        "@tsl//tsl/platform:logging",
        "@tsl//tsl/platform:status",
        "@tsl//tsl/platform:statusor",
    ],
)

cc_library(
    name = "temporary_device_memory",
    srcs = [
        "event.h",
        "temporary_device_memory.cc",
        "temporary_memory_manager.h",
    ],
    hdrs = ["temporary_device_memory.h"],
    deps = [
        ":device_memory",
        ":stream_executor_headers",
        "//xla/stream_executor/platform",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/synchronization",
        "@tsl//tsl/platform:status",
        "@tsl//tsl/platform:statusor",
    ],
)

cc_library(
    name = "stream_executor",
    textual_hdrs = [
        "allocator_stats.h",
        "blas.h",
        "data_type.h",
        "device_description.h",
        "device_memory.h",
        "device_memory_allocator.h",
        "device_options.h",
        "device_id_utils.h",
        "dnn.h",
        "event.h",
        "executor_cache.h",
        "fft.h",
        "kernel.h",
        "kernel_cache_config.h",
        "kernel_spec.h",
        "launch_dim.h",
        "module_spec.h",
        "multi_platform_manager.h",
        "platform.h",
        "plugin.h",
        "plugin_registry.h",
        "stream.h",
        "numeric_options.h",
        "stream_executor.h",
        "stream_executor_internal.h",
        "stream_executor_pimpl.h",
        "temporary_device_memory.h",
        "temporary_memory_manager.h",
        "trace_listener.h",
    ],
    deps = [
        ":stream_executor_headers",
        "@com_google_absl//absl/log:check",
        "@tsl//tsl/framework:device_id",
        "@tsl//tsl/framework:device_type",
        "@tsl//tsl/platform:errors",
        "@tsl//tsl/platform:logging",
        "@tsl//tsl/platform:status",
        "@tsl//tsl/platform:types",
    ] + if_static([":stream_executor_impl"]),
)

cc_library(
    name = "stream_executor_impl",
    deps = [
        ":allocator_stats",
        ":device_description",
        ":device_memory",
        ":dnn",
        ":dnn_proto_cc",
        ":event",
        ":executor_cache",
        ":kernel",
        ":kernel_spec",
        ":launch_dim",
        ":multi_platform_manager",
        ":platform",
        ":stream_executor_headers",
        ":stream_executor_pimpl",
        "//xla/stream_executor:dnn_proto_cc_impl",
        "@tsl//tsl/protobuf:dnn_proto_cc_impl",
    ],
)

cc_library(
    name = "device_memory_allocator",
    hdrs = ["device_memory_allocator.h"],
    deps = [
        ":device_memory",
        ":platform",
        ":stream_executor",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/types:span",
        "@tsl//tsl/platform:errors",
        "@tsl//tsl/platform:status",
        "@tsl//tsl/platform:statusor",
        "@tsl//tsl/platform:types",
    ],
)

cc_library(
    name = "scratch_allocator",
    srcs = ["scratch_allocator.cc"],
    hdrs = ["scratch_allocator.h"],
    deps = [
        ":device_memory",
        ":stream_executor_headers",
        "//xla/stream_executor/platform",
        "@com_google_absl//absl/container:inlined_vector",
        "@com_google_absl//absl/log:check",
        "@tsl//tsl/platform:statusor",
    ],
)

xla_cc_test(
    name = "stream_test",
    size = "small",
    srcs = ["stream_test.cc"],
    deps = [
        ":stream_executor",
        "//xla/stream_executor/host:host_platform",
        "@tsl//tsl/platform:test",
        "@tsl//tsl/platform:test_main",
    ],
)

xla_cc_test(
    name = "dnn_test",
    size = "small",
    srcs = ["dnn_test.cc"],
    deps = [
        ":dnn",
        "@tsl//tsl/platform:test",
        "@tsl//tsl/platform:test_main",
    ],
)

cc_library(
    name = "tf_allocator_adapter",
    srcs = ["tf_allocator_adapter.cc"],
    hdrs = ["tf_allocator_adapter.h"],
    deps = [
        ":device_memory",
        ":device_memory_allocator",
        ":platform",
        ":stream_executor_headers",
        "@com_google_absl//absl/log:check",
        "@com_google_absl//absl/synchronization",
        "@tsl//tsl/framework:allocator",
        "@tsl//tsl/platform:errors",
        "@tsl//tsl/platform:statusor",
    ],
)

cc_library(
    name = "lazy_op_runner",
    hdrs = ["lazy_op_runner.h"],
    deps = [
        ":dnn",
        ":stream_executor_headers",
        "@com_google_absl//absl/base",
    ],
)

exports_files(["lazy_op_runner.h"])

alias(
    name = "cuda_platform",
    actual = "//xla/stream_executor/cuda:all_runtime",
)

alias(
    name = "rocm_platform",
    actual = "//xla/stream_executor/rocm:all_runtime",
)

# TODO(se-owner): document or remove this.
cc_library(
    name = "stream_executor_bundle",
    deps = [
        ":dnn",
        ":event",
        ":multi_platform_manager",
        ":scratch_allocator",
        ":stream_executor",
        "//xla/stream_executor/cuda:cuda_platform_id",
        "//xla/stream_executor/host:host_platform_id",
        "//xla/stream_executor/platform:dso_loader",
        "//xla/stream_executor/rocm:rocm_platform_id",
    ],
)
