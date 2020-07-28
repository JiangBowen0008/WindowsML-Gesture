import onnx
import sys

from onnx_tf.backend import prepare


def main():
    print(len(sys.argv))
    print(str(sys.argv))
    if len(sys.argv) != 2:
        print("Wrong number of arguments")
        exit(1)
    
    name = str(sys.argv[1]).split(".")[0]
    print("Converting" + name)
    onnx_model = onnx.load(sys.argv[1])  # load onnx model
    tf_rep = prepare(onnx_model)  # prepare tf representation
    tf_rep.export_graph(name + ".pb")  # export the model


if __name__ == "__main__":
    print(onnx.__version__)
    main()