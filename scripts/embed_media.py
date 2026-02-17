#!/usr/bin/env python
"""Generate C++ source files containing embedded media data."""

import os
import sys

MEDIA_DIR = os.path.join(os.path.dirname(__file__), "..", "Tetrominos", "media")
DEFAULT_BUILD_DIR = os.path.join(os.path.dirname(__file__), "..", "build")


def sanitize_name(filename):
    """Convert a filename to a valid C identifier."""
    return filename.replace(".", "_").replace("-", "_").replace(" ", "_")


def main():
    media_dir = os.path.abspath(MEDIA_DIR)
    build_dir = os.path.abspath(sys.argv[1] if len(sys.argv) > 1 else DEFAULT_BUILD_DIR)
    os.makedirs(build_dir, exist_ok=True)

    files = sorted(f for f in os.listdir(media_dir) if os.path.isfile(os.path.join(media_dir, f)))
    if not files:
        print(f"No media files found in {media_dir}", file=sys.stderr)
        sys.exit(1)

    header_path = os.path.join(build_dir, "media_data.h")
    source_path = os.path.join(build_dir, "media_data.cpp")

    # --- Header ---
    with open(header_path, "w") as h:
        h.write("#pragma once\n")
        h.write("#include <cstddef>\n\n")
        h.write("bool findEmbeddedMedia(const char* path, const unsigned char** data, size_t* size);\n")

    # --- Source ---
    with open(source_path, "w") as s:
        s.write('#include "media_data.h"\n')
        s.write("#include <cstring>\n\n")

        # Emit byte arrays
        for filename in files:
            filepath = os.path.join(media_dir, filename)
            varname = sanitize_name(filename)
            data = open(filepath, "rb").read()
            s.write(f"static const unsigned char media_{varname}[] = {{\n")
            for i in range(0, len(data), 16):
                chunk = data[i : i + 16]
                s.write("    " + ", ".join(f"0x{b:02x}" for b in chunk) + ",\n")
            s.write("};\n\n")

        # Emit lookup table
        s.write("struct EmbeddedMediaEntry {\n")
        s.write("    const char* path;\n")
        s.write("    const unsigned char* data;\n")
        s.write("    size_t size;\n")
        s.write("};\n\n")
        s.write("static const EmbeddedMediaEntry g_embeddedMedia[] = {\n")
        for filename in files:
            varname = sanitize_name(filename)
            s.write(f'    {{ "media/{filename}", media_{varname}, sizeof(media_{varname}) }},\n')
        s.write("};\n\n")

        # Emit lookup function
        s.write("bool findEmbeddedMedia(const char* path, const unsigned char** data, size_t* size) {\n")
        s.write(f"    for (size_t i = 0; i < {len(files)}; i++) {{\n")
        s.write("        if (strcmp(path, g_embeddedMedia[i].path) == 0) {\n")
        s.write("            *data = g_embeddedMedia[i].data;\n")
        s.write("            *size = g_embeddedMedia[i].size;\n")
        s.write("            return true;\n")
        s.write("        }\n")
        s.write("    }\n")
        s.write("    return false;\n")
        s.write("}\n")

    print(f"Generated {header_path}")
    print(f"Generated {source_path}")
    print(f"Embedded {len(files)} files")


if __name__ == "__main__":
    main()
