import numpy as np
import cv2

def load_file(file_path):
    with open(file_path, "rb") as f:
        return bytearray(f.read())

def read_data_from_bytes(data_bytes, start, end):
    return int.from_bytes(data_bytes[start:end], byteorder='big')

def load_images_and_labels(img_path, label_path):
    imgs_byte = load_file(img_path)
    labels_byte = load_file(label_path)

    data_num = read_data_from_bytes(imgs_byte, 4, 8)
    img_height = read_data_from_bytes(imgs_byte, 8, 12)
    img_width = read_data_from_bytes(imgs_byte, 12, 16)

    imgs = []
    labels = []

    img_offset = 16
    label_offset = 8

    for _ in range(data_num):
        # load image
        img = np.array(imgs_byte[img_offset:img_offset + img_height * img_width], dtype=np.uint8)
        img = img.reshape((img_height, img_width))
        img_offset += img_height * img_width
        imgs.append(img)

        # load label
        labels.append(labels_byte[label_offset])
        label_offset += 1

    return data_num, img_height, img_width, imgs, labels

def main():
    img_path = "../train-images.idx3-ubyte"
    label_path = "../train-labels.idx1-ubyte"

    data_num, img_height, img_width, imgs, labels = load_images_and_labels(img_path, label_path)

    print(f"data_num: {data_num}, img_height: {img_height}, img_width: {img_width}")
    print(f"Min label: {min(labels)}, Max label: {max(labels)}")

    # display image
    cv2.imwrite(f"{labels[data_num - 1]}.jpg", imgs[data_num - 1])

if __name__ == "__main__":
    main()
