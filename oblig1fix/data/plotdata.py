import numpy as np
import matplotlib.pyplot as plt

# Dataset sizes and corresponding average elapsed times (seconds)
dataset_sizes = [1_000, 12_600, 3_00000, 1_000_000, 4_400_000]  # 2K and 4M letters 
elapsed_times = [0.0000000296, 0.000000352, 0.000000438, 0.000000788, 0.000001134]   # Approximate averages from given data

# Compute log2 of dataset sizes
log_dataset_sizes = np.log2(dataset_sizes)

# Create a reference O(log n) curve (scaled to match observed data)
scaling_factor = elapsed_times[-1] / log_dataset_sizes[-1]  # Scale to match the last data point
reference_curve = scaling_factor * log_dataset_sizes

# Plotting
plt.figure(figsize=(10, 6))
plt.plot(log_dataset_sizes, elapsed_times, 'o-', label='Observed Search Times (AVL Tree)')
plt.plot(log_dataset_sizes, reference_curve, 'r--', label='Reference $O(\\log n)$ Curve')

plt.xlabel('$\\log_2$(Dataset Size)')
plt.ylabel('Elapsed Time (seconds)')
plt.title('Search Time vs. $\\log_2$(Dataset Size) for AVL Tree')
plt.legend()
plt.grid(True)
plt.show()
