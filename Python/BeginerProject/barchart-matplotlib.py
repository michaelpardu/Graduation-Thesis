import matplotlib.pyplot as plt # type: ignore
# data to display on plots 
x = [1, 2, 3, 4, 5, 6, 7] 
y = [3, 2, 1, 4, 5, 6, 7] 

# This will plot a simple bar chart
plt.plot(x, y)

# Title to the plot
plt.title("line Chart")

# Adding the legends
plt.legend(["line"])
plt.show()
