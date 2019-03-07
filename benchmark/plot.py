# -*- coding: utf-8 -*-
import argparse
import matplotlib.pyplot as plt
import json
import os

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

            if not benchmark['name'] in data[raw['context']['host_name']]:
                data[raw['context']['host_name']][benchmark['name']] = []
            data[raw['context']['host_name']][benchmark['name']].append(benchmark)


def get_cpu_time( value ):
    if value['time_unit'] == 'ns':
        return value['cpu_time'] / 10**-9
    if value['time_unit'] == R'µs':
        return value['cpu_time'] / 10**-6
    if value['time_unit'] == 'ms':
        return value['cpu_time'] / 10**-3
    return value['cpu_time']


hwnd = []
for host_name in data:
    for group in data[host_name]:
        cpu_time = [get_cpu_time(item) for item in data[host_name][group]]
        line, = plt.plot(cpu_time, label=group)
        hwnd.append(line)

plt.legend(handles=hwnd)
plt.ylabel('CPU time')
plt.show()



