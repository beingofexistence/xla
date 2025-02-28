load("//xla:pytype.default.bzl", "pytype_strict_library")
load("//xla:strict.default.bzl", "py_strict_library", "py_strict_test")
load("@bazel_skylib//rules:common_settings.bzl", "bool_flag")
load("@local_config_cuda//cuda:build_defs.bzl", "if_cuda")
load(
    "//xla:xla.bzl",
    "xla_cc_test",
    "xla_py_test_deps",
)
load("@local_config_rocm//rocm:build_defs.bzl", "if_rocm")
load(
    "@tsl//tsl:tsl.bzl",
    "if_cuda_or_rocm",
)
load("@tsl//tsl:tsl.default.bzl", "tsl_pybind_extension")
load("@tsl//tsl/platform:build_config.bzl", "pyx_library", "tf_proto_library")
load("@tsl//tsl/platform:rules_cc.bzl", "cc_library")
load(
    "@tsl//tsl/platform/default:cuda_build_defs.bzl",
    "if_cuda_is_configured",
)

package(
    # copybara:uncomment default_applicable_licenses = ["//tensorflow:license"],
    default_visibility = [":friends"],
    licenses = ["notice"],
)

package_group(
    name = "friends",
    includes = [
        "//xla:friends",
        "//xla:internal",
    ],
)

pytype_strict_library(
    name = "xla_client",
    srcs = ["xla_client.py"],
    srcs_version = "PY3",
    visibility = ["//visibility:public"],
    deps = [
        ":xla_extension",
        "@ml_dtypes",
    ],
)

exports_files(["xla_client.pyi"])

pyx_library(
    name = "custom_call_for_test",
    testonly = True,
    srcs = ["custom_call_for_test.pyx"],
)

py_strict_test(
    name = "xla_client_backend_independent_test",
    srcs = ["xla_client_backend_independent_test.py"],
    python_version = "PY3",
    tags = ["no_oss"],  # TODO(phawkins): This test passes, but requires --config=monolithic.
    deps = [
        ":xla_client",
        ":xla_extension",
        "@absl_py//absl/testing:absltest",
    ] + xla_py_test_deps(),
)

py_strict_library(
    name = "xla_client_test",
    testonly = 1,
    srcs = ["xla_client_test.py"],
    srcs_version = "PY3",
    visibility = [":friends"],
    deps = [
        ":xla_client",
        "@absl_py//absl/flags",
        "@absl_py//absl/logging",
        "@absl_py//absl/testing:absltest",
        "@absl_py//absl/testing:parameterized",
    ],
)

py_strict_test(
    name = "xla_client_test_cpu",
    srcs = ["xla_client_test.py"],
    args = ["--backend=cpu"],
    main = "xla_client_test.py",
    python_version = "PY3",
    srcs_version = "PY3",
    tags = [
        "no_oss",
        "not_run:arm",
    ],  # TODO(phawkins): This test passes, but requires --config=monolithic.
    deps = [
        ":custom_call_for_test",
        ":xla_client",
        ":xla_extension",
        "@absl_py//absl/flags",
        "@absl_py//absl/logging",
        "@absl_py//absl/testing:absltest",
        "@absl_py//absl/testing:parameterized",
    ] + xla_py_test_deps(),
)

py_strict_test(
    name = "weakref_lru_cache_test",
    srcs = ["weakref_lru_cache_test.py"],
    python_version = "PY3",
    srcs_version = "PY3",
    tags = ["no_oss"],  # TODO(phawkins): This test passes, but requires --config=monolithic.
    deps = [
        ":xla_client",
        ":xla_extension",
        "@absl_py//absl/flags",
        "@absl_py//absl/logging",
        "@absl_py//absl/testing:absltest",
    ] + xla_py_test_deps(),
)

py_strict_test(
    name = "xla_client_test_gpu",
    srcs = ["xla_client_test.py"],
    args = ["--backend=gpu"],
    main = "xla_client_test.py",
    python_version = "PY3",
    srcs_version = "PY3",
    tags = [
        "config-cuda-only",
        "no_oss",
        "requires-gpu-nvidia",
    ],  # TODO(phawkins): This test passes, but requires --config=monolithic.
    deps = [
        ":xla_client",
        ":xla_extension",
        "@absl_py//absl/flags",
        "@absl_py//absl/logging",
        "@absl_py//absl/testing:absltest",
        "@absl_py//absl/testing:parameterized",
    ] + xla_py_test_deps(),
)

