import sys
import os

args = sys.argv
if len(args) < 3:
    print("Usage: {} <getter.cpp> <name> [indices ...]".format(args[0]))
    sys.exit(1)

name = args[2]

with open(args[1], "w") as cpp:
    cpp.write("// SPDX-License-Identifier: GPL-3.0-only\n\n// This file was auto-generated.\n// If you want to edit this, edit the .tag_indices files and rerun the generator script, instead.\n\n")
    cpp.write("#include <cstddef>\n")
    cpp.write("#include <cstring>\n")
    cpp.write("#include <invader/tag/index/index.hpp>\n")
    cpp.write("#include <invader/file/file.hpp>\n")
    cpp.write("namespace Invader {\n")
    indices = {}

    for i in range(3, len(args)):
        basename = os.path.basename(args[i]).split(".")[0]
        with open(args[i]) as a:
            indices[basename] = [line.rstrip() for line in a]
        cpp.write("    static std::vector<File::TagFilePath> {}_indices() {{\n".format(basename))
        cpp.write("        static File::TagFilePath indices[] = {\n")
        resource = basename == "bitmaps" or basename == "sounds" or basename == "loc"
        for n in indices[basename]:
            name_split = n.split(".")
            cpp.write("            File::TagFilePath(\"{}\", HEK::TagClassInt::TAG_CLASS_{}),\n".format(name_split[0].replace("\\", "\\\\"), "NONE" if resource else name_split[1].upper()))
        cpp.write("        };\n")
        cpp.write("        return std::vector<File::TagFilePath>(indices, indices + sizeof(indices) / sizeof(*indices));\n")
        cpp.write("    }\n")

    cpp.write("    std::vector<File::TagFilePath> {}_indices(const char *map_name) {{\n".format(name))
    for basename in indices:
        index_list = indices[basename]
        cpp.write("        if(std::strcmp(map_name, \"{}\") == 0) return {}_indices();\n".format(basename, basename))
    cpp.write("         return {};\n")
    cpp.write("    }\n")
    cpp.write("}\n")
