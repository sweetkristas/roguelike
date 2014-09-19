from random import randint, choice, shuffle

ceiling  = '#'
floor    = '.'
pit      = 'X'
lava     = '~'
door     = 'D'
wall     = 'T'
corridor = '%'

min_room_size = 7
max_room_size = 15

min_corridor_length = 2
max_corridor_length = 10

class dungeon_generator(object):
    def __init__(self, width, height):
        self._width = width
        self._height = height
        self._map = []
        self._wall_list = []
        for y in range(0, self._height):
            self._map.append([ceiling,] * self._width)
    def __str__(self):
        out = ''
        for row in self._map:
            for col in row:
                out += col
            out += '\n'
        return out
    def generate(self):
        rs = self.get_random_room_size()
        # Places starting room
        self.place_room(((self._width-rs[0])/2, (self._height-rs[1])/2, rs[0], rs[1]))        
        feature_count = 0
        while feature_count < 40 and len(self._wall_list) != 0:
            if self.create_feature(): feature_count += 1
    def get_random_room_size(self):
        return (randint(min_room_size, max_room_size), randint(min_room_size, max_room_size))
    def place_room(self, r):
        if self.is_rect_filled(r, ceiling):
            self.create_room(r)
            return True
        return False
    def place_corridor(self, r):
        if self.is_rect_filled(r, ceiling):
            self.create_corridor(r)
            return True
        return False
    def is_rect_filled(self, r, c):
        if r[0] < 0 or r[1] < 0 or r[0]+r[2] < 0 or r[1]+r[3] < 0: return False
        if r[0] >= self._width or r[1] >= self._height or r[0]+r[2] >= self._width or r[1]+r[3] >= self._height: return False
        for y in range(r[1], r[1]+r[3]):
            for x in range(r[0], r[0]+r[2]):
                if self._map[y][x] != c: return False
        return True
    def create_room(self, r):
        for y in range(r[1], r[1]+r[3]):
            for x in range(r[0], r[0]+r[2]):
                self._map[y][x] = floor
                if (x == r[0]+r[2]-1 or x == r[0] and (y != r[1] or y != r[1]+r[3]-1)) \
                    or (y == r[1] or y == r[1]+r[3]-1 and (x != r[0] or x != r[0]+r[2]-1)):
                    self._map[y][x] = wall
                    self._wall_list.append((x,y))
        shuffle(self._wall_list)
    def create_corridor(self, r):       
        for y in range(r[1], r[1]+r[3]):
            for x in range(r[0], r[0]+r[2]):
                self._map[y][x] = corridor
                self._wall_list.append((x,y))
        shuffle(self._wall_list)
    def create_feature(self):
        ri = randint(1,100)
        if ri < 30:
            return self.create_corridor_feature()
        return self.create_room_feature()
    def choose_wall(self):
        if len(self._wall_list) == 0: return (False, [])
        wll = self._wall_list[0]
        self._wall_list = self._wall_list[1:]
        c = self._map[wll[1]][wll[0]]
        if c != wall and c != corridor:
            print 'Item at (%d,%d) not wall (%c)' % (wll[0], wll[1], self._map[wll[1]][wll[0]])
            return (False, [])
        return (True, wll)        
       
    def create_corridor_feature(self):
        corridor_length = randint(min_corridor_length, max_corridor_length)
        (ret, wall) = self.choose_wall()
        if ret == False: return False
        if wall[1] > 0 and self._map[wall[1]-1][wall[0]] == ceiling:
            # north
            corridor_location = (wall[0], wall[1]-1)
            rx = wall[0]
            ry = wall[1] - (corridor_length+1)
            rw = 1
            rh = corridor_length
        elif wall[1] < self._height-1 and self._map[wall[1]+1][wall[0]] == ceiling:
            # south
            corridor_location = (wall[0], wall[1]+1)
            rx = wall[0]
            ry = wall[1] + 2
            rw = 1
            rh = corridor_length
        elif wall[0] > 0 and self._map[wall[1]][wall[0]-1] == ceiling:
            # west
            corridor_location = (wall[0]-1, wall[1])
            rx = wall[0] - (corridor_length+1)
            ry = wall[1]
            rw = corridor_length
            rh = 1
        elif wall[0] < self._width-1 and self._map[wall[1]][wall[0]+1] == ceiling:
            # east
            corridor_location = (wall[0]+1, wall[1])
            rx = wall[0] + 2
            ry = wall[1]
            rw = corridor_length
            rh = 1
        else:
            print 'No dir from wall'
            return False
        if not self.place_corridor((rx,ry,rw,rh)):
            print 'Couldn\'t place corridor: (%d,%d,%d,%d)' % (rx,ry,rw,rh)
            return False
        self._map[corridor_location[1]][corridor_location[0]] = corridor
        if self._map[wall[1]][wall[0]] != corridor:
            self._map[wall[1]][wall[0]] = door        
        return True
    def create_room_feature(self):
        # creates a square room
        # choose a wall
        (ret, wall) = self.choose_wall()
        if ret == False: return False
        rs = self.get_random_room_size()
        if wall[1] > 0 and self._map[wall[1]-1][wall[0]] == ceiling:
            # north
            corridor_location = (wall[0], wall[1]-1)
            rx = wall[0] - rs[0]/2
            ry = wall[1] - (rs[1]+1)
        elif wall[1] < self._height-1 and self._map[wall[1]+1][wall[0]] == ceiling:
            # south
            corridor_location = (wall[0], wall[1]+1)
            rx = wall[0] - rs[0]/2
            ry = wall[1] + 2
        elif wall[0] > 0 and self._map[wall[1]][wall[0]-1] == ceiling:
            # west
            corridor_location = (wall[0]-1, wall[1])
            rx = wall[0] - (rs[0]+1)
            ry = wall[1] - rs[1]/2
        elif wall[0] < self._width-1 and self._map[wall[1]][wall[0]+1] == ceiling:
            # east
            corridor_location = (wall[0]+1, wall[1])
            rx = wall[0] + 2
            ry = wall[1] - rs[1]/2
        else:
            print 'No dir from wall'
            return False
        if not self.place_room((rx,ry,rs[0],rs[1])):
            print 'Couldn\'t place room: (%d,%d,%d,%d)' % (rx,ry,rs[0],rs[1])
            return False
        self._map[corridor_location[1]][corridor_location[0]] = corridor
        self._map[wall[1]][wall[0]] = door
        return True
def main(width, height):
    dg = dungeon_generator(width, height)
    dg.generate()
    print dg

if __name__ == '__main__':
    main(80, 80)
