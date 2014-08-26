from random import randint
from copy import deepcopy

def gen_symbolic(cave, symbols):
    w = len(cave[0])
    h = len(cave)
    ret_map = ''
    for y in range(0,h):
        for x in range(0,w):
            ret_map += symbols[cave[y][x]]
        ret_map += '\n'
    return ret_map

def process_neighbor(x, y, cave, wtype):
    w = len(cave[0])
    h = len(cave)
    if y < 0 or y >= h:
        if x < 0 or x >= h:
            if wtype == 0: return 1
        if wtype == 0: return 1
        return 0
    if x < 0 or x >= h:
        if wtype == 0: return 1
        return 0
    if cave[y][x] == wtype: return 1
    return 0
    
def count_neighbours(cave, x, y, wtype=0):
    """Counts the number of neighbouring items of the corresponding type.
    Border values are assumed to be wall, i.e. 0
    Assumed that x/y are within the boundaries
    """
    w = len(cave[0])
    h = len(cave)
    neighbors = 0

    neighbors += process_neighbor(x-1, y-1, cave, wtype)
    neighbors += process_neighbor(x-1, y+0, cave, wtype)
    neighbors += process_neighbor(x-1, y+1, cave, wtype)

    neighbors += process_neighbor(x+0, y-1, cave, wtype)
    neighbors += process_neighbor(x+0, y+0, cave, wtype)
    neighbors += process_neighbor(x+0, y+1, cave, wtype)

    neighbors += process_neighbor(x+1, y-1, cave, wtype)
    neighbors += process_neighbor(x+1, y+0, cave, wtype)
    neighbors += process_neighbor(x+1, y+1, cave, wtype)
    return neighbors
    
    
def automata(old_map, thresholds):
    w = len(old_map[0])
    h = len(old_map)
    new_map = deepcopy(old_map)
    wall_value = 0
    floor_value = 1
    for y in range(0, h):
        for x in range(0, w):
            new_map[y][x] = floor_value
            for thr_fn in thresholds:
                if thr_fn(count_neighbours(old_map, x, y, wall_value)):
                    new_map[y][x] = wall_value
    return new_map
    
def generate_cave(width, height, threshold, symbols='#.', passes={'iterations':5, 'thresholds':[lambda x: x >= 5]}):
    initial = []
    threshold = int(threshold * 100)
    # 1 for floor, 0 for a wall
    for y in range(0, height):
        x_array = []
        for x in range(0, width):
            val = 1
            if randint(0,99) < threshold:
                val = 0
            x_array.append(val)
        initial.append(x_array)
    new_map = deepcopy(initial)
    for m in passes:
        for n in range(0, m['iterations']):
            new_map = deepcopy(automata(new_map, m['thresholds']))
    return gen_symbolic(new_map, symbols)

if __name__ == '__main__':
    print generate_cave(50, 50, 0.35, '#.', [{'iterations':4, 'thresholds':[lambda x: x >= 5, lambda x: x < 1]}, {'iterations':2, 'thresholds':[lambda x: x >= 5]}])
