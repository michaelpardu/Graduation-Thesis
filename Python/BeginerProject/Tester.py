import numpy as np
import matplotlib.pyplot as plt
import csv

def generate_data(start, stop, count, deviation):
    # Generate base data with a gradual increase
    base_data = np.linspace(start, stop, count)
    
    # Add deviation to each base data point
    noisy_data = base_data + np.random.uniform(-deviation, deviation, size=base_data.size)
    
    return noisy_data

def save_to_csv(data, filename):
    # Save data to CSV file
    with open(filename, mode='w', newline='') as file:
        writer = csv.writer(file)
        writer.writerow(['Index', 'Measurement (mm)'])  # Header
        for index, value in enumerate(data):
            writer.writerow([index, value])

# Parameters
start = 42       # Starting value in mm
stop = 24         # Ending value in mm
count = 100      # Number of data points
deviation = 1   # Deviation in mm

# Generate the data
data = generate_data(start, stop, count, deviation)

# Save data to CSV
csv_filename = 'file5.csv'
save_to_csv(data, csv_filename)

# Plotting the data for visualization
plt.figure(figsize=(10, 5))
plt.plot(data, label='Data')
plt.ylim(0, 50)
plt.xlabel('Index')
plt.ylabel('Measurement (mm)')
plt.title('Generated Data with Gradual Increase and Deviation')
plt.legend()
plt.show()

print(f'Data has been saved to {csv_filename}')
