
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

with  open("mnist-training-data.json", "w") as f:
    f.write("{\n")
    f.write("    \"inputs\": " + str(x_sizes * y_sizes) + ",\n")
    f.write("    \"outputs\": " + str(label_count) + ",\n")
    f.write("    \"input_params\": {\n")
    f.write("        \"type\": \"decimal\"\n")
    f.write("    },\n")
    f.write("    \"output_params\": {\n")
    f.write("        \"set\": [")
    first = True
    for num in label_set:
        if not first:
            f.write(", ")
        f.write("\"" + str(num) + "\"");
        first = False
    f.write("]\n")
    f.write("    },\n")
    f.write("    \"data\": [\n")
    ix = 0
    first = True
    while ix < images_count:
        if not first:
            f.write(",\n")
        f.write("        {\"input\": \"")
        image = train_images[ix]
        label = train_labels[ix]
        for row in image:
            for col in row:
                f.write(str(col) + " ")
                #if col != 0:
                #    print("X", end="")
                #else:
                #    print(" ", end="")
            #print("");
        f.write("\", \"output\": \"" + str(label) + "\"}")
        first = False
        ix+=1
    f.write("\n    ]\n")
    f.write("}")

