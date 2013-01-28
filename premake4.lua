OPENCV_DIR="C:/opencv243"

------------------------------------------------------------------------------
-- Solution
------------------------------------------------------------------------------
solution "dscvd3d"
configurations { "Release", "Debug" }
configuration "gmake Debug"
do
    buildoptions { "-g" }
    linkoptions { "-g" }
end

configuration "gmake"
do
    buildoptions { 
        "-Wall", 
    }
end

configuration "gmake windows"
do
    buildoptions { 
        "-U__CYGWIN__", 
    }
end

configuration "vs*"
do
    linkoptions {}
end

configuration "windows*"
do
    defines {
        'WIN32',
        '_WIN32',
        '_WINDOWS',
    }
end

configuration {}

------------------------------------------------------------------------------
-- Project
------------------------------------------------------------------------------
project "dscvd3d"
--language "C"
language "C++"
--kind "StaticLib"
--kind "DynamicLib"
--kind "ConsoleApp"
kind "WindowedApp"

files {
    "*.cpp", "*.h",
}
flags {
    "WinMain", "Unicode", "StaticRuntime",
}
buildoptions {
    "/wd4995",
}
defines {
}
includedirs {
    "C:/WinDDK/7600.16385.1/inc/atl71",
    "$(DXSDK_DIR)/include",
    OPENCV_DIR.."/build/include",
}
libdirs {
    "C:/WinDDK/7600.16385.1/lib/ATL/i386",
    "$(DXSDK_DIR)/lib/x86",
    OPENCV_DIR.."/build/x86/vc10/staticlib",
}
links {
    "atlthunk",
    "d3d9",
    "d3dx9",
}
configuration "Debug"
do
    defines { "DEBUG" }
    flags { "Symbols" }
    targetdir "debug"
    links {
        "zlibd",
        "opencv_core243d",
        "opencv_flann243d",
        "opencv_imgproc243d",
        "opencv_photo243d",
        "opencv_video243d",
        "opencv_features2d243d",
        "opencv_objdetect243d",
        "opencv_calib3d243d",
        "opencv_ml243d",
        "opencv_highgui243d",
        "opencv_contrib243d",
    }
end

configuration "Release"
do
    defines { "NDEBUG" }
    flags { "Optimize" }
    targetdir "release"
    links {
        "zlib",
        "opencv_core243",
        "opencv_flann243",
        "opencv_imgproc243",
        "opencv_photo243",
        "opencv_video243",
        "opencv_features2d243",
        "opencv_objdetect243",
        "opencv_calib3d243",
        "opencv_ml243",
        "opencv_highgui243",
        "opencv_contrib243",
    }
end

