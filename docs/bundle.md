# Bundle

The bundle format is a binary container for storing named, typed sections. It is used for plugin bundles, config bundles, and screen bundles.

## Structure

```
[magic: 4 bytes]               - "ALP\0" (legacy) or "ALB\0" (current)
[header length: uint32 LE]     - total size of everything except section data
[section count: uint16 LE]     - number of sections
[total size: uint32 LE]        - total bundle size in bytes (ALB only)

Section headers (repeated for each section):
  [type: 1 byte]               - section type
  [flags: uint16 LE]           - section flags, reserved (ALB only)
  [offset: uint32 LE]          - byte offset to section data (from start of bundle)
  [size: uint32 LE]            - section data size in bytes
  [crc32: uint32 LE]           - CRC32/IEEE checksum of section data
  [name: null-terminated string]

[header crc32: uint32 LE]      - CRC32/IEEE checksum of all preceding header bytes (ALB only)

Section data (repeated for each section):
  [data: N bytes]              - section data
  [null: 1 byte]               - null terminator
```

## Section Types

| Type   | Byte | Description                            |
|--------|------|----------------------------------------|
| ATTR   | 0x00 | String attribute (e.g. plugin name)    |
| BINARY | 0x01 | Binary data (e.g. code, config values) |
| SPRITE | 0x02 | Bitmap sprite data                     |
| CONFIG | 0x03 | Nested bundle containing config schema |

## Magic

- **ALP**: Legacy format. No header checksum. Section data is protected by per-section CRC32.
- **ALB**: Current format. Adds a total bundle size field, per-section flags, and a CRC32 checksum covering all header bytes (magic through end of section headers). This protects against corruption of section metadata (count, offsets, sizes, names).

Both formats are supported for decoding. New bundles are always encoded as ALB.

## Notes

- All integers are little-endian.
- Section names and ATTR data are null-terminated strings.
- The total size field (ALB) records the complete bundle size in bytes, allowing integrity verification before parsing.
- Section flags (ALB) are reserved for future use and must be set to zero.
- The section CRC32 checksum covers only the section data bytes for ALP (not the null terminator), and includes the null terminator for ALB.
- The header CRC32 checksum (ALB) covers all bytes preceding it in the header.
- CONFIG sections contain a complete, independently decodable bundle as their data.
