TILE_SIZE = 32

def test(wm, cx, cy, tex):
    cx *= TILE_SIZE
    cy *= TILE_SIZE
    
    if wm['w'] > tex['w']:
        src_x = 0
        src_w = tex['w']
        dst_x = (wm['w'] - tex['w'])/2
        dst_w = tex['w']
    else:
        if cx < wm['w']/2:
            src_x = 0
        elif cx > tex['w']-wm['w']/2:
            src_x = tex['w']-wm['w']
        else:
            src_x = cx - wm['w']/2
        src_w = wm['w']
        dst_x = 0
        dst_w = wm['w']

    if wm['h'] > tex['h']:
        src_y = 0
        src_h = tex['h']
        dst_y = (wm['h'] - tex['h'])/2
        dst_h = tex['h']
    else:
        if cy < wm['h']/2:
            src_y = 0
        elif cy > tex['h']-wm['h']/2:
            src_y = tex['h']-wm['h']
        else:
            src_y = cy - wm['h']/2
        src_h = wm['h']
        dst_y = 0
        dst_h = wm['h']
        
    print 'src: %d,%d (%d,%d,%d,%d)' % (cx/TILE_SIZE, cy/TILE_SIZE, src_x, src_y, src_w, src_h)
    print 'dst: (%d,%d,%d,%d)' % (dst_x, dst_y, dst_w, dst_h)

if __name__ == '__main__':
    for y in range(0,33):
        for x in range(0,33):
            test({'w':800, 'h':600}, x, y, {'w':1024, 'h':1024})
