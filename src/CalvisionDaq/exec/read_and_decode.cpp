#include "CalvisionDaq/digitizer/Digitizer.h"
#include "CalvisionDaq/digitizer/Decoder.h"
#include "CalvisionDaq/digitizer/CaenError.h"
#include "CalvisionDaq/root/RootIO.h"

#include <iostream>

int main(int argc, char** argv) {

    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " decoded_data.root\n";
        return 1;
    }

    std::string outfile_name(argv[1]);

    try {

        std::cout << "Opening digitizer\n";
        Digitizer digi;

        std::cout << "Setting up digitizer\n";
        digi.setup();


        std::cout << "Opening decoder and writer\n";
        Decoder decoder;
        RootWriter root_io(outfile_name);
        root_io.setup(decoder.event());
        digi.set_event_callback([&decoder, &root_io] (const char* data, UIntType count) {
                    decoder.read_event(data, count);
                    decoder.apply_corrections();
                    root_io.handle_event(decoder.event());
                });

        digi.print();

        std::cout << "Beginning readout\n";
        digi.readout([](const Digitizer& d) { return d.num_events_read() < 10000; });
        std::cout << "Stopped readout\n";

        std::cout << "Writing file...\n";
        root_io.write();

        std::cout << "Resetting digitizer...\n";
        digi.reset();

        std::cout << "Done.\n";

    } catch (CaenError error) {
        error.print_error();
    }

    return 0;
}