cc_library(
    name = "status_casters",
    hdrs = ["status_casters.h"],
    compatible_with = [],
    copts = [
        "-fexceptions",
        "-fno-strict-aliasing",
    ],
    features = ["-use_header_modules"],
    deps = [
        ":exceptions",
        "//xla:status",
        "//xla:statusor",
        "@pybind11",
        "@tsl//tsl/platform:macros",
    ],
)

tsl_pybind_extension(
    name = "status_casters_ext",
    srcs = ["status_casters_ext.cc"],
    visibility = ["//visibility:private"],
    deps = [
        ":exceptions",
        ":status_casters",
        "@pybind11",
    ],
)

py_strict_test(
    name = "status_casters_test",
    srcs = ["status_casters_test.py"],
    main = "status_casters_test.py",
    python_version = "PY3",
    srcs_version = "PY3",
    tags = ["no_oss"],
    deps = [
        ":status_casters_ext",
        "@absl_py//absl/testing:absltest",
    ] + xla_py_test_deps(),
)

cc_library(
    name = "exceptions",
    hdrs = ["exceptions.h"],
    copts = [
        "-fexceptions",
        "-fno-strict-aliasing",
    ],
    features = ["-use_header_modules"],
    deps = [
        "//xla:status",
        "@com_google_absl//absl/status",
    ],
)

cc_library(
    name = "types",
    srcs = ["types.cc"],
    hdrs = ["types.h"],
    compatible_with = [],
    copts = [
        "-fexceptions",
        "-fno-strict-aliasing",
    ],
    features = ["-use_header_modules"],
    visibility = [":friends"],
    deps = [
        ":exceptions",
        "//xla:literal",
        "//xla:shape_util",
        "//xla:status",
        "//xla:status_macros",
        "//xla:statusor",
        "//xla:types",
        "//xla:xla_data_proto_cc",
        "//xla/python/ifrt",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:inlined_vector",
        "@pybind11",
        "@pybind11_abseil//pybind11_abseil:absl_casters",
        "@tsl//tsl/platform:protobuf",
    ],
)

cc_library(
    name = "python_ref_manager",
    srcs = ["python_ref_manager.cc"],
    hdrs = ["python_ref_manager.h"],
    copts = [
        "-fexceptions",
        "-fno-strict-aliasing",
    ],
    features = ["-use_header_modules"],
    visibility = [":friends"],
    deps = [
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/container:inlined_vector",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/types:span",
        "@pybind11",
    ],
)

cc_library(
    name = "python_utils",
    hdrs = ["python_utils.h"],
    copts = [
        "-fexceptions",
        "-fno-strict-aliasing",
    ],
    features = ["-use_header_modules"],
    deps = [
        "//xla:status_macros",
        "//xla:util",
        "@local_config_python//:python_headers",  # buildcleaner: keep
        "@pybind11",
    ],
)

cc_library(
    name = "traceback",
    srcs = ["traceback.cc"],
    hdrs = ["traceback.h"],
    compatible_with = [],
    copts = [
        "-fexceptions",
        "-fno-strict-aliasing",
    ],
    features = ["-use_header_modules"],
    visibility = [":friends"],
    deps = [
        ":exceptions",
        ":python_ref_manager",
        # placeholder for index annotation deps
        "@com_google_absl//absl/container:inlined_vector",
        "@com_google_absl//absl/hash",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@tsl//tsl/platform:logging",
        "@pybind11",
    ],
)

cc_library(
    name = "pprof_profile_builder",
    srcs = ["pprof_profile_builder.cc"],
    hdrs = ["pprof_profile_builder.h"],
    compatible_with = [],
    copts = [
        "-fexceptions",
        "-fno-strict-aliasing",
    ],
    features = ["-use_header_modules"],
    deps = [
        ":traceback",
        "//xla:statusor",
        "//xla:util",
        "@com_google_absl//absl/container:flat_hash_map",
        "@pybind11",
        "@tsl//tsl/platform:protobuf",
        "@tsl//tsl/profiler/protobuf:profile_proto_cc",
    ],
)

