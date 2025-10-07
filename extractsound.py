import os
import wave
import numpy as np

# Folder containing the .bin file (current directory)
folder = "."
bin_files = [f for f in os.listdir(folder) if f.lower().endswith('.bin')]

for bin_file in bin_files:
    bin_path = os.path.join(folder, bin_file)
    with open(bin_path, "rb") as f:
        raw = f.read()

    shift = 3  # Only process shift 3
    shifted_raw = raw[shift:]
    # Only keep full int32 samples
    n_int32 = len(shifted_raw) // 4
    shifted_raw = shifted_raw[:n_int32 * 4]
    if n_int32 == 0:
        continue  # Not enough data for this shift

    data = np.frombuffer(shifted_raw, dtype=np.int32)

    # Reshape to (N, 2): [RIGHT, LEFT, RIGHT, LEFT, ...]
    if len(data) % 2 != 0:
        data = data[:-1]  # Remove last sample if odd

    stereo = data.reshape(-1, 2)
    right = stereo[:, 0]
    left = stereo[:, 1]

    # Interleave channels for stereo WAV, 32-bit PCM
    stereo_32 = np.column_stack((right, left)).ravel().astype(np.int32)

    # Write to WAV file (32 bits)
    wav_path = os.path.splitext(bin_path)[0] + f".wav"
    with wave.open(wav_path, 'w') as wf:
        wf.setnchannels(2)           # Stereo
        wf.setsampwidth(4)           # 32 bits
        wf.setframerate(44000)       # 44kHz
        wf.writeframes(stereo_32.tobytes())

    print(f"Converted {bin_file} (shift {shift}) to {os.path.basename(wav_path)} (32 bits)")