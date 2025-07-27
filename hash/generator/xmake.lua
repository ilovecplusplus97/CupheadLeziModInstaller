add_requires("hash-library")

target("hashgen")
    set_kind("binary")
    set_default(false)
    set_policy("build.fence", true)
    add_files("main.cpp")
    add_packages("hash-library")