cc_library(
    name = "py_client",
    srcs = [
        "py_array.cc",
        "py_buffer.cc",
        "py_client.cc",
        "py_compile_only_client.cc",
        "py_device_list.cc",
        "py_executable.cc",
        "py_host_callback.cc",
        "py_values.cc",
        "sharding.cc",
    ],
    hdrs = [
        "py_array.h",
        "py_buffer.h",
        "py_client.h",
        "py_compile_only_client.h",
        "py_device_list.h",
        "py_executable.h",
        "py_host_callback.h",
        "py_values.h",
        "sharded_device_array.h",
        "sharding.h",
    ],
    compatible_with = [],
    copts = [
        "-fexceptions",
        "-fno-strict-aliasing",
    ],
    defines = if_cuda(["GOOGLE_CUDA=1"]) + if_rocm([
        "TENSORFLOW_USE_ROCM=1",
    ]),
    features = ["-use_header_modules"],
    deps = [
        ":callback",
        ":exceptions",
        ":pprof_profile_builder",
        ":py_client_gpu",
        ":py_host_callback_proto_cc",
        ":python_ref_manager",
        ":python_utils",
        ":status_casters",
        ":traceback",
        ":transfer_guard_lib",
        ":types",
        ":util",
        # placeholder for index annotation deps
        "@com_google_absl//absl/algorithm:container",
        "@com_google_absl//absl/base",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/hash",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/types:span",
        "@com_google_absl//absl/types:variant",
        "//xla:comparison_util",
        "//xla:shape_util",
        "//xla:statusor",
        "//xla:types",
        "//xla:util",
        "//xla:xla_data_proto_cc",
        "//xla/client:xla_builder",
        "//xla/pjrt:host_callback",
        "//xla/pjrt:lru_cache",
        "//xla/pjrt:mlir_to_hlo",
        "//xla/pjrt:pjrt_client",
        "//xla/pjrt:pjrt_compiler",
        "//xla/pjrt:pjrt_future",
        "//xla/pjrt:pjrt_stream_executor_client",
        "//xla/pjrt:transpose",
        "//xla/python/ifrt",
        "//xla/python/pjrt_ifrt",
        "//xla/python/pjrt_ifrt:xla_host_callback_proto_cc",
        "//xla/python/pjrt_ifrt:xla_ifrt",
        "//xla/service:custom_call_status",
        "//xla/service:custom_call_target_registry",
        "//xla/service:platform_util",
        "@tsl//tsl/platform:errors",
        "@tsl//tsl/platform:fingerprint",
        "@tsl//tsl/platform:float8",
        "@tsl//tsl/platform:statusor",
        "@tsl//tsl/profiler/lib:traceme",
        "@tsl//tsl/python/lib/core:numpy",
        "@com_google_protobuf//:protobuf",
        "@llvm-project//llvm:Support",
        "@pybind11",
        "@pybind11_abseil//pybind11_abseil:absl_casters",
    ] + if_cuda([
        "@local_config_cuda//cuda:cuda_headers",
    ]) + if_rocm([
        "@local_config_rocm//rocm:rocm_headers",
    ]),
)

cc_library(
    name = "callback",
    srcs = [
        "callback.cc",
    ],
    hdrs = [
        "callback.h",
    ],
    copts = [
        "-fexceptions",
        "-fno-strict-aliasing",
    ],
    features = ["-use_header_modules"],
    deps = [
        ":python_ref_manager",
        "//xla:comparison_util",
        "//xla:xla_data_proto_cc",
        "//xla/pjrt:transpose",
        "//xla/service:custom_call_status",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/types:span",
        "@pybind11",
        "@tsl//tsl/platform:statusor",
    ],
)

cc_library(
    name = "py_client_gpu",
    srcs = if_cuda_or_rocm([
        "py_client_gpu.cc",
    ]),
    hdrs = if_cuda_or_rocm([
        "py_client_gpu.h",
    ]),
    copts = [
        "-fexceptions",
        "-fno-strict-aliasing",
    ],
    defines = if_cuda(["GOOGLE_CUDA=1"]) + if_rocm([
        "TENSORFLOW_USE_ROCM=1",
    ]),
    features = ["-use_header_modules"],
    deps = [
        ":callback",
        ":exceptions",
        "//xla:comparison_util",
        "//xla/service:custom_call_status",
        "@com_google_absl//absl/base",
        "@com_google_absl//absl/strings",
        "@pybind11",
        "@tsl//tsl/platform:errors",
    ] + if_cuda([
        "@local_config_cuda//cuda:cuda_headers",
    ]) + if_rocm([
        "@local_config_rocm//rocm:rocm_headers",
    ]),
)

cc_library(
    name = "dlpack",
    srcs = ["dlpack.cc"],
    hdrs = ["dlpack.h"],
    compatible_with = [],
    copts = [
        "-fexceptions",
        "-fno-strict-aliasing",
    ],
    features = ["-use_header_modules"],
    deps = [
        ":py_client",
        ":python_ref_manager",
        ":traceback",
        ":util",
        "//xla:types",
        "//xla:util",
        "//xla/pjrt:pjrt_client",
        "//xla/pjrt/gpu:se_gpu_pjrt_client",
        "@com_google_absl//absl/algorithm:container",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/types:span",
        "@dlpack",
        "@local_config_python//:python_headers",  # buildcleaner: keep
        "@pybind11",
    ],
)

