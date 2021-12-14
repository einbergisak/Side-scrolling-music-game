import math

tones = [
    ("D4", 293.66),
    ("E4", 329.63),
    ("G4", 392.00),
    ("A4", 440.00),
    ("B4", 493.88),
    ("C5", 523.25),
    ("D5", 587.33),
    ("E5", 659.25),
]

def gen_wave(name, frequency):
    print(f"\nchar {name}[] =", "{")
    n_ticks = (80000000/frequency)/1000
    for i in range(0, round(n_ticks)):
        print(math.floor(127.5 * math.sin(math.pi * 2 * i / n_ticks)), end=", ")
        #print(math.floor(50 * math.sin(math.pi * 2 * i / n_ticks)), end=", ")
        #print(i)
    print("};")
    print(f"int {name}length = sizeof({name});")

if __name__ == "__main__":
    for tone in tones:
        gen_wave(tone[0], tone[1])
