from PIL import Image
import os

path='../training/mnist-training-data.bin'

with open(path, "rb") as f:
    for i in range(10):
        out = int.from_bytes(f.read(1), "big")
        ba = bytearray(f.read(784))

        image = Image.new("L", (28, 28))
        for x in range(28):
            for y in range(28):
                image.putpixel((x,y), ba[y*28 + x])

        image.save("test_image-" + str(out) + ".jpg", "JPEG")