cc_library(
    name = "jax_jit",
    srcs = ["jax_jit.cc"],
    hdrs = ["jax_jit.h"],
    compatible_with = [],
    copts = [
        "-fexceptions",
        "-fno-strict-aliasing",
    ],
    features = ["-use_header_modules"],
    visibility = [":friends"],  # For the functions to access C++ flags/thread-local variables
    deps = [
        ":exceptions",
        ":py_client",
        ":python_ref_manager",
        ":python_utils",
        ":pytree",
        ":status_casters",
        ":types",
        ":util",
        # placeholder for index annotation deps
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:inlined_vector",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/types:span",
        "@local_config_python//:python_headers",  # build_cleaner: keep
        "//xla:shape_util",
        "//xla:statusor",
        "//xla:types",
        "//xla:util",
        "//xla:xla_data_proto_cc",
        "//xla/pjrt:lru_cache",
        "//xla/pjrt:pjrt_client",
        "//xla/python/ifrt",
        "@tsl//tsl/platform:status",
        "@tsl//tsl/profiler/lib:traceme",
        "@pybind11",
    ],
)

cc_library(
    name = "inspect_sharding",
    srcs = ["inspect_sharding.cc"],
    hdrs = ["inspect_sharding.h"],
    deps = [
        "//xla/hlo/ir:hlo",
        "//xla/service:custom_call_sharding_helper",
        "//xla/service/spmd:spmd_partitioner",
    ],
    # Always register 'InspectSharding' custom partitioning handler.
    alwayslink = 1,
)

cc_library(
    name = "custom_call_sharding",
    srcs = ["custom_call_sharding.cc"],
    hdrs = ["custom_call_sharding.h"],
    compatible_with = [],
    copts = [
        "-fexceptions",
        "-fno-strict-aliasing",
    ],
    features = ["-use_header_modules"],
    visibility = ["//visibility:private"],
    deps = [
        ":inspect_sharding",
        ":status_casters",
        # placeholder for index annotation deps
        "//xla/client:xla_computation",
        "//xla/hlo/ir:hlo",
        "//xla/hlo/utils:hlo_sharding_util",
        "//xla/service:call_inliner",
        "//xla/service:custom_call_sharding_helper",
        "//xla/service:hlo_pass_pipeline",
        "//xla/service/spmd:spmd_partitioner",
        "@tsl//tsl/platform:errors",
        "@pybind11",
    ],
)

cc_library(
    name = "ops",
    srcs = ["ops.cc"],
    hdrs = ["ops.h"],
    compatible_with = [],
    copts = [
        "-fexceptions",
        "-fno-strict-aliasing",
    ],
    features = ["-use_header_modules"],
    deps = [
        ":status_casters",
        ":types",
        # placeholder for index annotation deps
        "@com_google_absl//absl/types:span",
        "//xla:xla_data_proto_cc",
        "//xla/client:xla_builder",
        "//xla/client:xla_computation",
        "//xla/client/lib:approx_topk",
        "//xla/client/lib:approx_topk_shape",
        "//xla/client/lib:comparators",
        "//xla/client/lib:lu_decomposition",
        "//xla/client/lib:math",
        "//xla/client/lib:qr",
        "//xla/client/lib:self_adjoint_eig",
        "//xla/client/lib:sorting",
        "//xla/client/lib:svd",
        "@pybind11",
    ],
)

cc_library(
    name = "outfeed_receiver",
    srcs = ["outfeed_receiver.cc"],
    hdrs = ["outfeed_receiver.h"],
    deps = [
        "//xla:literal",
        "//xla:shape_util",
        "//xla:statusor",
        "//xla:util",
        "//xla/client:executable_build_options",
        "//xla/client:sharding_builder",
        "//xla/client:xla_builder",
        "//xla/client:xla_computation",
        "//xla/pjrt:pjrt_client",
        "//xla/pjrt:pjrt_executable",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/strings:str_format",
        "@tsl//tsl/profiler/lib:traceme",
    ],
)

