"""
PBR Texture Generator for Nanite Downtown City Block
Generates realistic scanned-style Albedo, Normal, and Puddle ORM textures.
"""

import os
import math
import struct
import zlib
import random

def save_png_rgb(filepath, width, height, rgb_bytes):
    raw_rows = []
    stride = width * 3
    for y in range(height):
        row = rgb_bytes[y * stride:(y + 1) * stride]
        raw_rows.append(b"\x00" + row)
    raw_data = b"".join(raw_rows)

    def chunk(tag, data):
        crc = zlib.crc32(tag + data) & 0xFFFFFFFF
        return struct.pack(">I", len(data)) + tag + data + struct.pack(">I", crc)

    ihdr = struct.pack(">IIBBBBB", width, height, 8, 2, 0, 0, 0)
    idat = zlib.compress(raw_data, 6)
    png = b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", ihdr) + chunk(b"IDAT", idat) + chunk(b"IEND", b"")

    os.makedirs(os.path.dirname(filepath), exist_ok=True)
    with open(filepath, "wb") as f:
        f.write(png)
    print(f"[Texture] Saved: {filepath}")

def generate_textures():
    out_dir = r"C:\Users\Admin\Documents\Unreal Projects\WonderTowerTycoon\Assets\WonderWorld\Downtown\Textures"
    os.makedirs(out_dir, exist_ok=True)

    w, h = 512, 512
    random.seed(42)

    # 1. Asphalt Albedo & Normal & Puddle ORM
    albedo = bytearray(w * h * 3)
    normal = bytearray(w * h * 3)
    orm = bytearray(w * h * 3)

    # Generate organic puddle elevation map using multiple sine harmonics
    for y in range(h):
        ny = y / h
        for x in range(w):
            nx = x / w
            idx = (y * w + x) * 3

            # Noise for aggregate grain
            grain = (random.random() - 0.5) * 16
            
            # Low-frequency wave for puddles
            puddle_val = (
                math.sin(nx * 12.0) * math.cos(ny * 10.0) +
                math.sin(nx * 24.0 + 1.5) * math.cos(ny * 20.0 + 2.0) * 0.5 +
                math.sin(nx * 6.0) * 0.7
            )

            is_puddle = puddle_val > 0.45

            if is_puddle:
                # Wet puddle: darker albedo (water saturation), near-flat normal, low roughness
                albedo[idx] = max(0, min(255, int(18 + grain * 0.3)))
                albedo[idx+1] = max(0, min(255, int(19 + grain * 0.3)))
                albedo[idx+2] = max(0, min(255, int(22 + grain * 0.3)))

                # Water surface has smooth normal pointing straight up (+Z: 128, 128, 255)
                normal[idx] = 128
                normal[idx+1] = 128
                normal[idx+2] = 255

                # ORM: R=AO (255), G=Roughness (14 -> 0.05 razor mirror for Lumen reflections!), B=Metallic (0)
                orm[idx] = 255
                orm[idx+1] = 14
                orm[idx+2] = 0
            else:
                # Dry asphalt: gravel tones, perturbed normal, high roughness
                base = 42 + int(grain)
                albedo[idx] = max(0, min(255, base))
                albedo[idx+1] = max(0, min(255, base + 1))
                albedo[idx+2] = max(0, min(255, base + 3))

                # Tangent space normal for asphalt stones
                nx_bump = int(128 + (random.random() - 0.5) * 45)
                ny_bump = int(128 + (random.random() - 0.5) * 45)
                normal[idx] = max(0, min(255, nx_bump))
                normal[idx+1] = max(0, min(255, ny_bump))
                normal[idx+2] = 240

                # ORM: Roughness = 210 (0.82 matte asphalt)
                orm[idx] = 240
                orm[idx+1] = 210
                orm[idx+2] = 0

    save_png_rgb(os.path.join(out_dir, "T_Asphalt_Albedo.png"), w, h, albedo)
    save_png_rgb(os.path.join(out_dir, "T_Asphalt_Normal.png"), w, h, normal)
    save_png_rgb(os.path.join(out_dir, "T_Asphalt_ORM.png"), w, h, orm)

    # 2. Concrete Sidewalk Albedo & Normal
    sw_albedo = bytearray(w * h * 3)
    sw_normal = bytearray(w * h * 3)
    for y in range(h):
        # Joint grooves every 128 pixels
        is_joint_y = (y % 128) < 3
        for x in range(w):
            is_joint_x = (x % 128) < 3
            idx = (y * w + x) * 3
            noise = (random.random() - 0.5) * 12

            if is_joint_x or is_joint_y:
                sw_albedo[idx] = 60
                sw_albedo[idx+1] = 60
                sw_albedo[idx+2] = 62
                sw_normal[idx] = 128
                sw_normal[idx+1] = 128
                sw_normal[idx+2] = 190
            else:
                val = max(0, min(255, int(155 + noise)))
                sw_albedo[idx] = val
                sw_albedo[idx+1] = val
                sw_albedo[idx+2] = val + 2
                sw_normal[idx] = int(128 + (random.random() - 0.5) * 20)
                sw_normal[idx+1] = int(128 + (random.random() - 0.5) * 20)
                sw_normal[idx+2] = 250

    save_png_rgb(os.path.join(out_dir, "T_Sidewalk_Albedo.png"), w, h, sw_albedo)
    save_png_rgb(os.path.join(out_dir, "T_Sidewalk_Normal.png"), w, h, sw_normal)

if __name__ == "__main__":
    generate_textures()
