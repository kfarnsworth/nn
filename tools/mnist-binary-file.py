
import numpy as np

def load_data(path):
    with np.load(path) as f:
        x_train, y_train = f['x_train'], f['y_train']
        x_test, y_test = f['x_test'], f['y_test']
        return (x_train, y_train), (x_test, y_test)

(train_images, train_labels), (test_images, test_labels) = load_data('mnist.npz')

images_count = len(train_images)
x_sizes = len(train_images[0])
y_sizes = len(train_images[0][0])
label_count = 10
label_set = list(set(train_labels))

with  open("mnist-training-data.bin", "wb") as f:
   ix = 0
   while ix < images_count:
        image = train_images[ix]
        label = train_labels[ix]
        arr = [ int(label) ]
        for row in image:
            for col in row:
                arr.append(int(col))
        f.write(bytearray(arr))
        ix+=1