cc_library(
    name = "pjit",
    srcs = ["pjit.cc"],
    hdrs = ["pjit.h"],
    compatible_with = [],
    copts = [
        "-fexceptions",
        "-fno-strict-aliasing",
    ],
    features = ["-use_header_modules"],
    visibility = ["//visibility:private"],
    deps = [
        ":jax_jit",
        ":py_client",
        ":python_utils",
        ":pytree",
        ":status_casters",
        ":transfer_guard_lib",
        ":util",
        # placeholder for index annotation deps
        "@com_google_absl//absl/synchronization",
        "//xla/pjrt:lru_cache",
        "//xla/python/ifrt",
        "//xla/python/pjrt_ifrt",
        "@tsl//tsl/platform:errors",
        "@tsl//tsl/profiler/lib:traceme",
        "@pybind11",
    ],
)

cc_library(
    name = "pmap_lib",
    srcs = ["pmap_lib.cc"],
    hdrs = ["pmap_lib.h"],
    compatible_with = [],
    copts = [
        "-fexceptions",
        "-fno-strict-aliasing",
    ],
    features = ["-use_header_modules"],
    visibility = ["//visibility:private"],
    deps = [
        ":exceptions",
        ":jax_jit",
        ":py_client",
        ":python_utils",
        ":pytree",
        ":status_casters",
        ":types",
        ":util",
        # placeholder for index annotation deps
        "@com_google_absl//absl/hash",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/types:span",
        "@com_google_absl//absl/types:variant",
        "//xla:xla_data_proto_cc",
        "//xla/pjrt:pjrt_client",
        "//xla/python/ifrt",
        "//xla/python/pjrt_ifrt",
        "@tsl//tsl/platform:logging",
        "@tsl//tsl/platform:statusor",
        "@tsl//tsl/profiler/lib:traceme",
        "@pybind11",
        "@pybind11_abseil//pybind11_abseil:absl_casters",
    ],
)

xla_cc_test(
    name = "outfeed_receiver_test_cpu",
    size = "small",
    srcs = ["outfeed_receiver_test.cc"],
    deps = [
        ":outfeed_receiver",
        "//xla:test",
        "//xla/client:client_library",
        "//xla/client:executable_build_options",
        "//xla/client:xla_builder",
        "//xla/pjrt:pjrt_client",
        "//xla/pjrt:pjrt_stream_executor_client",
        "//xla/pjrt:tfrt_cpu_pjrt_client",
        "//xla/service:platform_util",
        "@com_google_absl//absl/synchronization",
        "@tsl//tsl/platform:test_main",
    ],
)

cc_library(
    name = "outfeed_receiver_py",
    srcs = ["outfeed_receiver_py.cc"],
    hdrs = ["outfeed_receiver_py.h"],
    compatible_with = [],
    copts = [
        "-fexceptions",
        "-fno-strict-aliasing",
    ],
    features = ["-use_header_modules"],
    deps = [
        ":outfeed_receiver",
        ":py_client",
        ":status_casters",
        ":types",
        # placeholder for index annotation deps
        "@com_google_absl//absl/algorithm:container",
        "@com_google_absl//absl/synchronization",
        "//xla/client:executable_build_options",
        "//xla/client:xla_builder",
        "//xla/pjrt:pjrt_client",
        "@pybind11",
    ],
)

py_strict_test(
    name = "pytree_test",
    srcs = ["pytree_test.py"],
    python_version = "PY3",
    srcs_version = "PY3",
    tags = ["no_oss"],  # TODO(phawkins): This test passes, but requires --config=monolithic.
    deps = [
        ":xla_client",
        ":xla_extension",
        "@absl_py//absl/flags",
        "@absl_py//absl/logging",
        "@absl_py//absl/testing:absltest",
    ] + xla_py_test_deps(),
)

tf_proto_library(
    name = "pytree_proto",
    srcs = ["pytree.proto"],
    cc_api_version = 2,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "pytree",
    srcs = ["pytree.cc"],
    hdrs = ["pytree.h"],
    compatible_with = [],
    copts = [
        "-fexceptions",
        "-fno-strict-aliasing",
    ],
    features = ["-use_header_modules"],
    visibility = [":friends"],
    deps = [
        ":exceptions",
        ":pytree_proto_cc",
        # placeholder for index annotation deps
        "@com_google_absl//absl/algorithm:container",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:inlined_vector",
        "@com_google_absl//absl/hash",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@tsl//tsl/platform:logging",
        "@pybind11",
        "@pybind11_abseil//pybind11_abseil:absl_casters",
    ],
)

