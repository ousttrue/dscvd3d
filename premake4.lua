-- A solution contains projects, and defines the available configurations
solution "SampleGrabber"
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
project "SampleGrabber"
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
    "C:/WTL80/include",
    "$(DXSDK_DIR)/include",
    "C:/Program Files/Microsoft SDKs/Windows/v6.1/Include",
    "C:/opencv/include",
    "C:/opencv/modules/core/include",
    "C:/opencv/modules/flann/include",
    "C:/opencv/modules/imgproc/include",
    "C:/opencv/modules/photo/include",
    "C:/opencv/modules/video/include",
    "C:/opencv/modules/features2d/include",
    "C:/opencv/modules/objdetect/include",
    "C:/opencv/modules/calib3d/include",
    "C:/opencv/modules/ml/include",
    "C:/opencv/modules/highgui/include",
    "C:/opencv/modules/contrib/include",
}
libdirs {
    "C:/WinDDK/7600.16385.1/lib/ATL/i386",
    "$(DXSDK_DIR)/lib/x86",
    "C:/opencv/build/x86/vc10/staticlib",
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
        "opencv_core240d",
        "opencv_flann240d",
        "opencv_imgproc240d",
        "opencv_photo240d",
        "opencv_video240d",
        "opencv_features2d240d",
        "opencv_objdetect240d",
        "opencv_calib3d240d",
        "opencv_ml240d",
        "opencv_highgui240d",
        "opencv_contrib240d",
    }
end

configuration "Release"
do
    defines { "NDEBUG" }
    flags { "Optimize" }
    targetdir "release"
    links {
        "zlib",
        "opencv_core240",
        "opencv_flann240",
        "opencv_imgproc240",
        "opencv_photo240",
        "opencv_video240",
        "opencv_features2d240",
        "opencv_objdetect240",
        "opencv_calib3d240",
        "opencv_ml240",
        "opencv_highgui240",
        "opencv_contrib240",
    }
end

