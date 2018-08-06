# xfce_with_bing_wallpaper_cpp
Update the xfce background with the bing wallpaper of the day

This cpp program lets you update the bing wallpaper of the day for Xfce4 environment desktop.

This program check first wether or not you already have the bing wallpaper of the day before downloading it. So it uses no bandwith unnecessarily.
This program checks wether or not the bing wallpaper of the day is already set before setting it.
This program will wait for an internet connection if the download fail / 

So what are you waiting ???? Enjoy the bing wallpaper of the Day.

INSTALLATION

    First check that you have boost and libpthread (install by default on unix like systems) if you want to compile the program 
    yourself. If not please do install boost libraries.
    https://www.boost.org
    Download the source in your home directory or wherever you like : ~/xfce_bing_wallpaper/main.cpp
    run these commands : cd ~/xfce_bing_wallpaper/ && g++ -Wall -fexceptions -I/boost_install_dir/ -L/boost_install_dir/lib 
    -lboost_system 
    -lboost_filesystem -lboost_regex -lboost_timer -lboost_chrono -lboost_thread -lpthread ~/Xfce_Bing_Wallpaper/main.cpp -o 
    ~/xfce_bing_wallpaper.bin 
    If you have any issue to compile the program yourself, you can try to use the generated binary released for you

Now setup xfce

    Go in session & startup 
    Add this command in startup script 
     ~/xfce_bing_wallpaper.bin
    
Bravo it's OK your background will be updated everyday with bing daily wallpaper