cc_library(
    name = "mlir",
    srcs = ["mlir.cc"],
    hdrs = ["mlir.h"],
    compatible_with = [],
    copts = [
        "-fexceptions",
        "-fno-strict-aliasing",
    ],
    features = ["-use_header_modules"],
    deps = [
        ":refine_polymorphic_shapes",
        ":status_casters",
        ":types",
        # placeholder for index annotation deps
        "//xla:status",
        "//xla/client:xla_computation",
        "//xla/mlir/utils:error_util",
        "//xla/mlir_hlo",
        "//xla/mlir_hlo:all_passes",
        "//xla/pjrt:mlir_to_hlo",
        "//xla/service/llvm_ir:llvm_util",
        "//xla/translate/hlo_to_mhlo:hlo_to_mlir_hlo",
        "@tsl//tsl/platform:errors",
        "@llvm-project//llvm:Support",
        "@llvm-project//mlir:FuncDialect",
        "@llvm-project//mlir:FuncExtensions",
        "@llvm-project//mlir:IR",
        "@llvm-project//mlir:Parser",
        "@llvm-project//mlir:Pass",
        "@llvm-project//mlir:ReconcileUnrealizedCasts",
        "@llvm-project//mlir:SparseTensorDialect",
        "@pybind11",
        "@stablehlo//:chlo_ops",
        "@stablehlo//:stablehlo_ops",
        "@stablehlo//:stablehlo_serialization",
    ],
)

cc_library(
    name = "refine_polymorphic_shapes",
    srcs = ["refine_polymorphic_shapes.cc"],
    hdrs = ["refine_polymorphic_shapes.h"],
    deps = [
        "//xla/mlir/utils:error_util",
        "@com_google_absl//absl/status",
        "@llvm-project//llvm:Support",
        "@llvm-project//mlir:FuncDialect",
        "@llvm-project//mlir:FuncExtensions",
        "@llvm-project//mlir:IR",
        "@llvm-project//mlir:Parser",
        "@llvm-project//mlir:Pass",
        "@llvm-project//mlir:Support",
        "@llvm-project//mlir:Transforms",
        "@stablehlo//:chlo_ops",
        "@stablehlo//:stablehlo_ops",
        "@stablehlo//:stablehlo_passes",
        "@tsl//tsl/platform:errors",
    ],
)

cc_library(
    name = "profiler",
    srcs = ["profiler.cc"],
    hdrs = ["profiler.h"],
    # TODO(b/172353882): figure out why compatible_with is needed to avoid some internal errors.
    compatible_with = [],
    copts = [
        "-fexceptions",
        "-fno-strict-aliasing",
    ],
    features = ["-use_header_modules"],
    deps = [
        ":status_casters",
        ":types",
        ":xplane_to_profile_instructions",
        # placeholder for index annotation deps
        "//xla:status",
        "//xla/backends/profiler:profiler_backends",
        "//xla/backends/profiler/cpu:python_tracer",
        "//xla/python/profiler/internal:traceme_wrapper",
        "@tsl//tsl/profiler/lib:profiler_session",
        "@tsl//tsl/profiler/rpc:profiler_server_impl",
        "@tsl//tsl/profiler/rpc/client:capture_profile",
        "@tsl//tsl/profiler/rpc/client:profiler_client_impl",
        "@pybind11",
    ],
)

cc_library(
    name = "transfer_guard_lib",
    srcs = ["transfer_guard_lib.cc"],
    hdrs = ["transfer_guard_lib.h"],
    compatible_with = [],
    copts = [
        "-fexceptions",
        "-fno-strict-aliasing",
    ],
    features = ["-use_header_modules"],
    visibility = [":friends"],
    deps = [
        ":status_casters",
        # placeholder for index annotation deps
        "@com_google_absl//absl/base:core_headers",
        "//xla:status",
        "//xla:util",
        "@pybind11",
        "@pybind11_abseil//pybind11_abseil:absl_casters",
    ],
)

cc_library(
    name = "util",
    srcs = ["util.cc"],
    hdrs = ["util.h"],
    compatible_with = [],
    copts = [
        "-fexceptions",
        "-fno-strict-aliasing",
    ],
    features = ["-use_header_modules"],
    deps = [
        "//xla:status",
        "//xla:util",
        "//xla/pjrt:pjrt_client",
        "//xla/pjrt:pjrt_future",
        "//xla/python/ifrt",
        "@com_google_absl//absl/strings:str_format",
        "@pybind11",
    ],
)

