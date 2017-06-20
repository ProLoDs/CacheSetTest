import struct
import statistics as s
import numpy as np
import numpy.random
import matplotlib.pyplot as plt
import sphviewer as sph
data=[[] for x in xrange(5)]
x = []
y= []
def readChunks():
    with open("out.bin") as infile:
        while True:
            data = infile.read(8)
            if not data:
                break
            yield struct.unpack("<Q",data)[0]
            
            

def inc():
    a = -1
    while True:
        a+=1
        yield a

for num,delay in zip(inc(),readChunks()):
    if delay < 4000:
        data[num % 5].append((num % 5,delay))
        x.append(num % 5)
        y.append(delay)
    
#for row in data:
#    print s.mean(row[1])
def myplot(x, y, nb=32, xsize=500, ysize=500):   
    xmin = np.min(x)
    xmax = np.max(x)
    ymin = np.min(y)
    ymax = np.max(y)

    x0 = (xmin+xmax)/2.
    y0 = (ymin+ymax)/2.

    pos = np.zeros([3, len(x)])
    pos[0,:] = x
    pos[1,:] = y
    w = np.ones(len(x))

    P = sph.Particles(pos, w, nb=nb)
    S = sph.Scene(P)
    S.update_camera(r='infinity', x=x0, y=y0, z=0, 
                    xsize=xsize, ysize=ysize)
    R = sph.Render(S)
    R.set_logscale()
    img = R.get_image()
    extent = R.get_extent()
    for i, j in zip(xrange(4), [x0,x0,y0,y0]):
        extent[i] += j
    print extent
    return img, extent

heatmap_16, extent_16 = myplot(x,y, nb=16)
plt.imshow(heatmap_16, extent=extent_16, origin='lower', aspect='auto')
plt.show

    