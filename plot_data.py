import numpy as np
import matplotlib.pyplot as plt


if __name__ == '__main__':
    data=np.loadtxt('aubatch_data.txt')
    x=np.arange(3)

    labels=['Avg Turnaround', 'Avg CPU', 'Avg Wait']
    fig, ax=plt.subplots()

    ax.bar(x-.25, data[0], color='b', width=.25, label='FCFS')
    ax.bar(x, data[1], color='g', width=.25, label='SJF')
    ax.bar(x+.25, data[2], color='r', width=.25, label='Priority')

    ax.set_xticks(range(3), labels)
    ax.set_ylabel('Time (seconds)')

    legend=ax.legend(loc='upper right', shadow=True)

    plt.show()

    plt.clf()

    labels=['FCFS', 'SJF', 'Priority']
    fig, ax=plt.subplots()
    throughputs=[.013099, .014165, .012331]
    ax.bar(x, throughputs)
    ax.set_xticks(range(3), labels)
    ax.set_ylabel('Time (seconds)')
    plt.show()