cc_library(
    name = "weakref_lru_cache",
    srcs = ["weakref_lru_cache.cc"],
    hdrs = ["weakref_lru_cache.h"],
    compatible_with = [],
    copts = [
        "-fexceptions",
        "-fno-strict-aliasing",
    ],
    features = ["-use_header_modules"],
    visibility = ["//visibility:private"],
    deps = [
        # placeholder for index annotation deps
        "@com_google_absl//absl/cleanup",
        "@com_google_absl//absl/synchronization",
        "//xla/pjrt:lru_cache",
        "@pybind11",
    ],
)

cc_library(
    name = "xla_compiler",
    srcs = ["xla_compiler.cc"],
    hdrs = ["xla_compiler.h"],
    compatible_with = [],
    copts = [
        "-fexceptions",
        "-fno-strict-aliasing",
    ],
    features = ["-use_header_modules"],
    deps = [
        ":exceptions",
        ":py_client",
        ":status_casters",
        ":types",
        # placeholder for index annotation deps
        "@com_google_absl//absl/hash",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/types:span",
        "//xla:array",
        "//xla:debug_options_flags",
        "//xla:shape_util",
        "//xla:statusor",
        "//xla:util",
        "//xla:xla_data_proto_cc",
        "//xla:xla_proto_cc",
        "//xla/client:executable_build_options",
        "//xla/client:xla_builder",
        "//xla/client:xla_computation",
        "//xla/hlo/ir:hlo",
        "//xla/hlo/ir:hlo_module_group",
        "//xla/service:call_inliner",
        "//xla/service:computation_placer",
        "//xla/service:custom_call_target_registry",
        "//xla/service:flatten_call_graph",
        "//xla/service:hlo_dce",
        "//xla/service:hlo_graph_dumper",
        "//xla/service:hlo_module_config",
        "//xla/service:hlo_parser",
        "//xla/service:hlo_pass",
        "//xla/service:hlo_proto_cc",
        "//xla/service:name_uniquer",
        "//xla/service:tuple_simplifier",
        "@tsl//tsl/lib/strings:proto_serialization",
        "@pybind11",
    ],
)

tf_proto_library(
    name = "py_host_callback_proto",
    srcs = ["py_host_callback.proto"],
    cc_api_version = 2,
)

# TODO(phawkins): the configuration settings here are overly confusing. The right fix is to split
# xla_extension.so so that each backend is a separate plugin, however that must wait for a clean
# ABI separation between devices.
config_setting(
    name = "link_gpu_plugin",
    define_values = {"xla_python_enable_gpu": "true"},
)

bool_flag(
    name = "enable_gpu",
    build_setting_default = True,
)

config_setting(
    name = "gpu_enabled",
    flag_values = {
        ":enable_gpu": "True",
    },
)

# If this flag is enabled, it sets RPATH on the xla_extension to values that are suitable for
# finding NVIDIA's CUDA libraries when they are installed as pip packages.
bool_flag(
    name = "jax_cuda_pip_rpaths",
    build_setting_default = False,
)

config_setting(
    name = "use_jax_cuda_pip_rpaths",
    flag_values = {
        ":jax_cuda_pip_rpaths": "True",
    },
)

# We cannot nest select and if_cuda_is_configured so we introduce
# a standalone cc_library target.
cc_library(
    name = "gpu_plugin_deps",
    deps = [
        "//xla/service:gpu_plugin",
    ] + if_cuda_is_configured([
        "//xla/stream_executor:cuda_platform",
    ]),
)

tsl_pybind_extension(
    name = "xla_extension",
    srcs = ["xla_extension.cc"],
    linkopts = select({
        ":use_jax_cuda_pip_rpaths": [
            "-Wl,-rpath,$$ORIGIN/../nvidia/cuda_cupti/lib",
            "-Wl,-rpath,$$ORIGIN/../nvidia/cuda_runtime/lib",
            "-Wl,-rpath,$$ORIGIN/../nvidia/cublas/lib",
            "-Wl,-rpath,$$ORIGIN/../nvidia/cufft/lib",
            "-Wl,-rpath,$$ORIGIN/../nvidia/cudnn/lib",
            "-Wl,-rpath,$$ORIGIN/../nvidia/cusolver/lib",
            "-Wl,-rpath,$$ORIGIN/../nvidia/nccl/lib",
        ],
        "//conditions:default": [],
    }),
    pytype_deps = [
        "//third_party/py/numpy",
    ],
    pytype_srcs = glob(["xla_extension/*.pyi"]),
    visibility = ["//visibility:public"],
    deps = [
        ":xla_extension_library",
        "@pybind11",
    ],
)

