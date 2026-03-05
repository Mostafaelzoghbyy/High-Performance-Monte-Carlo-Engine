import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation

df = pd.read_csv("paths.csv", header=None)
paths = df.values
num_paths, num_steps = paths.shape

print(f"loaded {num_paths} paths, {num_steps} steps")

plt.style.use('dark_background')
fig, ax = plt.subplots(figsize=(12, 7))
fig.patch.set_facecolor('#0a0a0a')
ax.set_facecolor('#0a0a0a')

ax.set_xlim(0, num_steps)
ax.set_ylim(np.min(paths) * 0.9, np.max(paths) * 1.1)

ax.set_title("Monte Carlo — GBM paths", color='#aaaaaa', fontsize=12, pad=14)
ax.set_xlabel("trading days", color='#555555')
ax.set_ylabel("price ($)", color='#555555')
ax.tick_params(colors='#555555')

lines = [ax.plot([], [], lw=0.6, alpha=0.25, color='#00FFCC')[0] for _ in range(num_paths)]

# mean path on top
mean_path = paths.mean(axis=0)
mean_line, = ax.plot([], [], lw=1.6, color='#ff6b35', zorder=5)

def init():
    for ln in lines:
        ln.set_data([], [])
    mean_line.set_data([], [])
    return lines + [mean_line]

def animate(frame):
    x = np.arange(frame + 1)
    for i, ln in enumerate(lines):
        ln.set_data(x, paths[i, :frame + 1])
    mean_line.set_data(x, mean_path[:frame + 1])
    return lines + [mean_line]

ani = animation.FuncAnimation(fig, animate, init_func=init,
                               frames=num_steps, interval=10, blit=True)

plt.tight_layout()
plt.show()