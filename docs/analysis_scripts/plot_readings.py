import matplotlib.pyplot as plt
import numpy as np

# Data from earlier
configured_delay = np.array([0, 100, 200, 300, 400, 500])
rtt_avg = np.array([21.23, 184.48, 278.36, 380.23, 483.56, 581.81])
rtt_std = np.array([3.33, 29.83, 30.70, 30.32, 30.22, 32.03])

# Plot
plt.figure(figsize=(8, 5))
plt.errorbar(configured_delay, rtt_avg, yerr=rtt_std, fmt='-o', capsize=5, label="RTT (Avg ± Std Dev)")
plt.xlabel("Configured Network Delay (ms)")
plt.ylabel("RTT (ms)")
plt.title("RTT vs Configured Network Delay")
plt.grid(True, linestyle="--", alpha=0.6)
plt.legend()
plt.tight_layout()
plt.show()
