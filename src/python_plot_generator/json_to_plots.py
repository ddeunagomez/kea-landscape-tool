import json
import math
import matplotlib as m
import matplotlib.pyplot as plt
from mpl_toolkits.axes_grid1 import make_axes_locatable
import numpy as np
import sys
import ast

def makeColormap(seq):
    """Return a LinearSegmentedColormap
    seq: a sequence of floats and RGB-tuples. The floats should be increasing
    and in the interval (0,1).
    """
    seq = [(None,) * 3, 0.0] + list(seq) + [1.0, (None,) * 3]
    cdict = {'red': [], 'green': [], 'blue': []}
    for i, item in enumerate(seq):
        if isinstance(item, float):
            r1, g1, b1 = seq[i - 1]
            r2, g2, b2 = seq[i + 1]
            cdict['red'].append([item, r1, r2])
            cdict['green'].append([item, g1, g2])
            cdict['blue'].append([item, b1, b2])
    return m.colors.LinearSegmentedColormap('CustomMap', cdict)

def plotMap(matrix, fig, ax):
    """ Plot a matrix as a HeatMap
    matrix: the Numpy array to be plotted
    fig: The figure object to use for plotting
    ax: the axis object for plotting
    """

    # Colormap for the legend
    cm = None
    #c = m.colors.ColorConverter().to_rgb
    #cm = makeColormap([(0,1,0), (1,1,0), 0.1, (1,1,0), (1,0.5,0), 0.66, (1,0.5,0),(1,0,0)])
    #cm = makeColormap([(0,0.011,0.176), (1,1,0), 0.1, (1,1,0), (1,0.5,0), 0.66, (1,0.5,0),(1,1,1)])
    
    # Build the heatmap
    heatmap = ax.pcolor(matrix, cmap=cm)

    # Format
    ax.set_aspect('equal')
    ax.set_frame_on(False)    # turn off the frame
    
    # put the major ticks at the middle of each cell
    #ax.set_yticks(np.arange(matrix.shape[0]) + 0.5, minor=False)
    #ax.set_xticks(np.arange(matrix.shape[1]) + 0.5, minor=False)

    # want a more natural, table-like display
    ax.invert_yaxis()
    #ax.xaxis.tick_top()
    
    #ax.set_xticklabels(range(0,matrix.shape[0]), minor=False)
    #ax.set_yticklabels(range(0,matrix.shape[1]), minor=False)
    ax.grid(False)

    for t in ax.xaxis.get_major_ticks():
	t.tick1On = False
	t.tick2On = False
    for t in ax.yaxis.get_major_ticks():
	t.tick1On = False
	t.tick2On = False

    #ax.get_xaxis().set_ticks(range(0,matrix.shape[0]+1,10))
    #ax.get_yaxis().set_ticks(range(0,matrix.shape[0]+1,10))

    divider = make_axes_locatable(ax)		
    cax = divider.append_axes("right", size="5%", pad=0.05)
    plt.colorbar(heatmap,cax=cax,ticks=[])
    return (fig,ax)

def plotSolution(solution_data, figure, axis):


    side = int(math.sqrt(int(solution_data["nodes"])))
    
    currents = ast.literal_eval(str(solution_data["node_currents"]))
    currents = np.asarray(currents)
    currents = currents.reshape((side, side))
    
    plotMap(currents,figure,axis)



def readSolutions(json_filename, plot_intermediate = False):

    data = json.load(open(json_filename))

    base_solution = data["base_solution"]
    initial_solution = data["initial_solution"]
    best_solution = data["best_solution"]
    
    if plot_intermediate:
        intermediate_count = int(data["solutions_count"])
    else:
        intermediate_count = 0

    number_plots = intermediate_count + 3 #base, initial and best
    columns = min(number_plots,10.0)
    rows = math.ceil(number_plots/columns)

    
    
    fig = plt.figure(figsize=(20,20))

    axis_base = fig.add_subplot(rows,columns,1)
    plt.title("Base Solution ("+str(base_solution["resistance"])+")")

    axis_initial = fig.add_subplot(rows,columns,2)
    plt.title("Initial Solution ("+str(initial_solution["resistance"])+")")

    axis_intermediates = {}
    for i in range(0, intermediate_count):
        axis_intermediates[i] = fig.add_subplot(rows,columns,3 + i)
        plt.title("Solution"+str(i)+" ("+str(initial_solution["resistance"])+")")

    axis_best = fig.add_subplot(rows,columns,rows*columns)
    plt.title("Best Solution ("+str(best_solution["resistance"])+")")
    
    fig.tight_layout()
    plt.subplots_adjust(wspace = 0.1 )

    

    plotSolution(base_solution,fig, axis_base)
    plotSolution(initial_solution,fig, axis_initial)

    for i in range(0, intermediate_count):
       plotSolution(data["solution_"+str(i)], fig, axis_intermediates[i])

    plotSolution(best_solution,fig, axis_best)
    
    plt.show()


if __name__ == "__main__":
    readSolutions(sys.argv[1])