cc_library(
    name = "xla_extension_library",
    srcs = [
        "logging.cc",
        "logging.h",
        "xla.cc",
    ],
    hdrs = [
        "xla.h",
    ],
    compatible_with = [],
    copts = [
        "-fexceptions",
        "-fno-strict-aliasing",
    ],
    defines = select({
        ":gpu_enabled": ["XLA_PYTHON_ENABLE_GPU=1"],
        "//conditions:default": [],
    }),
    features = ["-use_header_modules"],
    deps = [
        ":custom_call_sharding",
        ":dlpack",
        ":jax_jit",
        ":mlir",
        ":ops",
        ":outfeed_receiver_py",
        ":pjit",
        ":pmap_lib",
        ":pprof_profile_builder",
        ":profiler",
        ":py_client",
        ":python_ref_manager",
        ":pytree",
        ":refine_polymorphic_shapes",
        ":status_casters",
        ":traceback",
        ":transfer_guard_lib",
        ":types",
        ":util",
        ":weakref_lru_cache",
        ":xla_compiler",
        # placeholder for index annotation deps
        "@com_google_absl//absl/base",
        "@com_google_absl//absl/log:initialize",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/types:span",
        "@local_config_python//:python_headers",  # buildcleaner: keep
        "//xla:literal",
        "//xla:shape_util",
        "//xla:status",
        "//xla:statusor",
        "//xla:types",
        "//xla:util",
        "//xla/pjrt:mlir_to_hlo",
        "//xla/pjrt:pjrt_api",
        "//xla/pjrt:pjrt_c_api_client",
        "//xla/pjrt:pjrt_client",
        "//xla/pjrt:pjrt_compiler",
        "//xla/pjrt:tfrt_cpu_pjrt_client",
        "//xla/pjrt/c:pjrt_c_api_hdrs",
        "//xla/pjrt/distributed",
        "//xla/pjrt/distributed:client",
        "//xla/pjrt/distributed:protocol_proto_cc",
        "//xla/pjrt/distributed:service",
        "//xla/python/ifrt",
        "//xla/python/pjrt_ifrt",
        "@tsl//tsl/distributed_runtime/preemption:preemption_sync_manager",
        "@tsl//tsl/platform",
        "@tsl//tsl/platform:logging",
        "@tsl//tsl/platform/cloud:gcs_file_system",
        "@tsl//tsl/python/lib/core:numpy",
        "@pybind11",
    ] + select({
        ":gpu_enabled": [
            "//xla/pjrt/gpu:se_gpu_pjrt_client",
        ],
        "//conditions:default": [],
    }) + select({
        ":link_gpu_plugin": [
            ":gpu_plugin_deps",
        ],
        "//conditions:default": [],
    }),
)

cc_library(
    name = "xplane_to_profile_instructions",
    srcs = ["xplane_to_profile_instructions.cc"],
    hdrs = ["xplane_to_profile_instructions.h"],
    deps = [
        "//xla:status",
        "//xla:xla_proto_cc",
        "//xla/hlo/ir:hlo",
        "//xla/service:hlo_proto_cc",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/types:optional",
        "@tsl//tsl/platform:env",
        "@tsl//tsl/platform:types",
        "@tsl//tsl/profiler/convert:xla_op_utils",
        "@tsl//tsl/profiler/protobuf:profiled_instructions_proto_cc",
        "@tsl//tsl/profiler/protobuf:xplane_proto_cc",
        "@tsl//tsl/profiler/utils:file_system_utils",
        "@tsl//tsl/profiler/utils:tf_xplane_visitor",
        "@tsl//tsl/profiler/utils:xplane_schema",
        "@tsl//tsl/profiler/utils:xplane_utils",
        "@tsl//tsl/profiler/utils:xplane_visitor",
    ],
)

xla_cc_test(
    name = "xplane_to_profile_instructions_test",
    srcs = ["xplane_to_profile_instructions_test.cc"],
    deps = [
        ":xplane_to_profile_instructions",
        "//xla/service:hlo_proto_cc",
        "//xla/tests:verified_hlo_module",
        "@tsl//tsl/platform:test",
        "@tsl//tsl/platform:test_main",
        "@tsl//tsl/profiler/convert:xla_op_utils",
        "@tsl//tsl/profiler/protobuf:profiled_instructions_proto_cc_impl",
        "@tsl//tsl/profiler/protobuf:xplane_proto_cc",
        "@tsl//tsl/profiler/rpc/client:save_profile",
        "@tsl//tsl/profiler/utils:file_system_utils",
        "@tsl//tsl/profiler/utils:xplane_builder",
        "@tsl//tsl/profiler/utils:xplane_schema",
    ],
)
