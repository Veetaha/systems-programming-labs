workspace "lab1"
    configurations { "Debug", "Release" }
    startproject "lab1"

project "lab1"
    language "C"
    cdialect "gnu11"
    kind     "ConsoleApp"

    targetname "lab1.out"
    location   "build"
    targetdir  "build/bin"
    objdir     "build/bin-int"

    files { "src/**.c" }
    includedirs {
        "include"
    }

    -- linkoptions {
    -- }
    -- buildoptions {
    --     "-std=gnu++2a", -- workaround: `cppdialect "gnu++2a"` is not allowed
    --     "-pedantic",
    --     "-s EXIT_RUNTIME=0",
    --     "-Wno-error-padded"
    -- }
    enablewarnings {
        "error",
        "all",
        "extra"
    }
    -- disablewarnings {
    --     "dollar-in-identifier-extension",
    --     "missing-prototypes",
    --     "global-constructors",
    --     "gnu-zero-variadic-macro-arguments",
    --     "c++98-compat-pedantic",
    --     "c++98-compat"
    -- }

    filter "configurations:Debug"
        runtime "Debug"
        defines { "DEBUG" }
        buildoptions {
            "-g3"
        }

    filter "configurations:Release"
        runtime "Release"
        defines { "NDEBUG" }
        buildoptions {
            "-O3",
        }
        linkoptions {
            "-O3"
            -- "--llvm-lto 1"
        }
