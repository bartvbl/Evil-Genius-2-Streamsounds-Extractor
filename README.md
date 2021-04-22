# .streamsounds resource file extractor for evil genius 2

Not 100% accurate, but manages to extract most of the sound files successfully.

Have only tested on linux, though it should work on all platforms.

It's a single file project. I've included a CMakeLists.txt file, though setting it up through some other means should be trivial. Note that the project uses std::filesystem, so make sure the C++17 standard or newer is selected.

Once compiled, run the executable with a path to a .streamsounds file to extract, and its contents will appear in the directory you run the program from.

Enjoy!
