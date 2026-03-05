import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation

print("Reading paths.csv...")
df = pd.read_csv("paths.csv", header=None)
paths = df.values 
num_paths, num_steps = paths.shape

plt.style.use('dark_background')
fig, ax = plt.subplots(figsize=(12, 7))
fig.patch.set_facecolor('#000000')
ax.set_facecolor('#000000')

ax.set_xlim(0, num_steps)
ax.set_ylim(np.min(paths) * 0.9, np.max(paths) * 1.1)

ax.set_title("Monte Carlo Asset Simulation | Modern C++ Engine", color='#00FFCC', fontsize=14, pad=20)
ax.set_xlabel("Trading Days", color='gray')
ax.set_ylabel("Price ($)", color='gray')

lines = [ax.plot([], [], lw=0.8, alpha=0.3, color='#00FFCC')[0] for _ in range(num_paths)]

def init():
    for line in lines:
        line.set_data([], [])
    return lines

def animate(frame):
    x = np.arange(frame + 1)
    for i in range(num_paths):
        lines[i].set_data(x, paths[i, :frame + 1])
    return lines

print(f"Animating {num_paths} paths... please wait.")

ani = animation.FuncAnimation(fig, animate, init_func=init, frames=num_steps, interval=10, blit=True)

plt.show()