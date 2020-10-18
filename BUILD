cc_library(
    name = "uniform_distribution",
    srcs = [
        "uniform_distribution.cc",
    ],
    hdrs = [
        "uniform_distribution.h",
    ]
)

cc_test(
    name = "uniform_distribution_test",
    deps = [
        "@com_github_google_googletest//:gtest",
        "@com_github_google_googletest//:gtest_main",
        ":uniform_distribution",
    ],
    srcs = ["uniform_distribution_test.cc"],
)
