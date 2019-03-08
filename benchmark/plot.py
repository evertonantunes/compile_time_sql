# -*- coding: utf-8 -*-
import argparse
import matplotlib.pyplot as plt
import matplotlib.markers as markers
import json
import os
import random

parser = argparse.ArgumentParser(description='Plot benchmark')
parser.add_argument('--directory', type=str, help='CSV file')

args = parser.parse_args()

data = {}

if args.directory != '':
    for file_name in os.listdir(args.directory):
        raw = json.loads(open('{}/{}'.format(args.directory, file_name), 'r').read())

        if not raw['context']['host_name'] in data:
            data[raw['context']['host_name']] = {}

        for benchmark in raw['benchmarks']:
            if 'stddev' in benchmark['name'] or 'mean' in benchmark['name'] or 'median' in benchmark['name']:
                continue

            benchmark['date'] = raw['context']['date']
            if not benchmark['name'] in data[raw['context']['host_name']]:
                data[raw['context']['host_name']][benchmark['name']] = []
            data[raw['context']['host_name']][benchmark['name']].append(benchmark)


def get_cpu_time( value ):
    if value['time_unit'] == 'ns':
        return value['cpu_time'] / 10**-9
    if value['time_unit'] == R'µs':
        return (value['cpu_time'] / 10**-6) * 1000
    if value['time_unit'] == 'ms':
        return (value['cpu_time'] / 10**-3) * 10**6
    return value['cpu_time']


def random_color():
    r = lambda: random.randint(0,255)
    return '#%02X%02X%02X' % (r(),r(),r())


fig, ax = plt.subplots()
hwnd = []

for host_name in data:
    for group in data[host_name]:
        cpu_time = [get_cpu_time(item) for item in data[host_name][group]]
        line, = ax.plot(cpu_time, label=group, marker='.')
        hwnd.append(line)


ticks = []
for host_name in data:
    for group in data[host_name]:
        host_ticks = [item['date'] for item in data[host_name][group]]
        if len(ticks) < len(host_ticks):
            ticks = host_ticks

ticks2 = []
first = 0
count = 0
last = None
for item in ticks:
    if item != last and count > 0:
        print('{} = {}'.format(item, last))
        ax.axvspan(first, count, facecolor=random_color(), alpha=0.5)
        count += 1
        first = count
        last = item
        ticks2.append(item)
    else:
        count += 1
        last = item
        if len(ticks2) == 0:
            ticks2.append(item)
        else:
            ticks2.append('{}'.format(count))
ax.axvspan(first-1, count, facecolor=random_color(), alpha=0.5)


ax.set(ylabel='CPU time ns', title='benchmark')
ax.legend(handles=hwnd)
ax.grid()
plt.xticks(range(0, len(ticks2)), ticks2, rotation=45, fontsize=8)

plt.show()



