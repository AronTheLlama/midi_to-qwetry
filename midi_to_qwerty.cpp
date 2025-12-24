#include <iostream>
#include <map>
#include <string>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <csignal>
#include <rtmidi/RtMidi.h>

volatile sig_atomic_t running = 1;

void signalHandler(int signum) {
    std::cout << "\nInterrupt signal received. Exiting...\n";
    running = 0;
}
std::map<int, std::string> noteToKey = {
    {36, "1"},
    {37, "exclam"},
    {38, "2"},
    {39, "at"},
    {40, "3"},
    {41, "4"},
    {42, "numbersign"},
    {43, "5"},
    {44, "percent"},
    {45, "6"},
    {46, "asciicircum"},
    {47, "7"},
    {48, "8"},
    {49, "asterisk"},
    {50, "9"},
    {51, "parenleft"},
    {52, "0"},
    {53, "q"},
    {54, "shift+q"},
    {55, "w"},
    {56, "shift+w"},
    {57, "e"},
    {58, "shift+e"},
    {59, "r"},
    {60, "t"},
    {61, "shift+t"},
    {62, "y"},
    {63, "shift+y"},
    {64, "u"},
    {65, "i"},
    {66, "shift+i"},
    {67, "o"},
    {68, "shift+o"},
    {69, "p"},
    {70, "shift+p"},
    {71, "a"},
    {72, "s"},
    {73, "shift+s"},
    {74, "d"},
    {75, "shift+d"},
    {76, "f"},
    {77, "g"},
    {78, "shift+g"},
    {79, "h"},
    {80, "shift+h"},
    {81, "j"},
    {82, "shift+j"},
    {83, "k"},
    {84, "l"},
    {85, "shift+l"},
    {86, "z"},
    {87, "shift+z"},
    {88, "x"},
    {89, "c"},
    {90, "shift+c"},
    {91, "v"},
    {92, "shift+v"},
    {93, "b"},
    {94, "shift+b"},
    {95, "n"},
    {96, "m"},
    {97, "shift+m"},
    {98, "parenright"}
};

void sendKey(const std::string& key) {
    std::string command = "xdotool key " + key;
    system(command.c_str());
}

void midiCallback(double deltatime, std::vector<unsigned char>* message, void* userData) {
    if (message->size() < 3) return;

    unsigned char status = (*message)[0];
    unsigned char note = (*message)[1];
    unsigned char velocity = (*message)[2];

    if ((status & 0xF0) == 0x90 && velocity > 0) {
        auto it = noteToKey.find(note);
        if (it != noteToKey.end()) {
            std::cout << "MIDI Note " << (int)note << " → Key '" << it->second << "'\n";
            sendKey(it->second);
        } else {
            std::cout << "MIDI Note " << (int)note << " (not mapped)\n";
        }
    }
}

int main() {
    std::cout << "=== MIDI to QWERTY Translator for Linux (C++) ===\n";
    std::cout << "Translates MIDI notes to keyboard keypresses\n";
    std::cout << "Supports 63 keys (C2 to D7, MIDI notes 36-98)\n";
    std::cout << "White keys = normal keys, Black keys = shifted keys\n\n";

    signal(SIGINT, signalHandler);

    RtMidiIn* midiin = nullptr;

    try {
        midiin = new RtMidiIn();

        unsigned int nPorts = midiin->getPortCount();
        std::cout << "Available MIDI input ports:\n";

        if (nPorts == 0) {
            std::cout << "  No MIDI input ports found!\n";
            std::cout << "\nPlease connect a MIDI device and try again.\n";
            delete midiin;
            return 1;
        }

        for (unsigned int i = 0; i < nPorts; i++) {
            std::cout << "  [" << i << "] " << midiin->getPortName(i) << "\n";
        }

        unsigned int selectedPort = 0;
        if (nPorts > 1) {
            std::cout << "\nSelect MIDI port number: ";
            std::cin >> selectedPort;

            if (selectedPort >= nPorts) {
                std::cout << "Invalid port number!\n";
                delete midiin;
                return 1;
            }
        } else {
            std::cout << "\nUsing MIDI port: " << midiin->getPortName(0) << "\n";
        }

        midiin->openPort(selectedPort);

        midiin->setCallback(&midiCallback);

        midiin->ignoreTypes(false, false, false);

        std::cout << "\nOpening MIDI port: " << midiin->getPortName(selectedPort) << "\n";
        std::cout << "Ready! Play MIDI notes to generate keypresses.\n";
        std::cout << "Press Ctrl+C to exit.\n\n";

        std::cout << "Switching to target window in 3 seconds...\n";
        std::this_thread::sleep_for(std::chrono::seconds(3));

        std::cout << "Listening for MIDI input...\n\n";


        while (running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }


        delete midiin;

    } catch (RtMidiError& error) {
        std::cerr << "RtMidi error: " << error.getMessage() << "\n";
        delete midiin;
        return 1;
    }

    std::cout << "\nExiting MIDI to QWERTY translator.\n";
    return 0;
}
