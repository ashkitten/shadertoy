#include <iostream>

#include "smol-v/source/smolv.h"

int main(int argc, char **argv) {
    if (argv[1][0] == 'e') {
        smolv::ByteArray spirv(
            (std::istreambuf_iterator<char>(std::cin)),
            (std::istreambuf_iterator<char>())
        );
        smolv::ByteArray smolv;
        smolv::Encode(
            spirv.data(),
            spirv.size(),
            smolv,
            smolv::EncodeFlags::kEncodeFlagStripDebugInfo
        );
        std::cout.write((char*) smolv.data(), smolv.size());
    } else if (argv[1][0] == 's') {
        smolv::ByteArray smolv(
            (std::istreambuf_iterator<char>(std::cin)),
            (std::istreambuf_iterator<char>())
        );
        std::cout << smolv::GetDecodedBufferSize(smolv.data(), smolv.size()) << std::endl;
    } else if (argv[1][0] == 'd') {
        smolv::ByteArray smolv(
            (std::istreambuf_iterator<char>(std::cin)),
            (std::istreambuf_iterator<char>())
        );
        size_t spirvSize = smolv::GetDecodedBufferSize(smolv.data(), smolv.size());
        uint8_t *spirv[spirvSize];
        smolv::Decode(smolv.data(), smolv.size(), spirv, spirvSize);
        std::cout.write((char*) spirv, spirvSize);
    }

    return 0;
}
