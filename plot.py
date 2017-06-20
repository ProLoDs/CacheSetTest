import struct
import statistics as s

data=[[] for x in xrange(2)]
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
        data[num % 2].append((num % 2,delay))
        x.append(num % 5)
        y.append(delay)
    
for row in data:
    print s.mean(row[1])


    