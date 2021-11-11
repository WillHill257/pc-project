# adapted from Steve James

import matplotlib.pyplot as plt
from numpy import double

if __name__ == "__main__":

    filepath = "efficiency.txt"
    names = [
        "400 x 400",
        "800 x 800",
        "1600 x 1600",
        "3200 x 3200",
        "4800 x 4800",
        "6400 x 6400",
    ]
    x = list(range(2, 20 + 1, 2))  # this is the range of input sizes tested
    with open(filepath, "r") as file:
        for i, line in enumerate(file):
            y = list(map(double, line.split(",")))
            plt.plot(x, y, label=names[i])

    plt.xlabel("Number of Processes")
    plt.ylabel("Efficiency")
    plt.title("Number of Processes vs. Efficiency")
    plt.legend()
    plt.xlim([2, 20])
    plt.xticks(range(0, 22 + 1, 2))
    plt.savefig("../images/efficiency.pdf")
    plt.show()
