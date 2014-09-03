TILE_SIZE = 32

def test(wm, cx, cy, tex):
    cx *= TILE_SIZE
    cy *= TILE_SIZE
    if cx > tex['w']/2:
        src_x = tex['w']/2 - (cx - wm['w'])
        src_w = wm['w'] - src_x
        dst_x = 0
        dst_w = 0
    else:
        src_x = cx
        src_w = wm['w']
        dst_x = 0
        dst_w = wm['w']
        
    if cy > tex['h']/2:
        src_y = 0
        src_h = 0
        dst_y = 0
        dst_h = 0
    else:
        src_y = cy
        src_h = 0
        dst_y = 0
        dst_h = 0
    
    print 'src: %d,%d (%d,%d,%d,%d)' % (cx/TILE_SIZE, cy/TILE_SIZE, src_x, src_y, src_w, src_h)
    print 'dst: (%d,%d,%d,%d)' % (dst_x, dst_y, dst_w, dst_h)

if __name__ == '__main__':
    for x in range(0,16):
        test({'w':800, 'h':600}, x, 0, {'w':1024, 'h':1024})
