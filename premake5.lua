workspace "chooter"
    configurations { "Debug", "Release" }

project "chooter"
    kind "ConsoleApp"
    language "C"
    targetdir "bin/%{cfg.buildcfg}"

    includedirs { "inc" }
    files { "src/*.c", "inc/*.h" }
    links { "SDL2", "SDL2_image", "SDL2_mixer" }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"
