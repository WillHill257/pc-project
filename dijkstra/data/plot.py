# adapted from Steve James

import matplotlib.pyplot as plt
from numpy import double

if __name__ == "__main__":

    vertices = "8192"
    filename = vertices + "-efficiencies"
    filepath = filename + ".txt"
    names = [
        "MPI (Density 35%)",
        "OpenMP (Density 35%)",
        "MPI (Density 90%)",
        "OpenMP (Density 90%)",
    ]
    x = list(range(2, 20 + 1, 2))  # this is the range of input sizes tested
    with open(filepath, "r") as file:
        for i, line in enumerate(file):
            y = list(map(double, line.split(",")))
            plt.plot(x, y, label=names[i])

    plt.xlabel("Number of Processes")
    plt.ylabel("Efficiency")
    plt.title("Number of Processes vs. Efficiency\n" + vertices + " Vertices")
    plt.legend()
    plt.xlim([2, 20])
    plt.xticks(range(0, 22 + 1, 2))
    plt.savefig("../images/" + filename + ".pdf")
    plt.show()
