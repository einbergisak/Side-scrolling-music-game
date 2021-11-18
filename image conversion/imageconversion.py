from PIL import Image
image = Image.open("yeet.png")

if image.size[1] > 32:
    print("invalid image size")
    exit()

image = image
pixels = list(image.convert("1").getdata())
print(pixels)
print("len: ", len(pixels))
output = []

for page in range(0, 4):
    for col in range(0, 128):
        bytestr = ""
        for row in range(0, 8):
            bytestr = str(pixels[page*1024+row*128 + col]//255) + bytestr
        byte = int(bytestr, 2)
        output.append(byte)



# for n in range(0, len(pixels) // 8):
#     bytestr = ""
#     for bitN in range(0, 8):
#         bytestr += str(pixels[n*8 + bitN]//255)
#     byte = int(bytestr, 2)
#     output.append(byte)

print(output)