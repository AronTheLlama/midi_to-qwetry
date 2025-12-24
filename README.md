# midi_to-qwetry

Required dependancies:
rtmidi, xdotool, base-devel

Linux Arch based:

sudo pacman -S rtmidi xdotool base-devel

Linux Debian based (apt):
sudo apt install rtmidi xdotool base-devel


# TO COMPILE:

g++ -o midi_translator midi_to_qwerty.cpp -lrtmidi -lpthread

# NOTE:

This piece of junk i made might not work on every distrobution - TESTED ON: endeavouros (arch)
