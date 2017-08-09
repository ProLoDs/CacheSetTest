import struct
import statistics as s
import sys

data=[[] for x in xrange(2)]
flush = []
no_flush = []
y= []
def readChunks(filename):
    with open(filename) as infile:
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

if len(sys.argv) > 2:
    file1 = sys.argv[1]
    file2 = sys.argv[2]
else:
    print "Usage: plot.py <noflushfile> <flushfile>"
    sys.exit(0)

for num,delay in zip(inc(),readChunks(file1)):
    no_flush.append(delay)


for num,delay in zip(inc(),readChunks(file2)):
    flush.append(delay)
    

print "median:"
print "flush",s.median(flush)
print "no flush",s.median(no_flush)
print "mean:"
print "flush",s.mean(flush)
print "no flush",s.mean(no_flush)
    
