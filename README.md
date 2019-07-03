# Invader

Invader is an open source map and tag builder for Halo: Custom Edition written in C++ using the C++17 standard.

## Programs

To remove the reliance of one huge executable, something that has caused issues with Halo Custom Edition's tool.exe, as
well as make things easier for me to work with, this project is split into different programs.

### invader-bitmap

This program generates bitmap files. It takes the following arguments:

| Argument | Alternate | Description |
| --- | --- | --- |
| `--info` | `-i` | Show credits, source info, and other info. |
| `--data <dir>` | `-d` | Data directory. |
| `--tags <dir>` | `-t` | Tags directory. |
| `--input-format <format>` | `-I` | Input format. Can be `tif` (default) or `png`. |
| `--mipmap-fade <factor>` | `-f` | Fade-to-gray factor for mipmaps. |
| `--mipmap-scale <type>` | `-s` | Mipmap scaling. Can be `linear` (default), `nearest-alpha`, `nearest`, or `none`. |

### invader-build

This program builds cache files.

| Argument | Alternate | Description |
| --- | --- | --- |
| `--info` | `-i` | Show credits, source info, and other info. |
| `--maps <dir>` | `-m` | Use a specific maps directory. |
| `--no-indexed-tags` | `-n` | Do not index tags with resource maps. |
| `--output <file>` | `-o` | Output to a specific file. |
| `--quiet` | `-q` | Only output error messages. |
| `--tags <dir>` | `-t` | Tags directory. Use multiple times to add tags directories. |
| `--with-index <file>` | `-w` | Use an index file for the tags. |

### invader-crc

This program gets and/or modifies the calculated CRC32 of a map file. It takes one or two arguments: `<map> [new crc]`.
Not specifying a CRC32 value will only calculate the CRC32 value of the cache file without modifying it. Otherwise, it
will modify it and then calculate the CRC32.

### invader-indexer

This program builds index files for usage with `--with-index` with invader-build. It takes exactly two arguments:
`<input map> <output index>`
