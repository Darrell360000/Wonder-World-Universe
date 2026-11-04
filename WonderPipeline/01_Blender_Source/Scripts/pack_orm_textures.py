"""
PBR Channel Packer for NVIDIA RTX 5060 (pack_orm_textures.py)
Packs Ambient Occlusion (Red), Roughness (Green), and Metallic (Blue) into a single
_ORM.png texture inside 02_Shared_Game_Core/Textures_2K_4K_ORM/.
"""

import os
import struct
import zlib


def write_minimal_orm_png(filepath, width=64, height=64, ao=255, roughness=64, metallic=210):
    """Generates a valid 24-bit RGB PNG where R=AO, G=Roughness, B=Metallic without external deps."""
    raw_rows = []
    row_bytes = bytes([ao, roughness, metallic]) * width
    for _ in range(height):
        raw_rows.append(b"\x00" + row_bytes)
    raw_data = b"".join(raw_rows)

    def make_chunk(chunk_type, data):
        crc = zlib.crc32(chunk_type + data) & 0xFFFFFFFF
        return struct.pack(">I", len(data)) + chunk_type + data + struct.pack(">I", crc)

    ihdr = struct.pack(">IIBBBBB", width, height, 8, 2, 0, 0, 0)
    idat = zlib.compress(raw_data, 9)
    png_bytes = b"\x89PNG\r\n\x1a\n" + make_chunk(b"IHDR", ihdr) + make_chunk(b"IDAT", idat) + make_chunk(b"IEND", b"")

    os.makedirs(os.path.dirname(filepath), exist_ok=True)
    with open(filepath, "wb") as f:
        f.write(png_bytes)


def main():
    pipeline_root = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
    out_dir = os.path.join(pipeline_root, "02_Shared_Game_Core", "Textures_2K_4K_ORM")
    os.makedirs(out_dir, exist_ok=True)

    presets = [
        ("T_DarrellXGen_EqualizerHat_ORM.png", 255, 45, 230),
        ("T_MamaWonderTower_Spire_ORM.png", 255, 38, 240),
        ("T_JaxsonAnvilCrown_ORM.png", 240, 80, 250),
    ]
    for filename, r_ao, g_rough, b_metal in presets:
        out_path = os.path.join(out_dir, filename)
        write_minimal_orm_png(out_path, width=64, height=64, ao=r_ao, roughness=g_rough, metallic=b_metal)
        print(f"[ORM Packer] Packed R(AO)={r_ao}, G(Rough)={g_rough}, B(Metal)={b_metal} -> {out_path}")


if __name__ == "__main__":
    main()
