import argparse
import pandas as pd
import matplotlib.pyplot as plt

parser = argparse.ArgumentParser(description='Plot benchmark CSV')
parser.add_argument('--input', type=str, help='CSV file')

args = parser.parse_args()

csv_data = pd.read_csv(args.input)

groups = [ item for item in csv_data.groupby('name') ]
labels = [ key for key, value in groups if not 'mean' in key and not 'median' in key and not 'stddev' in key ]

hwnd = []
for key, group in groups:
    if key in labels:
        x = group['real_time']
        line, = plt.plot(x.tolist(), label=key)
        hwnd.append(line)

plt.xticks([1, 500, 800], ["Teste1", "teste2", "teste3"])
plt.legend(handles=hwnd)
plt.show()



