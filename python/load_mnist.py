import numpy as np
import cv2

img_path = "../train-images.idx3-ubyte"
label_path = "../train-labels.idx1-ubyte"

with open(img_path, "rb") as f:
    imgs_byte = f.read()

with open(label_path, "rb") as f:
    labels_byte = f.read()

data_num = int.from_bytes(imgs_byte[4:8])
img_height = int.from_bytes(imgs_byte[8:12])
img_width = int.from_bytes(imgs_byte[12:16])
pixel_num = img_height * img_width
imgs = []
labels = []

img_offset = 16
label_offset = 8
for i in range(data_num):
    # load image
    img = [[] for j in range(img_height)]
    for j in range(img_height):
        for k in range(img_width):
            img[j].append(imgs_byte[img_offset])
            img_offset += 1
    imgs.append(img)

    # load label
    labels.append(labels_byte[label_offset])
    label_offset += 1
print(min(labels), max(labels))

# display image
cv2.imwrite("{}.jpg".format(labels[10]), np.array(imgs[10]))
# print("\n".join(["".join(map(str, [j//max(i) for j in i])) for i in imgs[0]]))