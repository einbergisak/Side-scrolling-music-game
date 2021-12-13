# Written by I Einberg 2021

from PIL import Image

image = Image.open("level1.png")

width = image.width

if image.size[1] > 32: # Makes sure that image height does not exceed the display height
    print("invalid image size")
    exit()

pixels = list(image.convert("1").getdata())
output = []

""" A page has 8 rows, and the image is "width" columns wide, so page*8*width gives the correct page start index.
The image is "width" columns wide, so row*width gives correct row, and col gives correct column.
Pixels will only have values 0 or 255, so a division by 255 turns every value into 0 or 1 instead.
These bits are concatenated to a string, which is then interpreted as a byte, with each bit corresponding to a pixel."""
for page in range(0, 4):
    for col in range(0, width):
        bytestr = ""
        for row in range(0, 8):
            bytestr = str(pixels[page * 8 * width + row * width + col] // 255) + bytestr
        output.append(int(bytestr, 2))

with open("output.txt", "w") as o:
    print(output, file=o)
    o.close()

print(output)
